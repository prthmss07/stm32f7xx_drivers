/*
 * stm32f7xx_spi_driver.c
 *
 *  Created on: 16-Jul-2026
 *      Author: prathameshs
 */

#include "stm32f7xx_spi_driver.h"
#include "stm32f7xx.h"
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);

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
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET); //it means buffer aint empty we must wait

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


/**
 * @brief  Receives a block of data over the SPI peripheral (Blocking/Polling mode)
 * @param  pSPIx    : Pointer to the base address of the hardware SPI peripheral (SPI1, SPI2, etc.)
 * @param  pRxBuffer: Pointer to the destination buffer in System RAM where incoming data will land
 * @param  len      : Total number of data bytes to be received
 * @retval None
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len)
{
	// Keep spinning until all expected bytes are pulled from hardware into RAM
	while(len > 0)
	{
		// 1. HARDWARE GUARD: Freeze here until the internal hardware RX Buffer contains data.
		// We wait for the shift register to finish pulling bits from MISO and lifting them into the RX Buffer.
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET); //FLAG_RESET

		// 2. DATA FRAME FORMAT CHECK: Check the DFF bit in Control Register 1
		// to see if we are extracting 16-bit chunks or 8-bit chunks.
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			/* --- 16-Bit Data Frame Format Reception --- */

			// A. Read the 16-bit data packet that just arrived at the DR register.
			// B. Typecast and dereference pRxBuffer to write those 2 full bytes into System RAM all at once.
			*((uint16_t*)pRxBuffer) = pSPIx->DR;

			// Decrement the loop counter by 2 because two bytes were processed simultaneously
			len--;
			len--;

			// Advance our RAM pointer by 2 bytes to open up the next 16-bit landing pad
			pRxBuffer += 2;
		}
		else
		{
			/* --- 8-Bit Data Frame Format Reception --- */

			// A. Read a single 8-bit byte from the hidden silicon RX Buffer via the DR gateway address.
			// B. Dereference the pointer to store that byte into its target location in System RAM.
			*pRxBuffer = pSPIx->DR;

			// Decrement our loop counter by 1 since a single byte was processed
			len--;

			// Advance our RAM pointer by 1 byte to prepare for the next individual byte
			pRxBuffer++;
		}
	}
}

/**
 * @brief  Initiates an asynchronous, non-blocking transmission of data over SPI using interrupts.
 * @note   This function stages the transmission parameters into the handle structure, locks
 *         the peripheral state, and turns on hardware interrupts. It returns immediately while
 *         the actual byte transfer is handled by the ISR in the background.
 * @param  pSPIHandle: Pointer to the SPI Handle structure holding hardware registers and state
 * @param  pTxBuffer : Pointer to the RAM array holding the data to be transmitted
 * @param  len       : Total number of bytes to transmit
 * @retval uint8_t   : Previous state of the peripheral (SPI_READY or SPI_BUSY_IN_TX)
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len)
{
	uint8_t state = pSPIHandle->TxState;

	// Guard against data race conditions: only initiate if peripheral is free
	if(state != SPI_BUSY_IN_TX)
	{
		// 1. Stage the application RAM parameters into the persistent handle clipboard
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = len;

		// 2. Lock the state machine to prevent other threads from corrupting this transaction
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		// 3. Enable the TXE Interrupt Enable bit (TXEIE) in Control Register 2.
		//    The silicon will now trigger a hardware interrupt as soon as the TX buffer is empty.
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}

	// Return the peripheral state prior to execution (allows caller to verify if command was accepted)
	return state;
}

/**
 * @brief  Initiates an asynchronous, non-blocking reception of data over SPI using interrupts.
 * @note   Stages the target destination address in RAM and enables RXNE interrupts.
 * @param  pSPIHandle: Pointer to the SPI Handle structure holding hardware registers and state
 * @param  pRxBuffer : Pointer to the destination RAM array where incoming bytes will land
 * @param  len       : Total number of bytes expected
 * @retval uint8_t   : Previous state of the peripheral (SPI_READY or SPI_BUSY_IN_RX)
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len)
{
    uint8_t state = pSPIHandle->RxState;

    // Guard against data race conditions: only initiate if peripheral is free
    if(state != SPI_BUSY_IN_RX)
    {
        // 1. Stage the application RAM parameters into the persistent handle clipboard
        pSPIHandle->pRxBuffer = pRxBuffer;
        pSPIHandle->RxLen = len;

        // 2. Lock the state machine to prevent concurrent reception calls
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        // 3. Enable the RXNE Interrupt Enable bit (RXNEIE) in Control Register 2.
        //    The silicon will trigger an interrupt as soon as a byte arrives in the RX buffer.
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
    }

    return state;
}

/**
 * @brief  Configures the NVIC interrupt line for a specific peripheral IRQ number.
 * @param  IRQNumber: Hardware IRQ line number assigned by the chip vendor
 * @param  EnorDi   : ENABLE or DISABLE control
 * @retval None
 */
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi) //this is just a generic function, nothing specific to SPI
{
    // Implementation configures the ARM Cortex-M NVIC ISER/ICER registers
	//Writing 1 to ISER enables the interrupt
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            // program ISER0 register
            *ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            // program ISER1 register
            *ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            // program ISER2 register
            *ISER2 |= (1 << (IRQNumber % 32));
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            // program ICER0 register
            *ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            // program ICER1 register
            *ICER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            // program ICER2 register
            *ICER2 |= (1 << (IRQNumber % 32));
        }
    }
}

