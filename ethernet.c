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
static void Eth_SwitchControlRegisterBank(uint8_t address){
    uint8_t bank = (address >> 5) & (ECON1_BSEL1 | ECON1_BSEL0);
    if ((current_bank != bank) && ((address & OPCODE_CLEAR_MASK) < 0x1B)){
        uint8_t addr = (ECON1 & OPCODE_CLEAR_MASK) | ((bank & ECON1_BSEL0) ? (ETH_OPCODE_BFS) : (ETH_OPCODE_BFC));
        uint8_t data = ECON1_BSEL0;
        CS_SEL;
        WRITE_ETH_SPI_BYTE(&addr);
        WRITE_ETH_SPI_BYTE(&data);
        CS_DESEL;
        addr = (ECON1 & OPCODE_CLEAR_MASK) | ((bank & ECON1_BSEL1) ? (ETH_OPCODE_BFS) : (ETH_OPCODE_BFC));
        data = ECON1_BSEL1;
        CS_SEL;
        WRITE_ETH_SPI_BYTE(&addr);
        WRITE_ETH_SPI_BYTE(&data);
        CS_DESEL;
        current_bank = bank & (ECON1_BSEL0 | ECON1_BSEL1);
    }
}

/**
 * @brief reading control register
 * @param arg control register address
 * @return register value
 */
