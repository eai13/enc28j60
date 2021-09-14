#include "net_prots.h"

extern uint8_t network_buffer[];

void IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length, uint8_t * msg, uint16_t msg_size){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    
    // Checking the packet
    if ((ip_pack->VERSION != 0x04) |
        (ip_pack->IHL != 0x05) |
        (ip_pack->TPA != LOCAL_IP)){
#ifdef ETHERNET_DEBUG
        print_er("IP frame error\r\n");
#endif
    }
    // Calculating payload size
    length = inv_16bit(ip_pack->PACK_LEN) - IP_PACKET_STRUCT_SIZE;
    // Checking the above protocol
    if (ip_pack->PROTOCOL == IP_PROTOCOL_ICMP){
        ICMP_PacketProc(eth_pack, length);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_UDP){
        UDP_PacketProc(ip_pack, length, msg, msg_size);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_TCP){
#ifdef ETHERNET_DEBUG
        print_er("TCP not ready\r\n"); // NOT WORKS: add TCP
#endif
    }
    else{
#ifdef ETHERNET_DEBUG
        print_wr("No such IP protocol\r\n");
#endif
    }
}

void IP_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);

    // Setting up the IP packet
    ip_pack->PACK_LEN = inv_16bit(length + IP_PACKET_STRUCT_SIZE);
    ip_pack->FRAG_ID = 0;
    ip_pack->FRAG_OFFST1 = 0;
    ip_pack->FRAG_OFFST2 = 0;
    ip_pack->TTL = 64;
    ip_pack->CSUM = 0;
    ip_pack->TPA = ip_pack->SPA;
    ip_pack->SPA = LOCAL_IP;
    ip_pack->CSUM = IP_CSum(0, (void *)ip_pack, IP_PACKET_STRUCT_SIZE);

    // Add to the ethernet packet
    Ethernet_Reply((void *)eth_pack, length + IP_PACKET_STRUCT_SIZE);
}

/**
 * @brief Checksum calculations
 * @param sum 
 * @param buf 
 * @param length 
 * @return uint16_t 
 */
uint16_t IP_CSum(uint32_t sum, uint8_t * buf, size_t length){
    // uint8_t str[63];
    while(length >= 2){
        // snprintf(str, 63, "%x\r\n", ((uint16_t)*buf << 8) | *(buf + 1));
        // print_in(str);
        sum += ((uint16_t)*buf << 8) | *(buf + 1);
        buf += 2;
        length -= 2;
    }
    if (length) sum += (uint16_t)*buf << 8;
    while(sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~inv_16bit((uint16_t)sum);
}