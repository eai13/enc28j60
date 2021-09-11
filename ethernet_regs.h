#ifndef ENC28J60_REGS_H
#define ENC28J60_REGS_H

// BANK IDs
#define BANK_0 (0x00)
#define BANK_1 (0x20)
#define BANK_2 (0x40)
#define BANK_3 (0x60)
#define COMMON (0xA0)

// MII ID
#define MII_ID (0x80)
// MAC ID
#define MAC_ID (0x80)

// OPCODE Macros
#define ETH_OPCODE_RCR      0b00000000
#define ETH_OPCODE_RBM      0b00100000
#define ETH_OPCODE_WCR      0b01000000
#define ETH_OPCODE_WBM      0b01100000
#define ETH_OPCODE_BFS      0b10000000
#define ETH_OPCODE_BFC      0b10100000
#define ETH_OPCODE_SRC      0b11100000
#define OPCODE_CLEAR_MASK   0b00011111

#define ETH_SPI_TRANSMIT_TIMEOUT    1000
#define ETH_SPI_RECEIVE_TIMEOUT     1000

// ENC28J60 PHY Registers
#define PHCON1      (0x00)  // PHY control register 1
    #define PHCON1_PRST     (uint16_t)(1 << 15) // PHY software reset
    #define PHCON1_PLOOPBK  (uint16_t)(1 << 14) // PHY loopback
    #define PHCON1_PPWRSV   (uint16_t)(1 << 11) // PHY power-down
    #define PHCON1_PDPXMD   (uint16_t)(1 << 8)  // PHY duplex mode
#define PHSTAT1     (0x01)  // Physical layer status register 1
    #define PHSTAT1_PFDPX   (uint16_t)(1 << 12) // PHY full-duplex capable 
    #define PHSTAT1_PHDPX   (uint16_t)(1 << 11) // PHY half-duplex capable
    #define PHSTAT1_LLSTAT  (uint16_t)(1 << 2)  // PHY latching link status
    #define PHSTAT1_JBSTAT  (uint16_t)(1 << 1)  // PHY latching jabber status
#define PHID1       (0x02)  // PHY identifier (0x0083)
#define PHID2       (0x03)  // PHY indentifier (000101) | PHY P/N (0x00) | PHY revision (0x00)
#define PHCON2      (0x10)  // PHY control register 2
    #define PHCON2_FRCLNK   (uint16_t)(1 << 14) // PHY force linkup bit
    #define PHCON2_TXDIS    (uint16_t)(1 << 13) // Twisted-pair transmitter disable
    #define PHCON2_JABBER   (uint16_t)(1 << 10) // Jabber correction disable
    #define PHCON2_HDLDIS   (uint16_t)(1 << 8)  // PHY half-duplex loopback disable bit
#define PHSTAT2     (0x11)  // Physical layer status register 2
    #define PHSTAT2_TXSTAT  (uint16_t)(1 << 13) // PHY transmit status
    #define PHSTAT2_RXSTAT  (uint16_t)(1 << 12) // PHY receive status
    #define PHSTAT2_COLSTAT (uint16_t)(1 << 11) // PHY collision status
    #define PHSTAT2_LSTAT   (uint16_t)(1 << 10) // PHY link status (non-latching)
    #define PHSTAT2_DPXSTAT (uint16_t)(1 << 9)  // PHY duplex status
    #define PHSTAT2_PLRITY  (uint16_t)(1 << 5)  // Polarity status
#define PHIE        (0x12)  // PHY interrupt enable register
    #define PHIE_PLNKIE     (uint16_t)(1 << 4)  // PHY link change interrupt enable
    #define PHIE_PGEIE      (uint16_t)(1 << 1)  // PHY global interrupt enable
#define PHIR        (0x13)  // PHY interrupt request register
    #define PHIR_PLNKIF     (uint16_t)(1 << 4)  // PHY link change interrupt flag
    #define PHIR_PGIF       (uint16_t)(1 << 2)  // PHY global interrupt flag
