/*
 * stm32f7xx_spi_driver.c
 *
 *  Created on: 16-Jul-2026
 *      Author: prathameshs
 */


#include "stm32f7xx_spi_driver.h"
#include "stm32f7xx.h"

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
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


void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
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

/*
 * Initializing SPI means substituting the user entered input from the
 * configuration structure inside the CR1 Register.
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//initialize tempreg (temp register)
	uint32_t tempreg = 0;

	//1. Configure device mode
	tempreg |= (pSPIHandle->SPIConfig.SPI_DeviceMode << 2);

	//2. Configure the bus (RX only, half-, full-DUPLEX etc)
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//Clear BIDI bit and RXONLY
		tempreg &= ~(1 << 15) & ~(1 << 10);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//Set BIDI bit and clear RXONLY
		tempreg |= (1 << 15);
		tempreg &= ~(1 << 10);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//set RXONLY bit and clear BIDI
		tempreg |= (1 << 10);
		tempreg &= ~(1 << 15);

	}
	//3. configure SCLK speed
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << 3; //as the values in macro already are in 3 bit length, we just push them 3 positions in

	//4. Configure DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << 11;

	//5. Configure CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << 1;

	//6. Configure CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << 0;
//afterwards replacing these hardcoded numbers with micros
	/*We have collected all the user configured values and implemented in tempreg
	 * now we just paste the tempreg values in the real CR1 register */
	pSPIHandle->pSPIx->CR1 = tempreg;
}




