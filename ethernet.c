#include "ethernet.h"

#define RESET_BANK current_bank = 0x00

#define RXSIZE  0x1A00
#define TXSIZE  0x2000 - RXSIZE
#define MXFRAME 1500

#define RXSTART 0
#define RXEND   RXSIZE - 1
#define TXSTART RXSIZE
#define TXEND   RXSIZE + TXSIZE - 1

// typedef enum current_bank{
//     BANK_0 = 0x00,
//     BANK_1 = 0x01,
//     BANK_2 = 0x02,
//     BANK_3 = 0x03
// } bank_t;

uint8_t current_bank = 0x00;

uint16_t rxrdpt = 0;

/**
 * @brief control register bank switch
 * @param bank bank number
 */
static inline void Eth_SwitchControlRegisterBank(uint8_t address){
    uint8_t bank = (address >> 5) & (ECON1_BSEL1 | ECON1_BSEL0);
    if ((current_bank != bank) && ((address & OPCODE_CLEAR_MASK) < 0x1B)){
        uint8_t data[2] = { (ECON1 & OPCODE_CLEAR_MASK) | ((bank & ECON1_BSEL0) ? (ETH_OPCODE_BFS) : (ETH_OPCODE_BFC)),
                            (ECON1_BSEL0) };
        CS_SEL;
        WRITE_ETH_SPI_2BYTE(data);
        CS_DESEL;
        data[0] = (ECON1 & OPCODE_CLEAR_MASK) | ((bank & ECON1_BSEL1) ? (ETH_OPCODE_BFS) : (ETH_OPCODE_BFC));
        data[1] = (ECON1_BSEL1);
        CS_SEL;
        WRITE_ETH_SPI_2BYTE(data);
        CS_DESEL;
        current_bank = bank & (ECON1_BSEL0 | ECON1_BSEL1);
    }
}

/**
 * @brief reading control register
 * @param arg control register address
 * @return register value
 */
static inline uint8_t Eth_ReadControlRegister(uint8_t addr){
    Eth_SwitchControlRegisterBank(addr);
    uint8_t byte = ETH_OPCODE_RCR | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&byte);
    READ_ETH_SPI_BYTE(&byte);
    CS_DESEL;
    return byte;
}

/**
 * @brief read coupled register function
 * @param addr register address
 * @return data
 */
static inline uint16_t Eth_ReadControlRegister_16(uint8_t addr){
    return (Eth_ReadControlRegister(addr) | ((uint16_t)Eth_ReadControlRegister(addr + 1) << 8));
}

/**
 * @brief ENC28J60 buffer memory read
 * @param data where to read
 * @param length size
 */
static inline void Eth_ReadBufferMemory(uint8_t * data, uint16_t length){
    uint8_t addr = ETH_OPCODE_RBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    while(length--){
        READ_ETH_SPI_BYTE(data++);
    }
    CS_DESEL;
}

/**
 * @brief ENC28J60 buffer memory write
 * @param data what to write
 * @param length size
 */
static inline void Eth_WriteBufferMemory(uint8_t * data, uint16_t length){
    uint8_t addr = ETH_OPCODE_WBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    while(length--){
        WRITE_ETH_SPI_BYTE(data++);
    }
    CS_DESEL;
}

/**
 * @brief writing control register
 * @param arg control register address
 * @param data data to write to the register
 */