#define PHLCON      (0x14)  // PHY module LED control register
    #define PHLCON_LACFG3   (uint16_t)(1 << 11) // LED A configuration bits
    #define PHLCON_LACFG2   (uint16_t)(1 << 10) // ...
    #define PHLCON_LACFG1   (uint16_t)(1 << 9)  // ...
    #define PHLCON_LACFG0   (uint16_t)(1 << 8)  // ...
    #define PHLCON_LBCFG3   (uint16_t)(1 << 7)  // LED B configuration bits
    #define PHLCON_LBCFG2   (uint16_t)(1 << 6)  // ...
    #define PHLCON_LBCFG1   (uint16_t)(1 << 5)  // ...
    #define PHLCON_LBCFG0   (uint16_t)(1 << 4)  // ...
    #define PHLCON_LFRQ1    (uint16_t)(1 << 3)  // LED pulse stretch time configuration bits
    #define PHLCON_LFRQ0    (uint16_t)(1 << 2)  // ...
    #define PHLCON_STRCH    (uint16_t)(1 << 1)  // LED pulse stretching enable

// ENC28J60 CONTROL REGISTERS
// BANK 0 REGISTERS
#define ERDPTL      (0x00 | BANK_0)  // Read pointer
#define ERDPTH      (0x01 | BANK_0)  // ...    
#define EWRPTL      (0x02 | BANK_0)  // Write pointer
#define EWRPTH      (0x03 | BANK_0)  // ...
#define ETXSTL      (0x04 | BANK_0)  // TX Start
#define ETXSTH      (0x05 | BANK_0)  // ...
#define ETXNDL      (0x06 | BANK_0)  // TX End
#define ETXNDH      (0x07 | BANK_0)  // ...
#define ERXSTL      (0x08 | BANK_0)  // RX Start
#define ERXSTH      (0x09 | BANK_0)  // ...
#define ERXNDL      (0x0A | BANK_0)  // RX End
#define ERXNDH      (0x0B | BANK_0)  // ...
#define ERXRDPTL    (0x0C | BANK_0)  // RX Read pointer
#define ERXRDPTH    (0x0D | BANK_0)  // ...
#define ERXWRPTL    (0x0E | BANK_0)  // RX Write pointer
#define ERXWRPTH    (0x0F | BANK_0)  // ...
#define EDMASTL     (0x10 | BANK_0)  // DMA Start
#define EDMASTH     (0x11 | BANK_0)  // ...
#define EDMANDL     (0x12 | BANK_0)  // DMA End
#define EDMANDH     (0x13 | BANK_0)  // ...
#define EDMADSTL    (0x14 | BANK_0)  // DMA Destination
#define EDMADSTH    (0x15 | BANK_0)  // ...
#define EDMACSL     (0x16 | BANK_0)  // DMA Checksum
#define EDMACSH     (0x17 | BANK_0)  // ...

// BANK 1 REGISTERS
#define EHT0        (0x00 | BANK_1)  // Hash Table
#define EHT1        (0x01 | BANK_1)  // ...
#define EHT2        (0x02 | BANK_1)  // ...
#define EHT3        (0x03 | BANK_1)  // ...
#define EHT4        (0x04 | BANK_1)  // ...
#define EHT5        (0x05 | BANK_1)  // ...
#define EHT6        (0x06 | BANK_1)  // ...
#define EHT7        (0x07 | BANK_1)  // ...
#define EPMM0       (0x08 | BANK_1)  // Pattern match mask
#define EPMM1       (0x09 | BANK_1)  // ...
#define EPMM2       (0x0A | BANK_1)  // ...
#define EPMM3       (0x0B | BANK_1)  // ...
#define EPMM4       (0x0C | BANK_1)  // ...
#define EPMM5       (0x0D | BANK_1)  // ...
#define EPMM6       (0x0E | BANK_1)  // ...
#define EPMM7       (0x0F | BANK_1)  // ...
#define EPMCSL      (0x10 | BANK_1)  // Pattern match checksum
#define EPMCSH      (0x11 | BANK_1)  // ...
#define EPMOL       (0x14 | BANK_1)  // Pattern match offset
#define EPMOH       (0x15 | BANK_1)  // ...
#define ERXFCON     (0x18 | BANK_1)  // Receive filter control register
    #define ERXFCON_UCEN    (1 << 7) // Unicast filter enable bit
    #define ERXFCON_ANDOR   (1 << 6) // AND/OR filter select bit
    #define ERXFCON_CRCEN   (1 << 5) // Post-filter CRC check enable
    #define ERXFCON_PMEN    (1 << 4) // PATTERN MATCH filter enable bit
    #define ERXFCON_MPEN    (1 << 3) // MAGIC PACKET filter enable
    #define ERXFCON_HTEN    (1 << 2) // HASH TABLE filter enable
    #define ERXFCON_MCEN    (1 << 1) // MULTICAST filter enable
    #define ERXFCON_BCEN    (1 << 0) // BROADCAST filter enable
