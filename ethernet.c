#include "ethernet.h"

#define RESET_BANK current_bank = 0x00
#define MAC_ADDR {0x12, 0x12, 0x12, 0x12, 0x12, 0x12}
static uint8_t current_bank = 0x00;

static void Eth_SwitchControlRegisterBank(uint8_t bank){
    Eth_WriteControlRegister(ECON1, (ECON1_BSEL0 | ECON1_BSEL1) & bank);
    current_bank = bank & (ECON1_BSEL0 | ECON1_BSEL1);
    int8_t str[32];
    snprintf(str, 31, "ENC28J60 Bank Switch %d\r\n", current_bank);
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

// TODO: function not tested
void Eth_WriteBufferMemory(uint8_t data){
    uint8_t addr = ETH_OPCODE_WBM | 0b00011010;
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

// TODO: function not tested
void Eth_BitFieldSet(uint8_t addr, uint8_t data){
    addr = ETH_OPCODE_BFS | (addr & OPCODE_CLEAR_MASK);
    CS_SEL;
    WRITE_ETH_SPI_BYTE(&addr);
    WRITE_ETH_SPI_BYTE(&data);
    CS_DESEL;
}

// TODO: function not tested
void Eth_BitFieldClear(uint8_t addr, uint8_t data){
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