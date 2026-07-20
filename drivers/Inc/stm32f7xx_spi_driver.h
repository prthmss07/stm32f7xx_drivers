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

typedef struct
{
    SPI_RegDef_t *pSPIx;      /*!< This holds the base address of SPIx(x:0,1,2) peripheral */
    SPI_Config_t SPIConfig;

    uint8_t  *pTxBuffer;      /*!< To store the app. Tx buffer address */
    uint8_t  *pRxBuffer;      /*!< To store the app. Rx buffer address */

    uint32_t TxLen;           /*!< To store Tx len */
    uint32_t RxLen;           /*!< To store Rx len */

    uint8_t  TxState;         /*!< To store Tx state */
    uint8_t  RxState;         /*!< To store Rx state */

} SPI_Handle_t;


//SPI application states
#define SPI_READY		0
#define SPI_BUSY_IN_RX	1
#define SPI_BUSY_IN_TX	2

/*
 * Possible SPI Application events
 */
#define SPI_EVENT_TX_CMPLT    1
#define SPI_EVENT_RX_CMPLT    2
#define SPI_EVENT_OVR_ERR     3
#define SPI_EVENT_CRC_ERR     4


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

/******************************************************************************/
/*                                                                            */
/*                      SPI Register Bit Positions                            */
/*                                                                            */
/******************************************************************************/

/* Bit position definitions for SPI Control Register 1 (SPIx_CR1) */
#define SPI_CR1_CPHA            0   /* Clock phase */
#define SPI_CR1_CPOL            1   /* Clock polarity */
#define SPI_CR1_MSTR            2   /* Master selection */
#define SPI_CR1_BR              3   /* Baud rate control (Starts at Bit 3, occupies 3,4,5) */
#define SPI_CR1_SPE             6   /* SPI peripheral enable */
#define SPI_CR1_LSBFIRST        7   /* Frame format */
#define SPI_CR1_SSI             8   /* Internal slave select */
#define SPI_CR1_SSM             9   /* Software slave management */
#define SPI_CR1_RXONLY          10  /* Receive only mode enabled */
#define SPI_CR1_DFF	            11  /* CRC length */
#define SPI_CR1_CRCNEXT         12  /* Transmit CRC next */
#define SPI_CR1_CRCEN           13  /* Hardware CRC calculation enable */
#define SPI_CR1_BIDIOE          14  /* Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE        15  /* Bidirectional data mode enable */

/* Bit position definitions for SPI Control Register 2 (SPIx_CR2) */
#define SPI_CR2_RXDMAEN         0   /* Rx buffer DMA enable */
#define SPI_CR2_TXDMAEN         1   /* Tx buffer DMA enable */
#define SPI_CR2_SSOE            2   /* SS output enable */
#define SPI_CR2_NSSP            3   /* NSS pulse management */
#define SPI_CR2_FRF             4   /* Frame format */
#define SPI_CR2_ERRIE           5   /* Error interrupt enable */
#define SPI_CR2_RXNEIE          6   /* RX buffer not empty interrupt enable */
#define SPI_CR2_TXEIE           7   /* Tx buffer empty interrupt enable */

#define SPI_CR2_DS              8   /* Data size (Starts at Bit 8, occupies Bits 8-11) */

#define SPI_CR2_FRXTH           12  /* FIFO reception threshold */
#define SPI_CR2_LDMARX          13  /* Last DMA transfer for reception */
#define SPI_CR2_LDMATX          14  /* Last DMA transfer for transmission */
/* Bit 15 is Reserved */

/* Bit position definitions for SPI Status Register (SPIx_SR) */
#define SPI_SR_RXNE             0   /* Receive buffer not empty */
#define SPI_SR_TXE              1   /* Transmit buffer empty */
#define SPI_SR_CHSIDE           2   /* Channel side */
#define SPI_SR_UDR              3   /* Underrun flag */
#define SPI_SR_CRCERR           4   /* CRC error flag */
#define SPI_SR_MODF             5   /* Mode fault */
#define SPI_SR_OVR              6   /* Overrun flag */
#define SPI_SR_BSY              7   /* Busy flag */
#define SPI_SR_FRE              8   /* Frame format error */
#define SPI_SR_FRLVL            9   /* FIFO reception level (Starts at Bit 9, occupies 9,10) */
#define SPI_SR_FTLVL            11  /* FIFO transmission level (Starts at Bit 11, occupies 11,12) */


/******************************************************************************/
/*                                                                            */
/*                      SPI Flag Status Bitmasks                              */
/*                                                                            */
/******************************************************************************/

/* Handy bitmasks to pass into your SPI_GetFlagStatus API */
#define SPI_RXNE_FLAG          (1 << SPI_SR_RXNE)
#define SPI_TXE_FLAG           (1 << SPI_SR_TXE)
#define SPI_CHSIDE_FLAG        (1 << SPI_SR_CHSIDE)
#define SPI_UDR_FLAG           (1 << SPI_SR_UDR)
#define SPI_CRCERR_FLAG        (1 << SPI_SR_CRCERR)
#define SPI_MODF_FLAG          (1 << SPI_SR_MODF)
#define SPI_OVR_FLAG           (1 << SPI_SR_OVR)
#define SPI_BUSY_FLAG          (1 << SPI_SR_BSY)
#define SPI_FRE_FLAG           (1 << SPI_SR_FRE)



// ******************APIs Supported by this driver***************************

//Peripheral clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

//Init and de-init
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

//Data send and receive
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len );
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len);

//Data send and receive interrupt based
void SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len );
void SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len);

//IRQ Configuration and IRQ handling
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandler(SPI_Handle_t *pHandle);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pHandle);
void SPI_CloseReception(SPI_Handle_t *pHandle);

//application callback
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);

//Other peripheral APIs










#endif /* STM32F7XX_SPI_DRIVER_H_ */