#define EPKTCNT     (0x19 | BANK_1)  // Packet count

// BANK 2 REGISTERS
#define MACON1      (0x00 | MAC_ID | BANK_2)  // MAC control register 1
    #define MACON1_TXPAUS   (1 << 3) // Pause control frame transmission enable
    #define MACON1_RXPAUS   (1 << 2) // Pause control frame reception enable
    #define MACON1_PASSALL  (1 << 1) // Pass all received frames enable
    #define MACON1_MARXEN   (1 << 0) // MAC receive enable
#define MACON3      (0x02 | MAC_ID | BANK_2)  // MAC control register 3
    #define MACON3_PADCFG2  (1 << 7) // Automatic pad and CRC configuration bits
    #define MACON3_PADCFG1  (1 << 6) // ...
    #define MACON3_PADCFG0  (1 << 5) // ...
    #define MACON3_TXCRCEN  (1 << 4) // Transmit CRC enable
    #define MACON3_PHDREN   (1 << 3) // Proprietary header enable
    #define MACON3_HFRMEN   (1 << 2) // Huge frame enable
    #define MACON3_FRMLNEN  (1 << 1) // Frame length checking enable
    #define MACON3_FULDPX   (1 << 0) // MAC full-duplex enable
#define MACON4      (0x03 | MAC_ID | BANK_2)  // MAC control register 4
    #define MACON4_DEFER    (1 << 6) // Defer transmission enable
    #define MACON4_BPEN     (1 << 5) // No backoff during backpressure enable
    #define MACON4_NOBKOFF  (1 << 4) // No backoff enable
#define MABBIPG     (0x04 | MAC_ID | BANK_2)  // Back-to-back iter-packet gap delay (0x12 recommended)
#define MAIPGL      (0x06 | MAC_ID | BANK_2)  // Non-back-to-back inter-packet gap
#define MAIPGH      (0x07 | MAC_ID | BANK_2)  // ... (0x0C12 recommended)
#define MACLCON1    (0x08 | MAC_ID | BANK_2)  // Retransmission maximum
#define MACLCON2    (0x09 | MAC_ID | BANK_2)  // Collision window
#define MAMXFLL     (0x0A | MAC_ID | BANK_2)  // Maximum frame length
#define MAMXFLH     (0x0B | MAC_ID | BANK_2)  // ...
#define MICMD       (0x12 | MII_ID | BANK_2)  // MII command register
    #define MICMD_MIISCAN   (1 << 1) // MII scan enable bit
    #define MICMD_MIIRD     (1 << 0) // MII read enable
#define MIREGADR    (0x14 | MII_ID | BANK_2)  // MII register address
#define MIWRL       (0x16 | MII_ID | BANK_2)  // MII write data
#define MIWRH       (0x17 | MII_ID | BANK_2)  // ...
#define MIRDL       (0x18 | MII_ID | BANK_2)  // MII read data
#define MIRDH       (0x19 | MII_ID | BANK_2)  // ...

// BANK 3 REGISTERS
#define MAADR5      (0x00 | BANK_3)  // MAC address
#define MAADR6      (0x01 | BANK_3)  // ...
#define MAADR3      (0x02 | BANK_3)  // ...
#define MAADR4      (0x03 | BANK_3)  // ...
#define MAADR1      (0x04 | BANK_3)  // ...
#define MAADR2      (0x05 | BANK_3)  // ...
#define EBSTSD      (0x06 | BANK_3)  // Built-in self-test fill seed
#define EBSTCON     (0x07 | BANK_3)  // Ethernet self-test control register
    #define EBSTCON_PSV2    (1 << 7) // Pattern shift value bits
    #define EBSTCON_PSV1    (1 << 6) // ...
    #define EBSTCON_PSV0    (1 << 5) // ...
    #define EBSTCON_PSEL    (1 << 4) // Port select
    #define EBSTCON_TMSEL1  (1 << 3) // Test mode select bits
    #define EBSTCON_TMSEL0  (1 << 2) // ...
    #define EBSTCON_TME     (1 << 1) // Test mode enable
    #define EBSTCON_BISTST  (1 << 0) // Built-in self-test start/busy
