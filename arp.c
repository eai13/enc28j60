#include "net_prots.h"

static uint8_t mac_addr[] = MAC_ADDR;
extern uint8_t network_buffer[];

#define ARP_CACHE_POINTER_INCREMENT \
        (ARP_CACHE_SIZE - arp_cache_pointer == 1) ? (arp_cache_pointer = 0) : (arp_cache_pointer++)

static uint8_t arp_cache_pointer = 0x00;

uint8_t * ARP_MACSearch(uint32_t source_ip){
    for (uint8_t iter = 0; iter < ARP_CACHE_SIZE; iter++){
        if (ARP_cache[iter].IP == source_ip){
            return ARP_cache[iter].MAC;
        }
    }
    return NULL;
}

inline uint8_t ARP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the ARP package
    ARP_packet_t * arp_pack = (void *)(eth_pack->Payload);
    // Checking the packet
    if ((length < ARP_PACKET_STRUCT_SIZE) |
        (arp_pack->HTYPE != ARP_HTYPE_ETHERNET) |
        (arp_pack->PTYPE != ARP_PTYPE_IPv4) |
        (arp_pack->TPA != LOCAL_IP)){
        print_er("ARP frame error\r\n");
        return FRAME_ERROR;
    }
    // For sending ARP response
    if (arp_pack->OPER == ARP_OPER_REQUEST){
        arp_pack->OPER = ARP_OPER_RESPONSE;
        arp_pack->TPA = arp_pack->SPA;
        arp_pack->SPA = LOCAL_IP;
        memcpy(arp_pack->THA, arp_pack->SHA, 6);
        memcpy(arp_pack->SHA, mac_addr, 6);
        return Ethernet_Reply(eth_pack, ARP_PACKET_STRUCT_SIZE);
    }
    // For sending ARP request
    else if (arp_pack->OPER == ARP_OPER_RESPONSE){
        if (ARP_MACSearch(arp_pack->SPA) == 0){
            ARP_cache[arp_cache_pointer].IP = arp_pack->SPA;
            memcpy(ARP_cache[arp_cache_pointer].MAC, arp_pack->SHA, 6);
            ARP_CACHE_POINTER_INCREMENT;
            print_in("NEW MAC in collection\r\n");
        }
    }
    else{
        print_er("Unknown ARP operation type\r\n");
        return FRAME_ERROR;
    }
}

uint8_t * ARP_MACResolver(uint32_t source_ip){
    ethernet_packet_t * eth_pack = (void *)network_buffer;
    ARP_packet_t * arp_pack = (void *)(eth_pack->Payload);
    uint8_t * search_mac;

    if (search_mac = ARP_MACSearch(source_ip)){
        return search_mac;
    }
    else{
        memset(eth_pack->THA, 0xff, 6);
        memcpy(eth_pack->SHA, mac_addr, 6);
        eth_pack->TYPE = ETH_ARP_TYPE;

        arp_pack->HTYPE = ARP_HTYPE_ETHERNET;
        arp_pack->PTYPE = ARP_PTYPE_IPv4;
        arp_pack->OPER = ARP_OPER_REQUEST;
        arp_pack->PLEN = ARP_PLEN_IP;
        arp_pack->HLEN = ARP_HLEN_MAC;
        memcpy(arp_pack->SHA, mac_addr, 6);
        memset(arp_pack->THA, 0x00, 6);
        arp_pack->TPA = source_ip;
        arp_pack->SPA = LOCAL_IP;

        Eth_SendPacket(eth_pack, ARP_PACKET_STRUCT_SIZE + ETH_PACKET_STRUCT_SIZE);
        return 0;
    }
}