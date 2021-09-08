#ifndef ETHERNET_ENC28J60
#define ETHERNET_ENC28J60

#include "stdint.h"
#include "ethernet_regs.h"
#include "debug.h"
#include "spi.h"
#include "gpio.h"

#define MAC_ADDR {0x00, 0x34, 0x56, 0x78, 0x9A, 0x00}

#define CS_SEL    HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 0)
#define CS_DESEL  HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 1) 
#define ISMII(arg)  (arg & MII_ID)
#define ISMAC(arg)  (arg & MAC_ID)

#define ETH_SPI hspi1

#define READ_ETH_SPI_BYTE(byte) HAL_SPI_Receive(&ETH_SPI, byte, 1, ETH_SPI_RECEIVE_TIMEOUT)
#define WRITE_ETH_SPI_BYTE(byte) HAL_SPI_Transmit(&ETH_SPI, byte, 1, ETH_SPI_TRANSMIT_TIMEOUT)

// ETHERNET macro TODO: change the way spi connected

#define ENC28J60_OK         0x00
#define ENC28J60_TIMEOUT    0x01
#define ENC28J60_ETHERR     0x02
#define ENC28J60_MACERR     0x03
#define ENC28J60_PHYERR     0x04
#define ENC28J60_ERROR      0x09

/**
 * @brief macros for ethernet
 */

/**
 * @brief minor ethernet module commands
 */
static void Eth_SwitchControlRegisterBank(uint8_t bank);

// Read control register
uint8_t Eth_ReadControlRegister(uint8_t addr);
uint16_t Eth_ReadControlRegister_16(uint8_t addr);

// Write control register
void Eth_WriteControlRegister(uint8_t addr, uint8_t data);
void Eth_WriteControlRegister_16(uint8_t addr, uint16_t data);

// Read/Write buffer memory TODO: untested
void Eth_ReadBufferMemory(uint8_t * data, uint16_t length);
void Eth_WriteBufferMemory(uint8_t * data, uint16_t length);

// Bitfield Set/Reset
void Eth_BitFieldSet(uint8_t addr, uint8_t data);
void Eth_BitFieldClear(uint8_t addr, uint8_t data);

// Soft reset
void Eth_SystemResetCommand(void);

// PHY registers Read/Write
uint16_t PHY_ReadRegister(uint8_t addr);
void PHY_WriteRegister(uint8_t addr, uint16_t data);

// Init function
uint8_t ENC28J60_Init(void);

// Send/Receive packet
uint8_t Eth_SendPacket(uint8_t * data, uint8_t length);
uint8_t Eth_ReceivePacket(uint8_t * data, uint16_t length);


#endif