uint8_t Eth_ReadControlRegister(uint8_t addr){
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
uint16_t Eth_ReadControlRegister_16(uint8_t addr){
    return (Eth_ReadControlRegister(addr) | ((uint16_t)Eth_ReadControlRegister(addr + 1) << 8));
}

// TODO: function not tested
void Eth_ReadBufferMemory(uint8_t * data, uint16_t length){
    uint8_t addr = ETH_OPCODE_RBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    while(length--){
        READ_ETH_SPI_BYTE(data);
        data++;
    }
    CS_DESEL;
}

/**
 * @brief writing control register
 * @param arg control register address
 * @param data data to write to the register
 */
void Eth_WriteControlRegister(uint8_t addr, uint8_t data){
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
void Eth_WriteControlRegister_16(uint8_t addr, uint16_t data){
    Eth_WriteControlRegister(addr, (uint8_t)data);
    Eth_WriteControlRegister(addr + 1, (uint8_t)(data >> 8));
}

// TODO: function not tested
void Eth_WriteBufferMemory(uint8_t * data, uint16_t length){
    uint8_t addr = ETH_OPCODE_WBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    while(length--){
        WRITE_ETH_SPI_BYTE(data);
        data++;
    }
    CS_DESEL;
}

/**
 * @brief Bit field set function (addr |= data)
 * @param addr register address
 * @param data bits to set
 */
void Eth_BitFieldSet(uint8_t addr, uint8_t data){
    Eth_SwitchControlRegisterBank(addr);
    addr = ETH_OPCODE_BFS | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

/**
 * @brief Bit field reset function (addr &= ~data)
 * @param addr register address
 * @param data bits to reset
 */
void Eth_BitFieldClear(uint8_t addr, uint8_t data){
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
void Eth_SystemResetCommand(void){
    CS_DESEL;
    uint8_t addr = ETH_OPCODE_SRC | 0x1F;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    CS_DESEL;
    HAL_Delay(1);
    RESET_BANK;
    Eth_BitFieldClear(MICMD, MICMD_MIIRD); // Resetting the bit (dont know for what)
}

/**
 * @brief PHY register reading
 * @param addr PHY register address
 * @return PHY register data
 */
uint16_t PHY_ReadRegister(uint8_t addr){
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
void PHY_WriteRegister(uint8_t addr, uint16_t data){
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
        if (HAL_GetTick() - timeout_ticks > 5000) return ENC28J60_TIMEOUT;
    }
    print_db("Ethernet Reboot Successful\r\n");

    // Setting up the buffer FIFO size
    Eth_WriteControlRegister_16(ERXSTL, RXSTART);   // RX buffer:
    if (Eth_ReadControlRegister_16(ERXSTL) != RXSTART) return ENC28J60_ETHERR;
    Eth_WriteControlRegister_16(ERXNDL, RXEND);     // 0x0000 - (ERX_size - 1)
    if (Eth_ReadControlRegister_16(ERXNDL) != RXEND) return ENC28J60_ETHERR;
    Eth_WriteControlRegister_16(ETXSTL, TXSTART);   // TX buffer:
    if (Eth_ReadControlRegister_16(ETXSTL) != TXSTART) return ENC28J60_ETHERR;
    Eth_WriteControlRegister_16(ETXNDL, TXEND);     // (ERX_size) - (ERX_size + ETX_size - 1)
    if (Eth_ReadControlRegister_16(ETXNDL) != TXEND) return ENC28J60_ETHERR;
    print_db("Ethernet configuration OK\r\n");

    // Filters setting up
    // TODO: Add and checkout filters

    // MAC setting up
    Eth_WriteControlRegister(MACON1, (MACON1_MARXEN |
                                     MACON1_RXPAUS |
                                     MACON1_TXPAUS));
    if (Eth_ReadControlRegister(MACON1) != (MACON1_MARXEN |
                                            MACON1_RXPAUS |
                                            MACON1_TXPAUS)) return ENC28J60_MACERR;
    Eth_WriteControlRegister(MACON3, MACON3_PADCFG0 |
                                     MACON3_TXCRCEN |
                                     MACON3_FRMLNEN |
                                     MACON3_FULDPX);
    if (Eth_ReadControlRegister(MACON3) != (MACON3_PADCFG0 |
                                           MACON3_TXCRCEN |
                                           MACON3_FRMLNEN |
                                           MACON3_FULDPX)) return ENC28J60_MACERR;

    Eth_WriteControlRegister_16(MAMXFLL, MXFRAME); // Max frame size
    if (Eth_ReadControlRegister_16(MAMXFLL) != MXFRAME) return ENC28J60_MACERR;
    Eth_WriteControlRegister(MABBIPG, 0x15);    // Back-to-back delay
    if (Eth_ReadControlRegister(MABBIPG) != 0x15) return ENC28J60_MACERR;
    Eth_WriteControlRegister(MAIPGL, 0x12);     // Non-back-to-back delay
    if (Eth_ReadControlRegister(MAIPGL) != 0x12) return ENC28J60_MACERR;
    Eth_WriteControlRegister(MAIPGH, 0x0C);
    if (Eth_ReadControlRegister(MAIPGH) != 0x0C) return ENC28J60_MACERR;
    print_db("MAC configuration OK\r\n");

    // MAC address setting up
    uint8_t mac_ad[] = MAC_ADDR;
    Eth_WriteControlRegister(MAADR6, mac_ad[0]);
    Eth_WriteControlRegister(MAADR5, mac_ad[1]);
    Eth_WriteControlRegister(MAADR4, mac_ad[2]);
    Eth_WriteControlRegister(MAADR3, mac_ad[3]);
    Eth_WriteControlRegister(MAADR2, mac_ad[4]);
    Eth_WriteControlRegister(MAADR1, mac_ad[5]);
    uint8_t str[64];
    snprintf(str, 63, "MAC address: %2x:%2x:%2x:%2x:%2x:%2x\r\n", Eth_ReadControlRegister(MAADR6), 
                                                                  Eth_ReadControlRegister(MAADR5),
                                                                  Eth_ReadControlRegister(MAADR4),
                                                                  Eth_ReadControlRegister(MAADR3),
                                                                  Eth_ReadControlRegister(MAADR2),
                                                                  Eth_ReadControlRegister(MAADR1));
    print_db(str);

    // PHY setting up
    PHY_WriteRegister(PHCON1, PHCON1_PDPXMD);
    if (PHY_ReadRegister(PHCON1) != PHCON1_PDPXMD) return ENC28J60_PHYERR;
    PHY_WriteRegister(PHCON2, PHCON2_HDLDIS);
    if (PHY_ReadRegister(PHCON2) != PHCON2_HDLDIS) return ENC28J60_PHYERR;
    PHY_WriteRegister(PHLCON, PHLCON_LACFG2 |
                              PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 |
                              PHLCON_LFRQ0 |
                              PHLCON_STRCH);
    if (PHY_ReadRegister(PHLCON) != (PHLCON_LACFG2 |
                                    PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 |
                                    PHLCON_LFRQ0 |
                                    PHLCON_STRCH)) return ENC28J60_PHYERR;
    print_db("PHY configuration OK\r\n");

    // Frames reception enable
    Eth_BitFieldSet(ECON1, ECON1_RXEN);

    

    return ENC28J60_OK;
    // Frames reception enable
    // Eth_WriteControlRegister(ECON1, ECON1_RXEN);
}

/**
 * @brief Packet sending
 * @param data pointer to the array
 * @param length array length
 * @return uint8_t Packet transmit status
 */
uint8_t Eth_SendPacket(uint8_t * data, uint8_t length){
    // Waiting for the 
    for (int iter = 0; Eth_ReadControlRegister(ECON1) & ECON1_TXRTS; iter++){
        // Check for errors
        if (Eth_ReadControlRegister(EIR) & EIR_TXERIF){
            Eth_BitFieldSet(ECON1, ECON1_TXRST);
            Eth_BitFieldClear(ECON1, ECON1_TXRST);
        }
        HAL_Delay(5);
        // Timeout call
        if (iter > 5) return ENC28J60_TIMEOUT;
    }
    Eth_WriteControlRegister_16(EWRPTL, TXSTART);
    Eth_WriteBufferMemory("\x00", 1);
    Eth_WriteBufferMemory(data, length);

    Eth_WriteControlRegister_16(ETXSTL, TXSTART);
    Eth_WriteControlRegister_16(ETXNDL, TXSTART + length);

    Eth_BitFieldSet(ECON1, ECON1_TXRST);

    return ENC28J60_OK;
}

/**
 * @brief Data reception
 * @param data pointer to the buffer start
 * @param length buffer size
 * @return uint8_t amount of bytes read
 */
uint8_t Eth_ReceivePacket(uint8_t * data, uint16_t length){
    uint16_t len = 0, rxlen, status, temp;
    if (Eth_ReadControlRegister(EPKTCNT)){
        Eth_WriteControlRegister_16(ERDPTL, rxrdpt);
        Eth_ReadBufferMemory((void *)&rxrdpt, sizeof(rxrdpt));
        Eth_ReadBufferMemory((void *)&rxlen, sizeof(rxlen));
        Eth_ReadBufferMemory((void *)&status, sizeof(status)); // TODO: add vector filtering
        uint8_t str[64];
        snprintf(str, 63, "RXRDPT: %x RXLEN: %x STATUS: %x\r\n", rxrdpt, rxlen, status);
        print_db(str);
        if (status & 0x80){
            len = rxlen - 4;
            if (len > length) len = length;
            Eth_ReadBufferMemory(data, len);
        }

        temp = (rxrdpt - 1) & RXEND;
        Eth_WriteControlRegister_16(ERXRDPTL, temp);

        Eth_BitFieldSet(ECON2, ECON2_PKTDEC);
    }
    return len;
}
    
