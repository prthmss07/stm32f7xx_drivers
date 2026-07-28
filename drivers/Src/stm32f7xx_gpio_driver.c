/*
 * stm32f7xx_gpio_driver.c
 *
 *  Created on: 08-Jul-2026
 *      Author: prathameshs
 */


#include "stm32f7xx_gpio_driver.h"

/**
 * @brief  Enables or disables the peripheral clock for the given GPIO port.
 * @note   Every GPIO port (A through I) sits on the AHB1 bus and has its own
 *         individual clock gate bit in RCC_AHB1ENR. The MCU powers down
 *         unused peripheral clocks by default to save power, so this must
 *         be called before touching any other register of that port.
 * @param  pGPIOx  : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  EnorDi  : Clock state control (ENABLE or DISABLE)
 * @retval None
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		// Identify the physical GPIO port and switch on its specific RCC clock gate
		if(pGPIOx == GPIOA)
		{
			GPIOA_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PERI_CLK_EN();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PERI_CLK_EN();
		}
	}
	else if(EnorDi == DISABLE)
	{
		// Shut down the clock gate to the target GPIO port to save power
		if(pGPIOx == GPIOA)
		{
			GPIOA_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PERI_CLK_DI();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PERI_CLK_DI();
		}
	}
}

/**
 * @brief  Initializes a single GPIO pin according to the user's configuration.
 * @note   GPIO modes split into two families:
 *         - Non-interrupt modes (IN/OUT/ALTFN/ANALOG) only define electrical
 *           behaviour (read, drive, analog, or peripheral function). They never
 *           notify the CPU when the pin's voltage changes.
 *         - Interrupt modes (rising/falling/both edge) additionally route the
 *           pin through the EXTI controller, which detects the edge and raises
 *           an interrupt so the CPU can run the matching ISR automatically.
 *         MODER, OSPEEDR, and PUPDR each use 2 bits per pin (4 possible states),
 *         so every field is shifted by (2 * PinNumber) to land on the correct
 *         pin's bit-pair. OTYPER and interrupt trigger registers use only
 *         1 bit per pin, so those use a plain PinNumber shift instead.
 * @param  pGPIOHandle : Pointer to the handle holding the target port and the
 *                       pin's desired mode/speed/pull/output-type/AF settings
 * @retval None
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; // temp = temporary staging value before writing to the live register

	//1. Configure GPIO pin mode
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		// Non-interrupt mode: write directly into the 2-bit MODER field for this pin

		// Shift the user's mode value into position: pin N's bits live at (2*N, 2*N+1)
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

		// Clear the existing 2 bits first (OR can only set bits, never clear them),
		// then OR in the new value so only this pin's field changes
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); // clearing
		pGPIOHandle->pGPIOx->MODER |= temp; //setting
	}
	else
	{
		// Interrupt mode: route this pin through the EXTI controller instead of MODER
		//
		// EXTI0 always corresponds to PinNumber 0 of whichever port is selected
		// (PA0, PB0, PC0, ...), and so on for all 16 EXTI lines (one per pin number).
		// Because of this, only one port's Pin-N can generate an EXTI-N interrupt
		// at a time - selecting a new port for that line silently disconnects the old one.
		//
		// Step 1: the pin is implicitly treated as an input, since detecting an
		// edge on an output pin driven by this same MCU wouldn't make sense.

		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			// Falling-edge trigger: set FTSR, and make sure RTSR is clear
			// so a stale rising-edge trigger from a previous config doesn't also fire
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			// Rising-edge trigger: set RTSR, clear any leftover FTSR bit
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// Both-edge trigger: set both FTSR and RTSR for this pin
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		// Step 2: tell SYSCFG_EXTICR which GPIO port owns this EXTI line.
		// Each EXTICR register holds 4 pins' port-selector nibbles, so
		// temp1 picks the register index and temp2 picks the nibble within it.
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4;
		uint8_t temp2 =  pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASE_ADDRESS_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << ( temp2*4);

		// Step 3: unmask this EXTI line in IMR so the interrupt request is
		// actually allowed to propagate onward to the NVIC
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	temp = 0;

	//2. Configure output speed (OSPEEDR): same 2-bit-per-pin layout as MODER
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); // clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp; //setting

	temp = 0;

	//3. Configure pull-up/pull-down (PUPDR): same 2-bit-per-pin layout as MODER
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); // clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp; //setting

	//4. Configure output type (OTYPER): push-pull vs open-drain, only 1 bit per pin
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	//5. Configure alternate function (AFR): only relevant when mode is ALTFN.
	// AFR is split into two 32-bit registers, each holding 8 pins' 4-bit AF codes:
	// AFR[0] covers pins 0-7, AFR[1] covers pins 8-15.
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		uint8_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}
}

/**
 * @brief  Resets all registers of the target GPIO port back to their default state.
 * @note   Leverages the RCC AHB1 Reset Register: setting a port's reset bit
 *         and then immediately clearing it forces every register of that
 *         port back to its power-on-reset value, without a full chip reset.
 * @param  pGPIOx : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @retval None
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	}
	else if(pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	}
	else if(pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	}
	else if(pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	}
	else if(pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	}
	else if(pGPIOx == GPIOF)
	{
		GPIOF_REG_RESET();
	}
	else if(pGPIOx == GPIOG)
	{
		GPIOG_REG_RESET();
	}
	else if(pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();
	}
	else if(pGPIOx == GPIOI)
	{
		GPIOI_REG_RESET();
	}
}

/**
 * @brief  Reads the current logic level of a single GPIO input pin.
 * @note   Shifts IDR right by PinNumber so the bit of interest lands at
 *         position 0, then masks with 0x1 to isolate just that one bit.
 * @param  pGPIOx    : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  PinNumber : Pin number to read (0-15)
 * @retval uint8_t   : 0 (GPIO_PIN_RESET) or 1 (GPIO_PIN_SET)
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)(pGPIOx->IDR >> PinNumber & (0x00000001));
	return value;
}

/**
 * @brief  Reads the raw contents of the GPIO port's input data register.
 * @note   IDR holds the live logic level of all 16 pins on this port at once,
 *         one bit per pin. PinNumber isn't needed here since the whole port
 *         is returned in a single read; it's kept in the signature purely
 *         for symmetry with GPIO_ReadFromInputPin, so it's marked unused.
 * @param  pGPIOx    : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  PinNumber : Unused - present only for API symmetry
 * @retval uint8_t   : Lower 8 bits of IDR
 */
