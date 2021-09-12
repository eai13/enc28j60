#include "net_prots.h"

extern uint8_t network_buffer[];

/**
 * @brief ICMP packet processing
 * @param eth_pack 
 * @param length 
 */
void ICMP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    ICMP_packet_t * icmp_pack = (void *)(ip_pack->Payload);

    // Packet length check
    if (length < ICMP_PACKET_STRUCT_SIZE){
#ifdef ETHERNET_DEBUG
        print_er("ICMP frame error\r\n");
#endif
    }
    // Reply to the host
    if (icmp_pack->TYPE == ICMP_TYPE_ECHO_REQUEST){
        icmp_pack->TYPE = ICMP_TYPE_ECHO_REPLY;
        icmp_pack->CSUM += ICMP_TYPE_ECHO_REQUEST;
        IP_Reply(eth_pack, length);
    }
    else{
#ifdef ETHERNET_DEBUG
        print_wr("ICMP frame: wrong structure\r\n");
#endif
    }
}
