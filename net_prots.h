#ifndef NET_PROTOCOLS_H
#define NET_PROTOCOLS_H

#include "ethernet.h"
#include "ethernet_regs.h"
#include <stdint.h>

#define FRAME_ERROR    0x01
#define FRAME_OK       0x00

#define ip_set(ip0, ip1, ip2, ip3) (((uint32_t)(ip3) << 24) | ((uint32_t)(ip2) << 16) | ((uint32_t)(ip1) << 8) | ((uint32_t)(ip0)))

#define LOCAL_IP ip_set(192, 168, 0, 5)
#define UDP_PORT 5555

#define inv_16bit(word) (((((uint16_t)word) >> 8) & 0xFF) | \
                         ((((uint16_t)word) << 8) & 0xFF00))

#define inv_32bit(word) (((((uint32_t)word) << 24) & 0xFF000000) | \
                         ((((uint32_t)word) << 8) & 0xFF0000) | \
                         ((((uint32_t)word) >> 8) & 0xFF00) | \
                         ((((uint32_t)word) >> 24) & 0xFF))

uint8_t network_buffer[512];

/**
 * @brief Ethernet protocol
 * ////////////////////////
 */
#define ETH_PACKET_STRUCT_SIZE  14

#define ETH_ARP_TYPE    inv_16bit(0x0806)
#define ETH_IP_TYPE     inv_16bit(0x0800)

#pragma pack(push, 1)
typedef struct ethernet_packet{
    uint8_t     THA[6];         // Target MAC
    uint8_t     SHA[6];         // Sender MAC
    uint16_t    TYPE;           // Upper protocol
    uint8_t     Payload[];      // Payload (data)
} ethernet_packet_t;
#pragma pack(pop)

void Ethernet_PacketProc(ethernet_packet_t * eth_pack, uint16_t length, uint8_t * msg, uint16_t msg_size);
void Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length);



/**
 * @brief ARP protocol
 * ///////////////////
 */
#define ARP_PACKET_STRUCT_SIZE  28

#define ARP_CACHE_SIZE  5

#define ARP_HTYPE_ETHERNET  inv_16bit(0x0001)

#define ARP_PTYPE_IPv4      inv_16bit(0x0800)

#define ARP_OPER_REQUEST    inv_16bit(0x0001)
#define ARP_OPER_RESPONSE   inv_16bit(0x0002)

#define ARP_HLEN_MAC        0x06

#define ARP_PLEN_IP         0x04

#pragma pack(push, 1)
typedef struct ARP_packet{
    uint16_t    HTYPE;  // Hardware type
    uint16_t    PTYPE;  // Protocol type
    uint8_t     HLEN;   // Hardware address length
    uint8_t     PLEN;   // Protocol address length
    uint16_t    OPER;   // Operation type
    uint8_t     SHA[6]; // Sender MAC
    uint32_t    SPA;    // Sender IP
    uint8_t     THA[6]; // Target MAC
    uint32_t    TPA;    // Target IP
} ARP_packet_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ARP_MAC_IP_couple{
    uint32_t IP;
    uint8_t MAC[6];
} ARP_MAC_IP_couple_t;

ARP_MAC_IP_couple_t ARP_cache[ARP_CACHE_SIZE];

uint8_t * ARP_MACSearch(uint32_t source_ip);
void ARP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);
uint8_t * ARP_MACResolver(uint32_t source_ip);



/**
 * @brief IP protocol
 * //////////////////
 */
#define IP_PACKET_STRUCT_SIZE   20

#define IP_PROTOCOL_ICMP            0x01
#define IP_PROTOCOL_TCP             0x06
#define IP_PROTOCOL_UDP             0x11

#define IP_IHL_DEFAULT              0x5

#define IP_VERSION_DEFAULT          0x4

#define IP_ECN_DEFAULT              0x00

#define IP_DSCP_DEFAULT             0x00

#define IP_FRAG_OFFST1_DEFAULT      0x00
#define IP_FRAG_OFFST2_DEFAULT      0x00

#define IP_FLAGS_NO_FRAGMENT        0x2

#define IP_TTL_64                   0x40

#pragma pack(push, 1)
// BUG: fix the bit fields (fixed, not tested)
typedef struct IP_packet{
    uint8_t     IHL         : 4;    // Protocol version
    uint8_t     VERSION     : 4;    // Header size
    uint8_t     ECN         : 4;    // Explicit congestion notification
    uint8_t     DSCP        : 4;    // Something else
    uint16_t    PACK_LEN    : 16;   // Packet length
    uint16_t    FRAG_ID     : 16;   // Packet fragments identification
    uint16_t    FRAG_OFFST1 : 5;    // Fragment offset 1
    uint16_t    FLAGS       : 3;    // Flags
    uint16_t    FRAG_OFFST2 : 8;    // Fragment offset 2
    uint8_t     TTL         : 8;    // Time to live
    uint8_t     PROTOCOL    : 8;    // Contents protocol type
    uint16_t    CSUM        : 16;   // Header CRC
    uint32_t    SPA         : 32;   // Sender IP
    uint32_t    TPA         : 32;   // Target IP
    uint8_t     Payload[];          // Payload (data)
} IP_packet_t;
#pragma pack(pop)

void     IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length, uint8_t * msg, uint16_t msg_size);
void     IP_Reply(ethernet_packet_t * eth_pack, uint16_t length);
uint16_t IP_CSum(uint32_t sum, uint8_t * buf, size_t length);



/**
 * @brief UDP protocol
 * ///////////////////
 */
#define UDP_PACKET_STRUCT_SIZE  8

#pragma pack(push, 1)
typedef struct UDP_packet{
    uint16_t SP;
    uint16_t TP;
    uint16_t LENGTH;
    uint16_t CSUM;
    uint8_t Payload[];
} UDP_packet_t;
#pragma pack(pop)

void UDP_PacketProc(IP_packet_t * ip_pack, uint16_t length, uint8_t * msg, uint16_t msg_size);
void UDP_Reply(UDP_packet_t * udp_pack, uint16_t length);
void UDP_PacketSend(uint32_t targ_ip, uint16_t targ_port, uint8_t * msg, uint16_t msg_size);



/**
 * @brief ICMP protocol
 * ////////////////////
 */
#define ICMP_PACKET_STRUCT_SIZE 8 

#define ICMP_TYPE_ECHO_REQUEST  8
#define ICMP_TYPE_ECHO_REPLY    0

#pragma pack(push, 1)
typedef struct ICMP_packet{
    uint8_t TYPE;       // Packet type
    uint8_t CODE;       // Packet code
    uint16_t CSUM;      // Packet checksum
    uint16_t PACK_ID;   // Packet ID
    uint16_t PACK_NUM;  // Packet number
    uint8_t Payload[];  // Payload (data)
} ICMP_packet_t;
#pragma pack(pop)

void ICMP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);

#endif