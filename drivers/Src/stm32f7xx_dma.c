/*
 * stm32f7xx_dma.c
 *
 *  Created on: 30-Jul-2026
 *      Author: prathameshs
 */


#include "stm32f7xx_dma.h"
#include "stm32f7xx.h"

/* =================================================================================
 * 1. PERIPHERAL CLOCK & INITIALIZATION APIs
 * ================================================================================= */

/**
 * @brief  Enables or disables the peripheral clock for DMA1 or DMA2.
 * @param  pDMAx: Base address of the DMA controller (DMA1 or DMA2).
 * @param  EnOrDis: ENABLE or DISABLE macro.
 */
void DMA_PeriClockControl(DMA_RegDef_t *pDMAx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		// Identify the physical peripheral block and trigger its specific RCC gate macro
		if(pDMAx == DMA1)
		{
			DMA1_PCLK_EN();
		}
		else if(pDMAx == DMA2)
		{
			DMA2_PCLK_EN();
		}
	}
	else if(EnorDi == DISABLE)
	{
		// Shut down the clock gate to the target peripheral to save power
		if(pDMAx == DMA1)
		{
			DMA1_PCLK_DI();
		}
		else if(pDMAx == DMA2)
		{
			DMA2_PCLK_DI();
		}
	}
}


/**
 * @brief  Initializes the specified DMA Stream based on the handle configuration
 * @param  pDMAHandle: Pointer to a DMA_Handle_t structure
 */
void DMA_Init(DMA_Handle_t *pDMAHandle)
{
    uint32_t tempreg = 0;

    //  DISABLE STREAM BEFORE CONFIGURATION
    pDMAHandle->pStream->CR &= ~(1 << DMA_SxCR_EN);

//wait till stream is disabled
    while (pDMAHandle->pStream->CR & (1 << DMA_SxCR_EN));

    // 3. CONFIGURE STREAM CONTROL REGISTER (DMA_SxCR)

    // a. Channel Selection (Bits 27:25)
    tempreg |= (pDMAHandle->DMA_Config.Channel << DMA_SxCR_CHSEL);

    // b. Software Priority Level (Bits 17:16)
    tempreg |= (pDMAHandle->DMA_Config.Priority << DMA_SxCR_PL);

    // c. Memory Data Size (Bits 14:13) & Peripheral Data Size (Bits 12:11)
    tempreg |= (pDMAHandle->DMA_Config.MSize << DMA_SxCR_MSIZE);
    tempreg |= (pDMAHandle->DMA_Config.PSize << DMA_SxCR_PSIZE);

    // d. Memory Increment (Bit 10) & Peripheral Increment (Bit 9)
    tempreg |= (pDMAHandle->DMA_Config.MInc << DMA_SxCR_MINC);
    tempreg |= (pDMAHandle->DMA_Config.PInc << DMA_SxCR_PINC);

    // e. Transfer Direction (Bits 7:6)
    tempreg |= (pDMAHandle->DMA_Config.Direction << DMA_SxCR_DIR);

    // f. Circular Mode (Bit 8)
    tempreg |= (pDMAHandle->DMA_Config.Mode << DMA_SxCR_CIRC);

    // Commit staged configuration to live hardware CR register
    pDMAHandle->pStream->CR = tempreg;


    // 4. CONFIGURE FIFO CONTROL REGISTER (DMA_SxFCR)
    tempreg = 0; // Reset local staging variable

    if (pDMAHandle->DMA_Config.FIFOMode == DMA_FIFO_ENABLE)
    {
        // Set DMDIS (Bit 2) = 1 to disable Direct Mode (Enables FIFO)
        tempreg |= (1 << DMA_SxFCR_DMDIS);

        // Set FIFO Threshold (Bits 1:0)
        tempreg |= (pDMAHandle->DMA_Config.FIFOThreshold << DMA_SxFCR_FTH);
    }
    // Note: If FIFOMode is DISABLE, tempreg stays 0 (Direct Mode enabled by default)

    // Commit staged configuration to live hardware FCR register
    pDMAHandle->pStream->FCR = tempreg;
}

/**
 * @brief  Resets a DMA Stream back to its hardware default state.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 */
void DMA_DeInit(DMA_Handle_t *pDMAHandle)
{
	// 1. Disable the stream and wait for hardware confirmation
	pDMAHandle->pStream->CR &= ~(1 << DMA_SxCR_EN);
    while (pDMAHandle->pStream->CR & (1 << DMA_SxCR_EN));

    // 2. Reset stream registers to hardware default reset values (0x00000000)
    pDMAHandle->pStream->CR   = 0;
    pDMAHandle->pStream->NDTR = 0;
    pDMAHandle->pStream->PAR  = 0;
    pDMAHandle->pStream->M0AR = 0;
    pDMAHandle->pStream->M1AR = 0;

    // 3. Reset FIFO Control Register to hardware reset default (0x00000021)
    pDMAHandle->pStream->FCR  = 0x00000021;

    // 4. Clear all pending status flags for this specific stream in LIFCR / HIFCR
    DMA_ClearAllStreamFlags(pDMAHandle);
}






