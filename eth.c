#include "net_prots.h"

static uint8_t mac_addr[6] = MAC_ADDR;

extern uint8_t network_buffer[];

uint8_t Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length){
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