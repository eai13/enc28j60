#ifndef ETHERNET_ENC28J60
#define ETHERNET_ENC28J60

#include "stdint.h"
#include "ethernet_regs.h"
#include "debug.h"
#include "spi.h"
#include "gpio.h"

#define CS_SEL    HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 0)
#define CS_DESEL  HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 1) 
#define ISMII(arg)  (arg & MII_ID)
#define ISMAC(arg)  (arg & MAC_ID)
#define READ_ETH_SPI_BYTE(byte) HAL_SPI_Receive(&ETH_SPI, byte, 1, ETH_SPI_RECEIVE_TIMEOUT)
#define WRITE_ETH_SPI_BYTE(byte) HAL_SPI_Transmit(&ETH_SPI, byte, 1, ETH_SPI_TRANSMIT_TIMEOUT)

// ETHERNET macro TODO: change the way spi connected
#define ETH_SPI hspi1

/**
 * @brief macros for ethernet
 */

/**
 * @brief minor ethernet module commands
 */
static void Eth_SwitchControlRegisterBank(uint8_t bank);

uint8_t Eth_ReadControlRegister(uint8_t addr);
uint16_t Eth_ReadControlRegister_16(uint8_t addr);

uint8_t Eth_ReadBufferMemory(void);

void Eth_WriteControlRegister(uint8_t addr, uint8_t data);
void Eth_WriteControlRegister_16(uint8_t addr, uint16_t data);

void Eth_WriteBufferMemory(uint8_t data);
void Eth_BitFieldSet(uint8_t addr, uint8_t data);
void Eth_BitFieldClear(uint8_t addr, uint8_t data);
void Eth_SystemResetCommand(void);

// PHY registers
uint16_t PHY_ReadRegister(uint8_t addr);
void PHY_WriteRegister(uint8_t addr, uint16_t data);

uint8_t ENC28J60_Init(void);


#endif