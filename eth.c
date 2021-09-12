#include "net_prots.h"

static uint8_t mac_addr[6] = MAC_ADDR;

extern uint8_t network_buffer[];

void Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Setting up the orig and dest MACs
    memcpy(eth_pack->THA, eth_pack->SHA, 6);
    memcpy(eth_pack->SHA, mac_addr, 6);
    // Pushing the packet to the buffer
    Eth_SendPacket((void *)eth_pack, length + ETH_PACKET_STRUCT_SIZE);
}

void Ethernet_PacketProc(ethernet_packet_t * eth_pack, uint16_t length, uint8_t * msg, uint16_t msg_size){
    if (length < ETH_PACKET_STRUCT_SIZE){
#ifdef ETHERNET_DEBUG
        print_er("Ethernet frame error\r\n");
#endif
    }
    if (eth_pack->TYPE == ETH_ARP_TYPE){
        ARP_PacketProc(eth_pack, length - ETH_PACKET_STRUCT_SIZE);
    }
    else if (eth_pack->TYPE == ETH_IP_TYPE){
        IP_PacketProc(eth_pack, length - ETH_PACKET_STRUCT_SIZE, msg, msg_size);
    }
    else return FRAME_ERROR;
}