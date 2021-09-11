#ifndef ETHERNET_ENC28J60
#define ETHERNET_ENC28J60

#include "stdint.h"
#include "ethernet_regs.h"
#include "debug.h"
#include "spi.h"
#include "gpio.h"

#define DELAY(ms) HAL_Delay(ms)

#define MAC_ADDR {0xa5, 0x11, 0x83, 0x91, 0x42, 0x25}

#define CS_SEL    HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 0)
#define CS_DESEL  HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 1) 
#define ISMII(arg)  (arg & MII_ID)
#define ISMAC(arg)  (arg & MAC_ID)

// ETHERNET macro FIXME: change the way spi connected
#define ETH_SPI hspi1

#define READ_ETH_SPI_BYTE(byte) HAL_SPI_Receive(&ETH_SPI, byte, 1, ETH_SPI_RECEIVE_TIMEOUT)
#define WRITE_ETH_SPI_BYTE(byte) HAL_SPI_Transmit(&ETH_SPI, byte, 1, ETH_SPI_TRANSMIT_TIMEOUT)
#define WRITE_ETH_SPI_2BYTE(byte) HAL_SPI_Transmit(&ETH_SPI, byte, 2, ETH_SPI_TRANSMIT_TIMEOUT)

#define WRITE_READ_ETH_SPI(in, out) HAL_SPI_TransmitReceive(&ETH_SPI, in, out, 2, ETH_SPI_TRANSMIT_TIMEOUT)


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
static inline void Eth_SwitchControlRegisterBank(uint8_t bank);

// Read control register
static inline uint8_t Eth_ReadControlRegister(uint8_t addr);
static inline uint16_t Eth_ReadControlRegister_16(uint8_t addr);

// Write control register
static inline void Eth_WriteControlRegister(uint8_t addr, uint8_t data);
static inline void Eth_WriteControlRegister_16(uint8_t addr, uint16_t data);

// Write and read buffer
static inline void Eth_ReadBufferMemory(uint8_t * data, uint16_t length);
static inline void Eth_WriteBufferMemory(uint8_t * data, uint16_t length);

// Bitfield Set/Reset
static inline void Eth_BitFieldSet(uint8_t addr, uint8_t data);
static inline void Eth_BitFieldClear(uint8_t addr, uint8_t data);

// Soft reset
static inline void Eth_SystemResetCommand(void);

// PHY registers Read/Write
static inline uint16_t PHY_ReadRegister(uint8_t addr);
static inline void PHY_WriteRegister(uint8_t addr, uint16_t data);

// Init function
uint8_t ENC28J60_Init(void);

// Send/Receive packet
uint16_t Eth_SendPacket(uint8_t * data, uint8_t length);
uint16_t Eth_ReceivePacket(uint8_t * data, uint16_t length);


#endif