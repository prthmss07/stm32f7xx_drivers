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
    uint8_t SPI_DeviceMode;   /* Controls MSTR bit (Bit 2) -> Sets Master or Slave role */
    uint8_t SPI_BusConfig;    /* Controls BIDIMODE (Bit 15), BIDIOE (Bit 14), and RXONLY (Bit 10)
                               * -> Sets Full-Duplex, Half-Duplex, or Receive-Only mode
                               */
    uint8_t SPI_SclkSpeed;    /* Controls BR[2:0] bits (Bits 5, 4, 3) -> Sets Baud Rate clock divider */
    uint8_t SPI_DFF;          /* Controls CRCL bit (Bit 11) -> Sets 8-bit or 16-bit data length
                               * (Note: Named 'DFF' on older STM32s, renamed 'CRCL' on newer ones)
                               */
    uint8_t SPI_CPOL;         // Controls CPOL bit (Bit 1) -> Sets Clock Polarity (Idle Low/High)
    uint8_t SPI_CPHA;         // Controls CPHA bit (Bit 0) -> Sets Clock Phase (Sample Edge)
    uint8_t SPI_SSM;          // Controls SSM bit (Bit 9) -> Turns Software Slave Management ON/OFF
                              // (Your code will also toggle the companion SSI bit at Bit 8 based on this)
} SPI_Config_t;

typedef struct {
    SPI_RegDef_t   *pSPIx;       // Holds the base address of the peripheral (SPI1, SPI2, etc.)
    SPI_Config_t  SPIConfig;    // Holds the user configurations from Slide 1
} SPI_Handle_t;


//SPI Modes
#define SPI_DEVICE_MODE_MASTER	1
#define SPI_DEVICE_MODE_SLAVE	0

//Bus Config
#define SPI_BUS_CONFIG_FD		1 //Full duplex
#define SPI_BUS_CONFIG_HD		2 //Half duplex
#define SPI_BUS_CONFIG_SIMPLEX_TXONLY	3
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY	4

//Sclk Speed baud control
#define SPI_SCLK_SPEED_DIV2		0
#define SPI_SCLK_SPEED_DIV4		1
#define SPI_SCLK_SPEED_DIV8		2
#define SPI_SCLK_SPEED_DIV16	3
#define SPI_SCLK_SPEED_DIV32	4
#define SPI_SCLK_SPEED_DIV64	5
#define SPI_SCLK_SPEED_DIV128	6
#define SPI_SCLK_SPEED_DIV256	7

//SPI DFF
#define SPI_DFF_8BITS	0
#define SPI_DFF_16BITS	1

//CPOL
#define SPI_CPOL_HIGH	1
#define SPI_CPOL_LOW	0

//CPHA
#define SPI_CPHA_HIGH	1
#define SPI_CPHA_LOW	0

//SSM
#define SPI_SSM_HW 1
#define SPI_SSM_SW 0




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