/**
 * @brief  Sets the priority level for a specific hardware IRQ line inside the NVIC.
 * @param  IRQNumber  : Hardware IRQ line number
 * @param  IRQPriority: Priority numerical value (0 = highest priority)
 * @retval None
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    // Implementation configures the ARM Cortex-M NVIC IPR registers
    uint32_t iprx = IRQNumber / 4;
    uint32_t iprx_section = IRQNumber % 4;

    // 1. Shift the priority to the top of an 8-bit block (e.g., if 4 bits implemented, shift by 4)
    uint8_t aligned_priority = (IRQPriority << (8 - NO_PR_BITS_IMPLEMENTED));

    // 2. Shift that aligned 8-bit block to the correct section (0, 8, 16, or 24 bits)
    uint8_t shift_amount = (8 * iprx_section);

    // 3. Clear the old priority bits first to prevent overlapping data, then set new ones
    *(NVIC_PR_BASE_ADDRESS + iprx) &= ~(0xFF << shift_amount);
    *(NVIC_PR_BASE_ADDRESS + iprx) |= (aligned_priority << shift_amount);
}

/**
 * @brief  Centralized Interrupt Service Routine (ISR) Handler for SPI peripherals.
 * @note   Evaluates both the Status Register flags (event occurred) AND Control Register bits
 *         (interrupt enabled) to identify what caused the interrupt and pass the execution to dedicated sub-handlers.
 * @param  pHandle: Pointer to the SPI Handle structure
 * @retval None
 */
void SPI_IRQHandler(SPI_Handle_t *pHandle)
{
    uint8_t temp1, temp2;

    // ------------------ 1. PROCESS TRANSMIT (TXE) INTERRUPTS ------------------
    temp1 = pHandle->pSPIx->SR  & (1 << SPI_SR_TXE);      // Check if TX Buffer is Empty
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);  // Check if TXE Interrupt is Enabled

    if (temp1 && temp2)
    {
        // Valid TXE interrupt event detected: service the outbound data pipeline
        spi_txe_interrupt_handle(pHandle);
    }

    // ------------------ 2. PROCESS RECEIVE (RXNE) INTERRUPTS ------------------
    temp1 = pHandle->pSPIx->SR  & (1 << SPI_SR_RXNE);     // Check if RX Buffer contains Data
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE); // Check if RXNE Interrupt is Enabled

    if (temp1 && temp2)
    {
        // Valid RXNE interrupt event detected: service the inbound data pipeline
        spi_rxne_interrupt_handle(pHandle);
    }

    // ------------------- 3. PROCESS OVERRUN (OVR) ERRORS -------------------
    temp1 = pHandle->pSPIx->SR  & (1 << SPI_SR_OVR);     // Check if Overrun Error flag is set
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);  // Check if Error Interrupt is Enabled

    if (temp1 && temp2)
    {
        // Valid Overrun error detected: invoke error handling logic
        spi_ovr_err_interrupt_handle(pHandle);
    }
}

/**
 * @brief  Private helper routine to handle TXE interrupts.
 * @note   Transfers a single frame (8-bit or 16-bit) from RAM into the DR register.
 *         Automatically closes the transaction and triggers an event callback when Len hits 0.
 * @param  pSPIHandle: Pointer to the SPI Handle structure
 * @retval None
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    // Evaluate Data Frame Format (DFF) configuration bit in CR1
    if (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        /* --- 16-Bit Data Frame Processing --- */

        // 1. Fetch 2 bytes from RAM and write the 16-bit word directly to the DR register
        pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);

        // 2. Decrement remaining byte count by 2
        pSPIHandle->TxLen--;
        pSPIHandle->TxLen--;

        // 3. Increment source RAM address by 2 bytes
        pSPIHandle->pTxBuffer += 2;
    }
    else
    {
        /* --- 8-Bit Data Frame Processing --- */

        // 1. Write single byte from RAM to the DR register
        pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;

        // 2. Decrement remaining byte count by 1
        pSPIHandle->TxLen--;

        // 3. Increment source RAM address by 1 byte
        pSPIHandle->pTxBuffer++;
    }

    // ------------------ CLOSING THE TRANSMISSION TRANSACTION ------------------
    void SPI_CloseTransmission(SPI_Handle_t *pHandle);
    // Notify the application layer via asynchronous callback that transmission is complete
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
}