uint8_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	(void)PinNumber;
	value = (uint8_t)pGPIOx->IDR;
	return value;
}

/**
 * @brief  Drives a single GPIO output pin high or low.
 * @param  pGPIOx    : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  PinNumber : Pin number to drive (0-15)
 * @param  Value     : GPIO_PIN_SET to drive high, GPIO_PIN_RESET to drive low
 * @retval None
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
	pGPIOx->ODR |= (1 << PinNumber);
	}
	else
	{
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/**
 * @brief  Writes a full 16-bit value to the GPIO port's output data register.
 * @note   Overwrites all 16 pins of the port in one shot - any pins not
 *         intended to change must have their previous bit value included
 *         in Value by the caller.
 * @param  pGPIOx : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  Value  : 16-bit value to drive onto the port (bit N = pin N)
 * @retval None
 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)//uint16_t because there are 16 pins in a port
{
	pGPIOx->ODR = Value;

}

/**
 * @brief  Toggles (inverts) the current output state of a single GPIO pin.
 * @param  pGPIOx    : Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param  PinNumber : Pin number to toggle (0-15)
 * @retval None
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}

/**
 * @brief  Enables or disables delivery of a given IRQ line at the NVIC level.
 * @note   Once EXTI raises an interrupt request, it still has to pass through
 *         the Nested Vectored Interrupt Controller (NVIC) before the CPU will
 *         actually jump to the ISR. The NVIC groups its 240 possible IRQ
 *         lines into banks of 32, each bank controlled by its own ISER
 *         (enable) / ICER (disable) register - hence the IRQNumber ranges
 *         below, and the "% 32" to find the bit position within that bank.
 * @param  IRQNumber : Hardware IRQ line number assigned by the vendor (see
 *                     IRQ_NO_* macros in stm32f7xx.h)
 * @param  EnorDi    : ENABLE or DISABLE
 * @retval None
 */
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            // Bank 0 covers IRQ numbers 0-31
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            // Bank 1 covers IRQ numbers 32-63
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            // Bank 2 covers IRQ numbers 64-95
            *NVIC_ISER2 |= (1 << (IRQNumber % 32));
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            *NVIC_ICER2 |= (1 << (IRQNumber % 32));
        }
    }
}

/**
 * @brief  Sets the priority level for a specific hardware IRQ line inside the NVIC.
 * @note   The NVIC packs 4 IRQ priorities into each 32-bit IPR register, one
 *         byte per IRQ, but only the top NO_PR_BITS_IMPLEMENTED bits of each
 *         byte are actually implemented in silicon (the rest are don't-care
 *         bits). So the priority value is shifted left to sit at the top of
 *         its byte before being placed into the correct byte-sized section.
 * @param  IRQNumber   : Hardware IRQ line number
 * @param  IRQPriority : Priority value (0 = highest priority)
 * @retval None
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint32_t iprx = IRQNumber / 4;
    uint32_t iprx_section = IRQNumber % 4;

    // 1. Shift the priority to the top of its 8-bit block (e.g., with 4 bits
    //    implemented, shift left by 4 so the value occupies the upper nibble)
    uint8_t aligned_priority = (IRQPriority << (8 - NO_PR_BITS_IMPLEMENTED));

    // 2. Shift that aligned 8-bit block to the correct byte section (0, 8, 16, or 24)
    uint8_t shift_amount = (8 * iprx_section);

    // 3. Clear the old priority bits first to avoid corrupting neighbouring
    //    IRQ priorities packed into the same 32-bit register, then set the new ones
    *(NVIC_PR_BASE_ADDRESS + iprx) &= ~(0xFF << shift_amount);
    *(NVIC_PR_BASE_ADDRESS + iprx) |= (aligned_priority << shift_amount);
}

/**
 * @brief  Generic EXTI pending-flag handler, called from the application's
 *         actual IRQHandler (e.g. EXTI0_IRQHandler) once an interrupt fires.
 * @note   EXTI_PR is a "write 1 to clear" register: writing a 1 to a pending
 *         bit clears it, writing 0 has no effect. So ORing in a 1 here is
 *         the correct way to acknowledge and clear this specific pin's flag,
 *         not a mistake despite looking like it should "set" something.
 * @param  PinNumber : Pin number whose pending interrupt flag should be cleared
 * @retval None
 */
void GPIO_IRQHandler(uint8_t PinNumber)
{
	if(EXTI->PR & (1 << PinNumber))
	{
		// Acknowledge and clear this pin's pending bit
		EXTI->PR |= (1 << PinNumber);
	}
}







