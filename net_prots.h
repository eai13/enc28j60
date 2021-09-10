#ifndef NET_PROTOCOLS_H
#define NET_PROTOCOLS_H

#include "ethernet.h"
#include "stdint.h"

#define inv_16bit(word) ((((word) >> 8) & 0xFF) | \
                         (((word) << 8) & 0xFF00))

#define inv_32bit(word) ((((word) << 24) & 0xFF000000) | \
                         (((word) << 8) & 0xFF0000) | \
                         (((word) >> 8) & 0xFF00) | \
                         (((word) >> 24) & 0xFF))

#define ip_set(ip0, ip1, ip2, ip3) (((uint32_t)(ip0)) | ((uint32_t)(ip1 << 8)) | \
                                    ((uint32_t)(ip2 << 16)) | ((uint32_t)(ip3 << 24)))

#pragma pack(push, 1)
typedef struct ethernet_packet{
    uint8_t     THA[6];         // Target MAC
    uint8_t     SHA[6];         // Sender MAC
    uint16_t    TYPE;           // Upper protocol
    uint8_t     Payload[];      // Payload (data)
} ethernet_packet_t;
#pragma pack(pop)

uint8_t Ethernet_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);
uint8_t Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length);

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

uint8_t ARP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);

#pragma pack(push, 1)
// BUG: fix the bit fields
typedef struct IP_packet{
    uint8_t     IHL         : 4;    // Protocol version
    uint8_t     VERSION     : 4;    // Header size
    uint8_t     DSCP        : 5;    // Differentiated services code point
    uint8_t     ECN         : 3;    // Explicit congestion notification
    uint16_t    PACK_LEN    : 16;   // Packet length
    uint16_t    FRAG_ID     : 16;   // Packet fragments identification
    uint16_t    CONT_FLAGS  : 3;    // Fragmentation controls flags
    uint16_t    FRAG_OFFSET : 13;   // Fragment offset
    uint8_t     TTL         : 8;    // Time to live
    uint8_t     PROTOCOL    : 8;    // Contents protocol type
    uint16_t    CSUM        : 16;   // Header CRC
    uint32_t    SPA         : 32;   // Sender IP
    uint32_t    TPA         : 32;   // Target IP
    uint8_t     Payload[];          // Payload (data)
} IP_packet_t;
#pragma pack(pop)

uint8_t     IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);
uint8_t     IP_Reply(ethernet_packet_t * eth_pack, uint16_t length);
uint16_t    IP_CSum(uint32_t sum, uint8_t * buf, size_t length);

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

uint8_t ICMP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length);

#pragma pack(push, 1)
typedef struct UDP_packet{
    uint32_t orig_IP;
    uint32_t dest_IP;
    uint16_t protocol;
    uint16_t packet_len;
    uint16_t orig_port;
    uint16_t dest_port;
    uint16_t packet_leng;
    uint8_t data[];
} UDP_packet_t;
#pragma pack(pop)

// uint8_t UDP_PacketProc(UDP_packet_t * udp_pack, uint16_t length);
// uint8_t UDP_Reply(ethernet_packet_t * eth_pack, uint16_t length);

#endif