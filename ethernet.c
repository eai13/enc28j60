#include "ethernet.h"

#define RESET_BANK current_bank = 0x00
#define MAC_ADDR {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}

static uint8_t current_bank = 0x00;

/**
 * @brief control register bank switch
 * @param bank bank number
 */
static void Eth_SwitchControlRegisterBank(uint8_t bank){
    Eth_WriteControlRegister(ECON1, (ECON1_BSEL0 | ECON1_BSEL1) & bank);
    current_bank = bank & (ECON1_BSEL0 | ECON1_BSEL1);
    int8_t str[32];
    snprintf(str, 31, "ENC28J60 Bank Switch %d\r\n", current_bank); // TODO: delete after debug
    print_db(str);
}

/**
 * @brief reading control register
 * @param arg control register address
 * @return register value
 */
uint8_t Eth_ReadControlRegister(uint8_t addr){
    uint8_t bank = ((addr >> 5) & (ECON1_BSEL0 | ECON1_BSEL1));
    if (current_bank != bank){
        Eth_SwitchControlRegisterBank(bank);
    }
    uint8_t byte = ETH_OPCODE_RCR | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&byte);
    if (ISMII(addr) || ISMAC(addr)){
        READ_ETH_SPI_BYTE(&byte);
    }
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
    return (Eth_ReadControlRegister(addr) | (Eth_ReadControlRegister(addr + 1) << 8));
}

// TODO: function not tested
uint8_t Eth_ReadBufferMemory(void){
    uint8_t addr = ETH_OPCODE_RBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    READ_ETH_SPI_BYTE(&addr);
    CS_DESEL;
    return addr;
}

/**
 * @brief writing control register
 * @param arg control register address
 * @param data data to write to the register
 */
void Eth_WriteControlRegister(uint8_t addr, uint8_t data){
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
void Eth_WriteBufferMemory(uint8_t data){
    uint8_t addr = ETH_OPCODE_WBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

/**
 * @brief Bit field set function (addr |= data)
 * @param addr register address
 * @param data bits to set
 */
void Eth_BitFieldSet(uint8_t addr, uint8_t data){
    uint8_t bank = ((addr >> 5) & (ECON1_BSEL0 | ECON1_BSEL1));
    if (current_bank != bank){
        Eth_SwitchControlRegisterBank(bank);
    }
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
    uint8_t bank = ((addr >> 5) & (ECON1_BSEL0 | ECON1_BSEL1));
    if (current_bank != bank){
        Eth_SwitchControlRegisterBank(bank);
    }
    addr = ETH_OPCODE_BFC | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

// TODO: function not tested
void Eth_SystemResetCommand(void){
    uint8_t addr = ETH_OPCODE_SRC | 0x1F;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    CS_DESEL;
    RESET_BANK;
    print_db("ENC28J60 Controller Reset\r\n");
}

/**
 * @brief PHY register reading
 * @param addr PHY register address
 * @return PHY register data
 */ // TODO: implement PHY reading
uint16_t PHY_ReadRegister(uint8_t addr){
    Eth_WriteControlRegister(MIREGADR, addr);
    return addr;
}

/**
 * @brief PHY register writing
 * @param addr PHY register address
 * @param data data to write
 */ // TODO: implement PHY writing
void PHY_WriteRegister(uint8_t addr, uint16_t data){

}



uint8_t ENC28J60_Init(void){
    // Setting up the buffer FIFO size
    uint16_t ERX_size = 0x1000; // RX buffer size
    uint16_t ETX_size = 0x1000; // TX buffer size
    Eth_WriteControlRegister_16(ERXSTL, 0x0000);                    // RX buffer:
    Eth_WriteControlRegister_16(ERXNDL, ERX_size - 1);              // 0x0000 - (ERX_size - 1)
    Eth_WriteControlRegister_16(ETXSTL, ERX_size);                  // TX buffer:
    Eth_WriteControlRegister_16(ETXNDL, ERX_size + ETX_size - 1);   // (ERX_size) - (ERX_size + ETX_size - 1)

    // Filters setting up
    // TODO: Add and checkout filters

    // MAC setting up
    Eth_WriteControlRegister(MACON1, MACON1_MARXEN |
                                     MACON1_RXPAUS |
                                     MACON1_TXPAUS);
    Eth_WriteControlRegister(MACON3, MACON3_PADCFG0 |
                                     MACON3_TXCRCEN |
                                     MACON3_FRMLNEN |
                                     MACON3_FULDPX);
    uint16_t max_frame_sz = 600;
    Eth_WriteControlRegister_16(MAMXFLL, max_frame_sz); // Max frame size
    Eth_WriteControlRegister(MABBIPG, 0x15);    // Back-to-back delay
    Eth_WriteControlRegister(MAIPGL, 0x12);     // Non-back-to-back delay
    Eth_WriteControlRegister(MAIPGH, 0x0C);
    uint8_t mac_ad[] = MAC_ADDR;
    Eth_WriteControlRegister(MAADR6, mac_ad[0]); // MAC address setting
    Eth_WriteControlRegister(MAADR5, mac_ad[1]);
    Eth_WriteControlRegister(MAADR4, mac_ad[2]);
    Eth_WriteControlRegister(MAADR3, mac_ad[3]);
    Eth_WriteControlRegister(MAADR2, mac_ad[4]);
    Eth_WriteControlRegister(MAADR1, mac_ad[5]);

    // PHY setting up
    // TODO: PHY setting up
}

