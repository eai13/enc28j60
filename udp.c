#include "net_prots.h"

static uint8_t mac_addr[6] = MAC_ADDR;

extern uint8_t network_buffer[];

uint8_t UDP_PacketProc(IP_packet_t * ip_pack, uint16_t length){
    UDP_packet_t * udp_pack = (void *)ip_pack->Payload;
    uint8_t str[64];
    snprintf(str, 63, "%x, %x, %x, %x\r\n", udp_pack->SP, udp_pack->TP, udp_pack->LENGTH, udp_pack->CSUM);
    print_in(str);
    print_in(udp_pack->Payload);
    print_in("\r\n");
}

uint8_t UDP_PacketSend(uint32_t targ_ip, uint16_t targ_port, uint8_t * data, uint16_t length){
    ethernet_packet_t * eth_pack;
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    UDP_packet_t * udp_pack = (void *)(ip_pack->Payload);
    
    // // UDP preparing
    // udp_pack->SP = 5555;
    // udp_pack->TP = inv_16bit(targ_port);
    // udp_pack->LENGTH = inv_16bit(length + UDP_PACKET_STRUCT_SIZE);
    // udp_pack->CSUM = 0;
    // memcpy(udp_pack->Payload, data, length);

    // // IP preparing
    // ip_pack->VERSION =      4;
    // ip_pack->IHL =          5;
    // ip_pack->ECN =          0x00;
    // ip_pack->DSCP =         0x00;
    // ip_pack->PACK_LEN =     inv_16bit(length + UDP_PACKET_STRUCT_SIZE + IP_PACKET_STRUCT_SIZE);
    // ip_pack->FRAG_ID =      0x12;
    // ip_pack->FLAGS =        0b010;
    // ip_pack->FRAG_OFFSET =  0x00;
    // ip_pack->TTL =          0x40;
    // ip_pack->PROTOCOL =     IP_PROTOCOL_UDP;
    // ip_pack->CSUM =         IP_CSum((void *)ip_pack, IP_PACKET_STRUCT_SIZE);
    // ip_pack->TPA =          targ_ip;
    // ip_pack->SPA =          ip_addr;

    // Ethernet praparing
    eth_pack->TYPE =        ETH_IP_TYPE;
    // memcpy(eth_pack->THA, broad_mac, 6);
    memcpy(eth_pack->SHA, mac_addr, 6);
    
    return Eth_SendPacket(eth_pack, /*length + UDP_PACKET_STRUCT_SIZE + IP_PACKET_STRUCT_SIZE + */ETH_PACKET_STRUCT_SIZE);
}
