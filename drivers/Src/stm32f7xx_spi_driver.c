/*
 * stm32f7xx_spi_driver.c
 *
 *  Created on: 16-Jul-2026
 *      Author: prathameshs
 */

#include "stm32f7xx_spi_driver.h"
#include "stm32f7xx.h"

/**
 * @brief  Enables or disables the peripheral clock for the given SPI instance.
 * @param  pSPIx   : Base address of the SPI peripheral (SPI1, SPI2, etc.)
 * @param  EnorDi  : Clock state control (ENABLE or DISABLE)
 * @retval None
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		// Identify the physical peripheral block and trigger its specific RCC gate macro
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
	}
	else if(EnorDi == DISABLE)
	{
		// Shut down the clock gate to the target peripheral to save power
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}

/**
 * @brief  Resets all registers of the target SPI peripheral back to default states.
 * @param  pSPIx : Base address of the SPI peripheral (SPI1, SPI2, etc.)
 * @retval None
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	// Leverage the MCU's internal Reset Registers (RCC_RSTR) to instantly clear the silicon blocks
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}
	else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}
	else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}
	else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}
}

/**
 * @brief  Initializes the SPI peripheral according to user configurations.
 * @note   Configures all properties in a temporary variable first to prevent multiple
 *         live register writes, minimizing risk of hardware glitches.
 * @param  pSPIHandle : Pointer to the SPI Handle structure holding base address and config properties
 * @retval None
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// Temporary register used to stage all configurations before committing to live silicon
	uint32_t tempreg = 0;

	// 1. DEVICE MODE CONFIGURATION (Master vs Slave selection)
	tempreg |= (pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR);

	// 2. BUS CONFIGURATION (Full-Duplex, Half-Duplex, or Simplex Rx-Only)
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// Full Duplex: 2 lines active. Clear BIDIMODE (2-line mode) and clear RXONLY (Full TX/RX)
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg &= ~(1 << SPI_CR1_RXONLY);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// Half Duplex: 1 bidirectional line active. Set BIDIMODE (1-line mode) and clear RXONLY
		tempreg |= (1 << SPI_CR1_BIDIMODE);
		tempreg &= ~(1 << SPI_CR1_RXONLY);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// Simplex Rx-Only: Disconnects the internal TX paths. Set RXONLY and clear BIDIMODE
		tempreg |= (1 << SPI_CR1_RXONLY);
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}

	// 3. SERIAL CLOCK SPEED CONFIGURATION (Baud Rate Prescaler selection)
	// User macros provide pre-shifted 3-bit values; we shift them to the BR bit fields
	tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	// 4. DATA FRAME FORMAT CONFIGURATION (8-bit vs 16-bit wide data packets)
	tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

	// 5. CLOCK POLARITY CONFIGURATION (Sets base state of SCK wire during idle)
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

	// 6. CLOCK PHASE CONFIGURATION (Determines which clock edge captures the data bits)
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	/* --------------------------- HARDWARE COMMIT ---------------------------
	 * All configuration options are gathered safely inside the local RAM variable 'tempreg'.
	 * We now execute a single parallel write statement to commit all options to live silicon!
	 * ----------------------------------------------------------------------- */
	pSPIHandle->pSPIx->CR1 = tempreg;
}

/**
 * @brief  Checks the state of a specific status flag inside the SPI Status Register (SR).
 * @param  pSPIx    : Base address of the target SPI hardware block
 * @param  FlagName : Hexadecimal bitmask representing the target flag bit (e.g., SPI_TXE_FLAG)
 * @retval FLAG_SET or FLAG_RESET
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	// Isolate the bit of interest using a bitwise AND operator
    if(pSPIx->SR & FlagName)
    {
        return FLAG_SET;   // Target bit is '1' (Active/True)
    }
    return FLAG_RESET;     // Target bit is '0' (Inactive/False)
}
//Data send and receive
/**
 * @brief  Transmits a block of data over the SPI peripheral (Blocking/Polling mode)
 * @param  pSPIx    : Pointer to the base address of the hardware SPI peripheral (SPI1, SPI2, etc.)
 * @param  pTxBuffer: Pointer to the source data array sitting in System RAM
 * @param  len      : Total number of data bytes to be transmitted
 * @retval None
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{
	// Keep spinning until all requested bytes are pushed from RAM into the hardware
	while(len > 0)
	{
		// 1. HARDWARE GUARD: Freeze here until the internal hardware TX Buffer is empty.
		// If the shift register is busy pumping out old data, we wait to prevent corruption.
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		// 2. DATA FRAME FORMAT CHECK: Check the DFF bit in Control Register 1
		// to see if we are dealing with 16-bit chunks or 8-bit chunks.
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			/* --- 16-Bit Data Frame Format Transmission --- */

			// A. Dereference after typecasting to grab 2 full bytes from RAM at once.
			// B. Write the 16-bit chunk to the DR gateway address, which parallel-drops
			//    it straight into the hidden silicon TX Buffer.
			pSPIx->DR = *((uint16_t*)pTxBuffer);

			// Decrement the loop counter by 2 because two bytes were processed simultaneously
			len--;
			len--;

			// Advance our RAM pointer by 2 bytes to point to the next 16-bit chunk
			pTxBuffer += 2;
		}
		else
		{
			/* --- 8-Bit Data Frame Format Transmission --- */

			// A. Dereference the pointer to grab a single 8-bit byte out of RAM.
			// B. Push it through the DR gateway address into the hidden silicon TX Buffer.
			pSPIx->DR = *pTxBuffer;

			// Decrement our loop counter by 1 since a single byte was processed
			len--;

			// Advance our RAM pointer by 1 byte to point to the next individual byte
			pTxBuffer++;
		}
	}
}


void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);