#define EBSTCSL     (0x08 | BANK_3)  // Built-in self-test checksum
#define EBSTCSH     (0x09 | BANK_3)  // ...
#define MISTAT      (0x0A | MII_ID | BANK_3)  // MII status register
    #define MISTAT_NVALID   (1 << 2) // MII management read data not valid
    #define MISTAT_SCAN     (1 << 1) // MII management scan operation
    #define MISTAT_BUSY     (1 << 0) // MII management busy
#define EREVID      (0x12 | BANK_3)  // Ethernet revision ID
#define ECOCON      (0x15 | BANK_3)  // Clock output control register
    #define ECOCON_COCON2   (1 << 2) // Clock output configuration bits
    #define ECOCON_COCON1   (1 << 1) // ...
    #define ECOCON_COCON0   (1 << 0) // ...
#define EFLOCON     (0x17 | BANK_3)  // Ethernet flow control register
    #define EFLOCON_FULDPXS (1 << 2) // Read-only MAC full-duplex shadow
    #define EFLOCON_FCEN1   (1 << 1) // Flow-control enable bits
    #define EFLOCON_FCEN0   (1 << 0) // ...
#define EPAUSL      (0x18 | BANK_3)  // Pause timer value
#define EPAUSH      (0x19 | BANK_3)  // ...

// COMMON REGISTERS
#define EIE         (0x1B)  // Ethernet interrupt enable register  
    #define EIE_INTIE       (1 << 7) // Global INT interrupt enable bit
    #define EIE_PKTIE       (1 << 6) // Receive packet pending interrupt enable
    #define EIE_DMAIE       (1 << 5) // DMA interrupt enable bit
    #define EIE_LINKIE      (1 << 4) // Link status change interrupt enable bit
    #define EIE_TXIE        (1 << 3) // Transmit enable bit
    #define EIE_TXERIE      (1 << 1) // Transmit error interrupt enable bit
    #define EIE_RXERIE      (1 << 0) // Receive error interrupt enable bit
#define EIR         (0x1C)  // Ethernet interrupt request register
    #define EIR_PKTIF       (1 << 6) // Receive packet pending interrupt flag
    #define EIR_DMAIF       (1 << 5) // DMA interrupt flag bit
    #define EIR_LINKIF      (1 << 4) // Link change interrupt flag bit
    #define EIR_TXIF        (1 << 3) // Transmit interrupt flag bit
    #define EIR_TXERIF      (1 << 1) // Transmit error interrupt flag bit
    #define EIR_RXERIF      (1 << 0) // Receive error interrupt flag bit
#define ESTAT       (0x1D)  // Ethernet status register
    #define ESTAT_INT       (1 << 7) // INT interrupt flag bit
    #define ESTAT_BUFER     (1 << 6) // Ethernet buffer error status bit
    #define ESTAT_LATECOL   (1 << 4) // Late collision error bit
    #define ESTAT_RXBUSY    (1 << 2) // Receive busy bit
    #define ESTAT_TXABRT    (1 << 1) // Transmit abort error bit
    #define ESTAT_CLKRDY    (1 << 0) // Clock ready bit
#define ECON2       (0x1E)  // Ethernet control register 2
    #define ECON2_AUTOINC   (1 << 7) // Automatic buffer pointer increment enable bit
    #define ECON2_PKTDEC    (1 << 6) // Packet decrement bit
    #define ECON2_PWRSV     (1 << 5) // Power save enable bit
    #define ECON2_VRPS      (1 << 3) // Voltage regulator power save enable bit
#define ECON1       (0x1F)  // Ethernet control register 1
    #define ECON1_TXRST     (1 << 7) // Transmit logic reset bit
    #define ECON1_RXRST     (1 << 6) // Receive logic reset bit
    #define ECON1_DMAST     (1 << 5) // DMA start and busy status bit
    #define ECON1_CSUMEN    (1 << 4) // DMA checksum enable bit
    #define ECON1_TXRTS     (1 << 3) // Transmit request to send bit
    #define ECON1_RXEN      (1 << 2) // Receive enable bit
    #define ECON1_BSEL1     (1 << 1) // Bank select 1
    #define ECON1_BSEL0     (1 << 0) // Bank select 0

#endif