static inline void Eth_WriteControlRegister(uint8_t addr, uint8_t data){
    Eth_SwitchControlRegisterBank(addr);
    addr = ETH_OPCODE_WCR | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

/**
 * @brief writing coupled control register
 * @param addr control register address
 * @param data data to write
 */
static inline void Eth_WriteControlRegister_16(uint8_t addr, uint16_t data){
    Eth_SwitchControlRegisterBank(addr);
    uint8_t dat[2] = { (ETH_OPCODE_WCR | (addr & OPCODE_CLEAR_MASK)),
                       (uint8_t)(data) };
    CS_SEL;
    WRITE_ETH_SPI_2BYTE(dat);
    CS_DESEL;
    dat[0] = (ETH_OPCODE_WCR | ((addr + 1) & OPCODE_CLEAR_MASK));
    dat[1] = (uint8_t)(data >> 8);
    CS_SEL;
    WRITE_ETH_SPI_2BYTE(dat);
    CS_DESEL;
}

/**
 * @brief Bit field set function (addr |= data)
 * @param addr register address
 * @param data bits to set
 */
static inline void Eth_BitFieldSet(uint8_t addr, uint8_t data){
    uint8_t dat[2] = { (ETH_OPCODE_BFS | (addr & OPCODE_CLEAR_MASK)),
                       data };
    Eth_SwitchControlRegisterBank(addr);
    CS_SEL;
    WRITE_ETH_SPI_2BYTE(dat);
    CS_DESEL;
}

/**
 * @brief Bit field reset function (addr &= ~data)
 * @param addr register address
 * @param data bits to reset
 */
static inline void Eth_BitFieldClear(uint8_t addr, uint8_t data){
    Eth_SwitchControlRegisterBank(addr);
    addr = ETH_OPCODE_BFC | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

/**
 * @brief Soft reset implementation
 */
static inline void Eth_SystemResetCommand(void){
    CS_DESEL;
    uint8_t addr = ETH_OPCODE_SRC | 0x1F;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    CS_DESEL;
    DELAY(1);
    RESET_BANK;
    Eth_BitFieldClear(MICMD, MICMD_MIIRD); // Resetting the bit (dont know for what)
}

/**
 * @brief PHY register reading
 * @param addr PHY register address
 * @return PHY register data
 */
static inline uint16_t PHY_ReadRegister(uint8_t addr){
    Eth_WriteControlRegister(MIREGADR, addr);
    Eth_BitFieldSet(MICMD, MICMD_MIIRD);
    while(Eth_ReadControlRegister(MISTAT) & MISTAT_BUSY);
    Eth_BitFieldClear(MICMD, MICMD_MIIRD);
    return Eth_ReadControlRegister_16(MIRDL);
}

/**
 * @brief PHY register writing
 * @param addr PHY register address
 * @param data data to write
 */
static inline void PHY_WriteRegister(uint8_t addr, uint16_t data){
    Eth_WriteControlRegister(MIREGADR, addr);
    Eth_WriteControlRegister(MIWRL, (uint8_t)data);
    Eth_WriteControlRegister(MIWRH, (uint8_t)(data >> 8));
    while(Eth_ReadControlRegister(MISTAT) & MISTAT_BUSY);
}

uint8_t ENC28J60_Init(void){
    // Ethernet reboot
    Eth_SystemResetCommand();
    uint64_t timeout_ticks = HAL_GetTick();
    while(Eth_ReadControlRegister(ESTAT) & ESTAT_CLKRDY != ESTAT_CLKRDY){
#ifdef ETHERNET_DEBUG
        print_wr("ENC28J60 Reboot in progress\r\n");
#endif
        if (HAL_GetTick() - timeout_ticks > 5000){
#ifdef ETHERNET_DEBUG
            print_er("ENC28J60 Reboot timeout\r\n");
#endif
            return ENC28J60_TIMEOUT;
        }
    }

#ifdef ETHERNET_DEBUG
    print_in("ENC28J60 Reboot Successful\r\n");
#endif

    // Setting up the buffer FIFO size
    Eth_WriteControlRegister_16(ERXSTL, RXSTART);   // RX buffer:
    Eth_WriteControlRegister_16(ERXNDL, RXEND);     // 0x0000 - (ERX_size - 1)
    Eth_WriteControlRegister_16(ETXSTL, TXSTART);   // TX buffer:
    Eth_WriteControlRegister_16(ETXNDL, TXEND);     // (ERX_size) - (ERX_size + ETX_size - 1)
#ifdef ETHERNET_DEBUG    
    if (Eth_ReadControlRegister_16(ERXSTL) != RXSTART) print_wr("Ethernet register corrupted: ERXST\r\n");
    if (Eth_ReadControlRegister_16(ERXNDL) != RXEND) print_wr("Ethernet register corrupted: ERXND\r\n");
    if (Eth_ReadControlRegister_16(ETXSTL) != TXSTART) print_wr("Ethernet register corrupted: ETXST\r\n");
    if (Eth_ReadControlRegister_16(ETXNDL) != TXEND) print_wr("Ethernet register corrupted: ETXND\r\n");
#endif    

#ifdef ETHERNET_DEBUG
    print_in("Ethernet configured\r\n");
#endif

    // Filters setting up
    // TODO: Add and checkout filters

    // MAC setting up
    Eth_WriteControlRegister(MACON1, (MACON1_MARXEN |
                                     MACON1_RXPAUS |
                                     MACON1_TXPAUS |
                                     MACON1_PASSALL));
    Eth_WriteControlRegister(MACON3, MACON3_PADCFG0 |
                                     MACON3_TXCRCEN |
                                     MACON3_FRMLNEN |
                                     MACON3_FULDPX);
    Eth_WriteControlRegister_16(MAMXFLL, MXFRAME); // Max frame size
    Eth_WriteControlRegister(MABBIPG, 0x15);    // Back-to-back delay
    Eth_WriteControlRegister(MAIPGL, 0x12);     // Non-back-to-back delay
    Eth_WriteControlRegister(MAIPGH, 0x0C);
#ifdef ETHERNET_DEBUG
    if (Eth_ReadControlRegister(MACON1) != (MACON1_MARXEN |
                                            MACON1_RXPAUS |
                                            MACON1_TXPAUS |
                                            MACON1_PASSALL)) print_wr("MAC register corrupted: MACON1\r\n");
    if (Eth_ReadControlRegister(MACON3) != (MACON3_PADCFG0 |
                                           MACON3_TXCRCEN |
                                           MACON3_FRMLNEN |
                                           MACON3_FULDPX)) print_wr("MAC register corrupted: MACON3\r\n");
    if (Eth_ReadControlRegister_16(MAMXFLL) != MXFRAME) print_wr("MAC register corrupted: MAMXFL\r\n");
    if (Eth_ReadControlRegister(MABBIPG) != 0x15) print_wr("MAC register corrupted: MABBIPG\r\n");
    if (Eth_ReadControlRegister(MAIPGL) != 0x12) print_wr("MAC register corrupted: MAIPGL\r\n");
    if (Eth_ReadControlRegister(MAIPGH) != 0x0C) print_wr("MAC register corrupted: MAIPGH\r\n");
#endif

#ifdef ETHERNET_DEBUG
    print_in("MAC configured\r\n");
#endif

    // MAC address setting up
    uint8_t mac_ad[] = MAC_ADDR;
    Eth_WriteControlRegister(MAADR6, mac_ad[5]);
    Eth_WriteControlRegister(MAADR5, mac_ad[4]);
    Eth_WriteControlRegister(MAADR4, mac_ad[3]);
    Eth_WriteControlRegister(MAADR3, mac_ad[2]);
    Eth_WriteControlRegister(MAADR2, mac_ad[1]);
    Eth_WriteControlRegister(MAADR1, mac_ad[0]);

#ifdef ETHERNET_DEBUG
    uint8_t str[64];
    snprintf(str, 63, "MAC address: %x:%x:%x:%x:%x:%x\r\n", Eth_ReadControlRegister(MAADR6), 
                                                            Eth_ReadControlRegister(MAADR5),
                                                            Eth_ReadControlRegister(MAADR4),
                                                            Eth_ReadControlRegister(MAADR3),
                                                            Eth_ReadControlRegister(MAADR2),
                                                            Eth_ReadControlRegister(MAADR1));
    print_in(str);
#endif

    // PHY setting up
    PHY_WriteRegister(PHCON1, PHCON1_PDPXMD);
    PHY_WriteRegister(PHCON2, PHCON2_HDLDIS);
    PHY_WriteRegister(PHLCON, PHLCON_LACFG2 |
                              PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 |
                              PHLCON_LFRQ0 |
                              PHLCON_STRCH);

#ifdef ETHERNET_DEBUG
    if (PHY_ReadRegister(PHCON1) != PHCON1_PDPXMD) print_wr("PHY register corrupted: PHCON1\r\n");
    if (PHY_ReadRegister(PHCON2) != PHCON2_HDLDIS) print_wr("PHY register corrupted: PHCON2\r\n");
    if (PHY_ReadRegister(PHLCON) != (PHLCON_LACFG2 |
                                    PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 |
                                    PHLCON_LFRQ0 |
                                    PHLCON_STRCH)) print_wr("PHY register corrupted: PHLCON\r\n");
#endif

#ifdef ETHERNET_DEBUG
    print_in("PHY configured\r\n");
#endif

    // Frames reception enable
    Eth_BitFieldSet(ECON1, ECON1_RXEN);

    return ENC28J60_OK;
}

/**
 * @brief Packet sending
 * @param data pointer to the array
 * @param length array length
 * @return uint8_t Packet transmit status
 */
uint16_t Eth_SendPacket(uint8_t * data, uint8_t length){
    // Waiting for the transmitter to be ready
    for (int iter = 0; Eth_ReadControlRegister(ECON1) & ECON1_TXRTS; iter++){
        // Check for errors
        if (Eth_ReadControlRegister(EIR) & EIR_TXERIF){
            Eth_BitFieldSet(ECON1, ECON1_TXRST);
            Eth_BitFieldClear(ECON1, ECON1_TXRST);
        }
        DELAY(5);
        // Timeout call
        if (iter > 5){
#ifdef ETHERNET_DEBUG
            print_er("Ethernet transmitter timeout\r\n");
#endif
            return ENC28J60_TIMEOUT;
        }
    }
    Eth_WriteControlRegister_16(EWRPTL, TXSTART);
    Eth_WriteBufferMemory((uint8_t *)"\x00", 1);
    Eth_WriteBufferMemory(data, length);

    Eth_WriteControlRegister_16(ETXSTL, TXSTART);
    Eth_WriteControlRegister_16(ETXNDL, TXSTART + length);

    Eth_BitFieldSet(ECON1, ECON1_TXRTS);

    return ENC28J60_OK;
}

/**
 * @brief Data reception
 * @param data pointer to the buffer start
 * @param length buffer size
 * @return uint8_t amount of bytes read
 */
uint16_t Eth_ReceivePacket(uint8_t * data, uint16_t length){
    uint16_t len = 0, rxlen, status, temp;
    
    if (Eth_ReadControlRegister(EPKTCNT)){
        Eth_WriteControlRegister_16(ERDPTL, rxrdpt);
        Eth_ReadBufferMemory((void *)&rxrdpt, sizeof(rxrdpt));
        Eth_ReadBufferMemory((void *)&rxlen, sizeof(rxlen));
        Eth_ReadBufferMemory((void *)&status, sizeof(status));
        if (status & 0x80){
            len = rxlen - 4;
            if (len > length) len = length;
            Eth_ReadBufferMemory(data, len);
        }

        temp = ((rxrdpt - 1) & RXEND);
        Eth_WriteControlRegister_16(ERXRDPTL, temp);

        Eth_BitFieldSet(ECON2, ECON2_PKTDEC);
    }
    return len;
}
    
