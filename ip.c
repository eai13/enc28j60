#include "net_prots.h"

extern uint8_t network_buffer[];

uint8_t IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    
    // Checking the packet
    if ((ip_pack->VERSION != 0x04) |
        (ip_pack->IHL != 0x05) |
        (ip_pack->TPA != LOCAL_IP)){
        print_er("IP frame error\r\n");
        // return FRAME_ERROR;
    }
    
    uint8_t str[128];
    snprintf(str, 127, "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x\r\n", ip_pack->VERSION, ip_pack->IHL, ip_pack->DSCP, ip_pack->ECN, ip_pack->PACK_LEN, ip_pack->FRAG_ID, ip_pack->FLAGS, ip_pack->FRAG_OFFSET, ip_pack->TTL, ip_pack->PROTOCOL);
    print_in(str);
    // Calculating payload size
    length = inv_16bit(ip_pack->PACK_LEN) - IP_PACKET_STRUCT_SIZE;
    // Checking the above protocol
    if (ip_pack->PROTOCOL == IP_PROTOCOL_ICMP){
        return ICMP_PacketProc(eth_pack, length);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_UDP){
        return UDP_PacketProc(ip_pack, 0);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_TCP){
        print_wr("TCP not ready\r\n"); // NOT WORKS: add TCP
        return FRAME_ERROR;
    }
    else{
        return FRAME_ERROR;
    }
}

uint8_t IP_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);

    // Setting up the IP packet
    ip_pack->PACK_LEN = inv_16bit(length + IP_PACKET_STRUCT_SIZE);
    ip_pack->FRAG_ID = 0;
    ip_pack->FRAG_OFFSET = 0;
    ip_pack->TTL = 64;
    ip_pack->CSUM = 0;
    ip_pack->TPA = ip_pack->SPA;
    ip_pack->SPA = LOCAL_IP;
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
uint16_t IP_CSum(uint8_t * buf, size_t length){
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