/**
 * @brief  Private helper function to handle Receive Interrupts (RXNE).
 * @note   Executed by the central ISR whenever new data arrives in the DR register.
 *         Reads 8-bit or 16-bit data, stores it into RAM, updates buffer pointers,
 *         and automatically closes reception when RxLen reaches zero.
 * @param  pSPIHandle: Pointer to the SPI Handle structure holding registers and state
 * @retval None
 */
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    // Check the Data Frame Format (DFF) bit in CR1 register (0 = 8-bit, 1 = 16-bit)
    if (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        /* --- 16-Bit Data Frame Processing --- */

        // 1. Read 16 bits of incoming data from DR and write it into RAM
        *((uint16_t*)pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;

        // 2. Decrement remaining byte count by 2
        pSPIHandle->RxLen--;
        pSPIHandle->RxLen--;

        // 3. Advance target RAM buffer address by 2 bytes
        pSPIHandle->pRxBuffer += 2;
    }
    else
    {
        /* --- 8-Bit Data Frame Processing --- */

        // 1. Read 1 byte of incoming data from DR and write it into RAM
        *((uint8_t*)pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;

        // 2. Decrement remaining byte count by 1
        pSPIHandle->RxLen--;

        // 3. Advance target RAM buffer address by 1 byte
        pSPIHandle->pRxBuffer++;
    }

    // ------------------ CLOSING RECEPTION TRANSACTION ------------------
    if (!pSPIHandle->RxLen)
    {
        // All requested bytes have been received!
        // Turn off interrupts and release the SPI peripheral state.
        SPI_CloseReception(pSPIHandle);

        // Notify the application layer via callback that reception is complete
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
    }
}

/**
 * @brief  Private helper function to handle Overrun Error Interrupts (OVR).
 * @note   Triggered when incoming data overwrites unread data in the DR register.
 *         Clears the hardware flag and notifies the application layer.
 * @param  pSPIHandle: Pointer to the SPI Handle structure holding registers and state
 * @retval None
 */
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    uint8_t temp;

    // 1. Clear the hardware Overrun (OVR) flag.
    //    Per the hardware reference manual, reading DR followed by SR clears OVR.
    if (pSPIHandle->TxState != SPI_BUSY_IN_TX)
    {
        temp = pSPIHandle->pSPIx->DR;
        temp = pSPIHandle->pSPIx->SR;
    }

    // Silences compiler warnings regarding the unused 'temp' variable
    (void)temp;

    // 2. Notify the application layer that an Overrun Error occurred
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}

/**
 * @brief  Closes an active SPI transmission transaction.
 * @note   Disables the TXE interrupt bit, resets transmission handle variables,
 *         and restores the state machine back to SPI_READY.
 * @param  pSPIHandle: Pointer to the SPI Handle structure
 * @retval None
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
    if (!pSPIHandle->TxLen)
    {
        // 1. Disable the TXE Interrupt Enable bit (TXEIE) in CR2 register
        pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);

        // 2. Clear handle parameters and set state to READY to unblock future calls
        pSPIHandle->pTxBuffer = NULL;
        pSPIHandle->TxLen = 0;
        pSPIHandle->TxState = SPI_READY;
    }
}

/**
 * @brief  Closes an active SPI reception transaction.
 * @note   Disables the RXNE interrupt bit, resets reception handle variables,
 *         and restores the state machine back to SPI_READY.
 * @param  pSPIHandle: Pointer to the SPI Handle structure
 * @retval None
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
    if (!pSPIHandle->RxLen)
    {
        // 1. Disable the RXNE Interrupt Enable bit (RXNEIE) in CR2 register
        pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

        // 2. Clear handle parameters and set state to READY to unblock future calls
        pSPIHandle->pRxBuffer = NULL;
        pSPIHandle->RxLen = 0;
        pSPIHandle->RxState = SPI_READY;
    }
}

/**
 * @brief  Application Event Callback function.
 * @note   Marked as '__weak' so it can be overridden in main.c by the application layer
 *         to handle asynchronous events (TX Complete, RX Complete, Errors).
 * @param  pSPIHandle: Pointer to the SPI Handle structure
 * @param  AppEv     : Event identifier macro (e.g., SPI_EVENT_TX_CMPLT)
 * @retval None
 */
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
    // Default weak implementation (empty placeholder).
    // The application developer can redefine this function in main.c
}



