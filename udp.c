#include "net_prots.h"

static uint8_t mac_addr[6] = MAC_ADDR;

extern uint8_t network_buffer[];

void UDP_PacketProc(IP_packet_t * ip_pack, uint16_t length, uint8_t * msg, uint16_t msg_size){
    UDP_packet_t * udp_pack = (void *)ip_pack->Payload;
    if (length < UDP_PACKET_STRUCT_SIZE){
#ifdef ETHERNET_DEBUG
        print_er("UDP frame: wrong structure\r\n");
        return;
#endif
    }
    if (udp_pack->LENGTH < msg_size) msg_size = udp_pack->LENGTH;
    memcpy(msg, (void *)(udp_pack->Payload), msg_size);
}

void UDP_PacketSend(uint32_t targ_ip, uint16_t targ_port, uint8_t * msg, uint16_t msg_size){
    ethernet_packet_t * eth_pack = (void *)(network_buffer);
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    UDP_packet_t * udp_pack = (void *)(ip_pack->Payload);
    
    uint8_t * search_mac;
    uint16_t len;
    if ((search_mac = ARP_MACResolver(targ_ip)) == 0){
        while((len = Eth_ReceivePacket(network_buffer, sizeof(network_buffer))) != 0){
            Ethernet_PacketProc((void *)(network_buffer), len, NULL, 0);
        }
    }

    // Ethernet preparing
    memcpy(eth_pack->SHA, mac_addr, 6);
    memcpy(eth_pack->THA, search_mac, 6);
    eth_pack->TYPE = ETH_IP_TYPE;

    // IP preparing
    ip_pack->IHL =          IP_IHL_DEFAULT;
    ip_pack->VERSION =      IP_VERSION_DEFAULT;
    ip_pack->ECN =          IP_ECN_DEFAULT;
    ip_pack->DSCP =         IP_DSCP_DEFAULT;
    ip_pack->PACK_LEN =     inv_16bit(msg_size + UDP_PACKET_STRUCT_SIZE + IP_PACKET_STRUCT_SIZE);
    ip_pack->FRAG_ID =      0x1234;
    ip_pack->FRAG_OFFST1 =  IP_FRAG_OFFST1_DEFAULT;
    ip_pack->FRAG_OFFST2 =  IP_FRAG_OFFST2_DEFAULT;
    ip_pack->FLAGS =        IP_FLAGS_NO_FRAGMENT;
    ip_pack->TTL =          IP_TTL_64;
    ip_pack->PROTOCOL =     IP_PROTOCOL_UDP;
    ip_pack->CSUM =         IP_CSum(0, (void *)(ip_pack), IP_PACKET_STRUCT_SIZE);
    ip_pack->SPA =          LOCAL_IP;
    ip_pack->TPA =          targ_ip;

    // UDP preparing
    udp_pack->SP =          inv_16bit(5555);
    udp_pack->TP =          inv_16bit(targ_port);
    udp_pack->LENGTH =      inv_16bit(msg_size + UDP_PACKET_STRUCT_SIZE);
    udp_pack->CSUM =        0x0000;

    memcpy(udp_pack->Payload, msg, msg_size);
    
    Eth_SendPacket(eth_pack, msg_size + UDP_PACKET_STRUCT_SIZE + IP_PACKET_STRUCT_SIZE + ETH_PACKET_STRUCT_SIZE);
}
