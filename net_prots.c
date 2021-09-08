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

///////////////////////////////
// Ethernet frame processing //
///////////////////////////////
uint8_t Ethernet_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Setting up the orig and dest MACs
    memcpy(eth_pack->THA, eth_pack->SHA, 6);
    memcpy(eth_pack->SHA, mac_addr, 6);
    // Pushing the packet to the buffer
    Eth_SendPacket((void *)eth_pack, length + sizeof(ethernet_packet_t));
    // FIXME: error check
    return 0x00; // FIXME: error check
}

uint8_t Ethernet_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    print_db("Ethernet Processing\r\n");
    if (length < sizeof(ethernet_packet_t)) return 0x01; // FIXME: error check
    if (eth_pack->TYPE == ETH_ARP_TYPE){
        ARP_PacketProc(eth_pack, length - sizeof(ethernet_packet_t));
    }
    else if (eth_pack->TYPE == ETH_IP_TYPE){
        IP_PacketProc(eth_pack, length - sizeof(ethernet_packet_t));
    }
    else return 0x02; // FIXME: error check
    return 0x00; // FIXME: error check
}

//////////////////////////
// ARP frame processing //
//////////////////////////
uint8_t ARP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the ARP package
    ARP_packet_t * arp_pack = (void *)(eth_pack->Payload);

    // Checking the packet
    if (length < sizeof(ARP_packet_t)) return 0x01; // FIXME: error check
    if (arp_pack->HTYPE != ARP_HTYPE_ETHERNET) return 0x02; // FIXME: error check
    if (arp_pack->PTYPE != ARP_PTYPE_IPv4) return 0x03; // FIXME: error check
    if (arp_pack->OPER != ARP_OPER_REQUEST) return 0x04; // FIXME: error check
    if (arp_pack->TPA != ip_addr) return 0x05; // FIXME: error check

    // Operation type setting
    arp_pack->OPER = ARP_OPER_RESPONSE;
    
    // Target and own ip setting
    arp_pack->TPA = arp_pack->SPA;
    arp_pack->SPA = ip_addr;

    // Target and own MAC setting
    memcpy(arp_pack->THA, arp_pack->SHA, 6);
    memcpy(arp_pack->SHA, mac_addr, 6);

    Ethernet_Reply(eth_pack, sizeof(ARP_packet_t));

    return 0x00; // FIXME: error check
}

//////////////////////////
// IP packet processing //
//////////////////////////
uint8_t IP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    print_db("IP Provessing\r\n"); // TODO: remove
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    
    // Checking the packet
    if (ip_pack->VERSION != 0x4) return 0x01; // FIXME: error check
    if (ip_pack->IHL != 5) return 0x02; // FIXME: error check
    if (ip_pack->TPA != ip_addr) return 0x03; // FIXME: error check;
    
    // Calculating payload size
    length = inv_16bit(ip_pack->PACK_LEN) - sizeof(IP_packet_t);

    // Checking the above protocol
    if (ip_pack->PROTOCOL == IP_PROTOCOL_ICMP){
        ICMP_PacketProc(eth_pack, length);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_UDP){
        UDP_PacketProc(eth_pack, length);
    }
    else if (ip_pack->PROTOCOL == IP_PROTOCOL_TCP){
        return 0x04; // FIXME: add TCP
    }

    return 0x00; // FIXME: error check
}

uint8_t IP_Reply(ethernet_packet_t * eth_pack, uint16_t length){
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);

    // Setting up the IP packet
    ip_pack->PACK_LEN = inv_16bit(length + sizeof(IP_packet_t));
    ip_pack->FRAG_ID = 0;
    ip_pack->FRAG_OFFSET = 0;
    ip_pack->TTL = 64;
    ip_pack->CSUM = 0;
    ip_pack->TPA = ip_pack->SPA;
    ip_pack->SPA = ip_addr;
    ip_pack->CSUM = IP_CSum(0, (void *)ip_pack, sizeof(IP_packet_t));

    // Add to the ethernet packet
    Ethernet_Reply((void *)eth_pack, length + sizeof(IP_packet_t));

    return 0x00; // FIXME: check errors
}

// Control sum calculation for IP
uint16_t IP_CSum(uint32_t sum, uint8_t * buf, size_t length){
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
uint8_t ICMP_PacketProc(ethernet_packet_t * eth_pack, uint16_t length){
    print_db("ICMP Processing\r\n"); // TODO: remove
    // Getting the IP package
    IP_packet_t * ip_pack = (void *)(eth_pack->Payload);
    ICMP_packet_t * icmp_pack = (void *)(ip_pack->Payload);

    // Packet length check
    if (length < sizeof(ICMP_packet_t)) return 0x01; // FIXME: error check
    // Reply to the host
    if (icmp_pack->TYPE == ICMP_TYPE_ECHO_REQUEST){
        icmp_pack->TYPE = ICMP_TYPE_ECHO_REPLY;
        icmp_pack->CSUM += ICMP_TYPE_ECHO_REQUEST;
        IP_Reply(eth_pack, length);
    }
    else return 0x02; // FIXME: error check

    return 0x00; // FIXME: error check
}
