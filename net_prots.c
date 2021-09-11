#include "net_prots.h"
#include "stdint.h"

#define ETH_ARP_TYPE    inv_16bit(0x0806)
#define ETH_IP_TYPE     inv_16bit(0x0800)

#define ARP_HTYPE_ETHERNET  inv_16bit(0x0001)
#define ARP_PTYPE_IPv4      inv_16bit(0x0800)
#define ARP_OPER_REQUEST    inv_16bit(1)
#define ARP_OPER_RESPONSE   inv_16bit(2)

#define IP_PROTOCOL_ICMP    0x01
#define IP_PROTOCOL_TCP     0x06
#define IP_PROTOCOL_UDP     0x11

#define ICMP_TYPE_ECHO_REQUEST  8
#define ICMP_TYPE_ECHO_REPLY    0

uint32_t ip_addr = ip_set(192, 168, 0, 5);
uint8_t mac_addr[6] = MAC_ADDR;

uint8_t str[128];

///////////////////////////////
// Ethernet frame processing //
///////////////////////////////

/**
 * @brief ethernet send package
 * @param eth_pack ethernet packet
 * @param length packet size
 * @return uint8_t return state 
 */
static inline uint8_t Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Setting up the orig and dest MACs
    memcpy(eth_pack->THA, eth_pack->SHA, 6);
    memcpy(eth_pack->SHA, mac_addr, 6);
    // Pushing the packet to the buffer
    return Eth_SendPacket((void *)eth_pack, length + ETH_PACKET_STRUCT_SIZE);
}

uint8_t Ethernet_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    if (length < ETH_PACKET_STRUCT_SIZE){
        print_er("Ethernet frame error\r\n");
        return FRAME_ERROR;
    }
    if (eth_pack->TYPE == ETH_ARP_TYPE){
        return ARP_PacketProc(eth_pack, length - ETH_PACKET_STRUCT_SIZE);
    }
    else if (eth_pack->TYPE == ETH_IP_TYPE){
        return IP_PacketProc(eth_pack, length - ETH_PACKET_STRUCT_SIZE);
    }
    else return FRAME_ERROR;
}

//////////////////////////
// ARP frame processing //
//////////////////////////
static inline uint8_t ARP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the ARP package
    ARP_packet_t * arp_pack = (void *)(eth_pack->Payload);
    // Checking the packet
    if ((length < ARP_PACKET_STRUCT_SIZE) |
        (arp_pack->HTYPE != ARP_HTYPE_ETHERNET) |
        (arp_pack->PTYPE != ARP_PTYPE_IPv4) |
        (arp_pack->OPER != ARP_OPER_REQUEST) |
        (arp_pack->TPA != ip_addr)){
        print_er("ARP frame error\r\n");
        return FRAME_ERROR;
    }
    
    // Operation type setting
    arp_pack->OPER = ARP_OPER_RESPONSE;
    
    // Target and own ip setting
    arp_pack->TPA = arp_pack->SPA;
    arp_pack->SPA = ip_addr;

    // Target and own MAC setting
    memcpy(arp_pack->THA, arp_pack->SHA, 6);
    memcpy(arp_pack->SHA, mac_addr, 6);

    return Ethernet_Reply(eth_pack, ARP_PACKET_STRUCT_SIZE);
}

//////////////////////////
// IP packet processing //
//////////////////////////
static inline uint8_t IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    
    // Checking the packet
    if ((ip_pack->VERSION != 0x04) |
        (ip_pack->IHL != 0x05) |
        (ip_pack->TPA != ip_addr)){
        print_er("IP frame error\r\n");
        return FRAME_ERROR;
    }
    // Calculating payload size
    length = inv_16bit(ip_pack->PACK_LEN) - IP_PACKET_STRUCT_SIZE;
    // Checking the above protocol
    if (ip_pack->PROTOCOL == IP_PROTOCOL_ICMP){
        return ICMP_PacketProc(eth_pack, length);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_UDP){
        print_wr("UDP not ready\r\n"); // NOT WORKS: add UDP
        return FRAME_ERROR;
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_TCP){
        print_wr("IP not ready\r\n"); // NOT WORKS: add TCP
        return FRAME_ERROR;
    }
    else{
        return FRAME_ERROR;
    }
}

static inline uint8_t IP_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);

    // Setting up the IP packet
    ip_pack->PACK_LEN = inv_16bit(length + IP_PACKET_STRUCT_SIZE);
    ip_pack->FRAG_ID = 0;
    ip_pack->FRAG_OFFSET = 0;
    ip_pack->TTL = 64;
    ip_pack->CSUM = 0;
    ip_pack->TPA = ip_pack->SPA;
    ip_pack->SPA = ip_addr;
    ip_pack->CSUM = IP_CSum((void *)ip_pack, IP_PACKET_STRUCT_SIZE);

    // Add to the ethernet packet
    return Ethernet_Reply((void *)eth_pack, length + IP_PACKET_STRUCT_SIZE);
}

/**
 * @brief Checksum calculations
 * @param sum 
 * @param buf 
 * @param length 
 * @return uint16_t 
 */
static inline uint16_t IP_CSum(uint8_t * buf, size_t length){
    uint32_t sum = 0;
    while(length >= 2){
        sum += ((uint16_t)*buf << 8) | *(buf + 1);
        buf += 2;
        length -= 2;
    }
    if (length) sum += (uint16_t)*buf << 8;
    while(sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~inv_16bit((uint16_t)sum);
}

////////////////////////////
// ICMP packet processing //
////////////////////////////
static inline uint8_t ICMP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    ICMP_packet_t * icmp_pack = (void *)(ip_pack->Payload);

    // Packet length check
    if (length < ICMP_PACKET_STRUCT_SIZE){
        print_er("ICMP frame error\r\n");
        return FRAME_ERROR;
    }
    // Reply to the host
    if (icmp_pack->TYPE == ICMP_TYPE_ECHO_REQUEST){
        icmp_pack->TYPE = ICMP_TYPE_ECHO_REPLY;
        icmp_pack->CSUM += ICMP_TYPE_ECHO_REQUEST;
        return IP_Reply(eth_pack, length);
    }
    else return FRAME_ERROR;
}
