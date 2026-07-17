/*
 * stm32f7xx_spi_driver.h
 *
 *  Created on: 16-Jul-2026
 *      Author: prathameshs
 */

#ifndef STM32F7XX_SPI_DRIVER_H_
#define STM32F7XX_SPI_DRIVER_H_

#include "stm32f7xx.h"

typedef struct {
    uint8_t SPI_DeviceMode;   // Master or Slave (Controls MSTR bit)
    uint8_t SPI_BusConfig;    // Full-Duplex, Half-Duplex, or Simplex (Rx Only)
    uint8_t SPI_SclkSpeed;         // Baud Rate Prescaler (/2, /4, /8, etc.)
    uint8_t SPI_DFF;          // 8-bit or 16-bit data frame format
    uint8_t SPI_CPOL;         // Clock Polarity (Idle Low or Idle High)
    uint8_t SPI_CPHA;         // Clock Phase (First edge or Second edge)
    uint8_t SPI_SSM;          // Software Slave Management (Enable/Disable)
} SPI_Config_t;

typedef struct {
    SPI_RegDef_t   *pSPIx;       // Holds the base address of the peripheral (SPI1, SPI2, etc.)
    SPI_Config_t  SPIConfig;    // Holds the user configurations from Slide 1
} SPI_Handle_t;


//SPI Modes
#define SPI_DEVICE_MODE_MASTER
#define SPI_DEVICE_MODE_SLAVE

//Bus Config
#define SPI_BUS_CONFIG_FD //Full duplex
#define SPI_BUS_CONFIG_HD //Half duplex
#define SPI_BUS_CONFIG_S_TXONLY
#define SPI_BUS_CONFIG_S_RXONLY


// ******************APIs Supported by this driver***************************

//Peripheral clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

//Init and de-init
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

//Data send and receive
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len );
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);

//IRQ Configuration and IRQ handling
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandler(SPI_Handle_t *pHandle);


//Other peripheral APIs










#endif /* STM32F7XX_SPI_DRIVER_H_ */
