/*
 * stm32f7xx_gpio_driver.c
 *
 *  Created on: 08-Jul-2026
 *      Author: prathameshs
 */


#include "stm32f7xx_gpio_driver.h"

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
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

/*
 * for GPIO initialization, basically this function sets up the GPIO by applying the user specified
 * mode (IN/OUT/ANALOG/ALT FNC), speed(LOW/MEDIUM/HIGH/VERY HIGH), PUPD, PP/OD etc;
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; // temp = temporary

	/* Why GPIO modes are divided into non interrupt and interrupt modes?
	 *
	 * INPUT, OUTPUT, ANALOG, and ALTERNATE FUNCTION only define how the GPIO pin
	 * behaves (read, drive, analog, or peripheral function). They do not notify
	 * the CPU when the pin changes state. Thus non interrupt.
	 *
	 * Rising/Falling edge modes additionally connect the pin to the EXTI controller,
	 * which detects voltage transitions and generates an interrupt, allowing the
	 * CPU to execute the corresponding ISR automatically. Thus interrupt
	 */

	//1. Configure GPIO pin mode
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_ANALOG_MODE)
	{
		//Non interrupt mode
		/*
		 * So here we take the user defined value and shift it by twice the pin number is (e.g, if its 4th pin then value
		 * shifts by 8 positions, because the mode register of GPIO has two bits per pin as there are 4 possible modes.
		 */
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		/*
		 * clearing those specific bits to 00, as we will do an OR operation with "tempt",
		 * and as OR operation has no power to manipulate and flit a 1 bit to 0, it can only either keep 0 as 0 or change 0 to 1,
		 * so we need to make sure the bits are 00, only then we can manipulate both bits using OR.
  	     */
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
		pGPIOHandle->pGPIOx->MODER |= temp; //setting
	}
	else
	{
		//interrupt mode
		//GPIO Pin Interrupt configuration
		//EXTI0 is meant for PinNumber 0 of any port we select (PA0, PB0, PC0 etc) and such like total 16 EXTI lines as total 16 pins per port.
		//Once a specific Port's Pin is selected and routed to EXTI line, no other same pin number of any other port can generate interrupt simultaneously.
		//1. Pin must be in input mode (as the whole thing is about receiving an interrupt
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			//configure FTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clear corresponding RTSR bit to avoid potential conflicts
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//configure RTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clear corresponding FTSR bit to avoid potential conflicts
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			//configure both FTSR and RTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		//2. Configure the GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4;
		uint8_t temp2 =  pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASE_ADDRESS_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << ( temp2*4);

		//3. Enable the EXTI interrupt delivery using IMR
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	temp = 0;

	//2. Configure speed of GPIO
	/*
	 * Consider PinNUmber is 8, by doing pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed we access the value stored
	 *  in GPIO_PinSpeed by the user e.g, if value = 3 then (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed)=3,
	 *  which eventually means (3 << (2 * 8)) means 3 << 16 (shift the binary value 11 by 16 positions).
	 */
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp; //setting

	temp = 0;

	//3. Configure PUPD  settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp; //setting

	//4. Configure OP TYPE
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	//5. ALT FUNC
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIOGPIO_MODE_ALTFN)
	{
		uint8_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}
}

/*
 * To de-initialize GPIO Port we have to toggle the RCC AHB1 Reset register to 1 and then back to 0
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

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)(pGPIOx->IDR >> PinNumber & (0x00000001)); //shifting the number of times Pin number is to bring the desired pin number value at the 0th position and then do AND operation to mask it and get only that value
	return value;
}

uint8_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)pGPIOx->IDR; //shifting the number of times Pin number is to bring the desired pin number value at the 0th position and then do AND operation to mask it and get only that value
	return value;
}

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

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)//uint16_t because there are 16 pins in a port
{
	pGPIOx->ODR = Value;

}

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}

/*
 * After the EXTI controller generates an Interrupt Request (IRQ), it is
 * forwarded to the Nested Vectored Interrupt Controller (NVIC). This function
 * configures the NVIC by enabling/disabling the corresponding IRQ using the
 * ISER/ICER registers and assigning its priority. Once enabled, the NVIC can
 * forward the IRQ to the CPU, allowing execution of the corresponding ISR.
 */
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDie == ENABLE)
	{
		if(IRQ_Number <= 31)
		{
			//program ISER0 register
			*ISER0 |= (1 << IRQ_Number);
		}
		else if(IRQ_Number > 31 && IQR_Number < 64)
		{
			//program ISER1 register
			*ISER1 |= (1 << IRQ_Number % 32);
		}
		else if(IQR_Number >= 64 && IQR_Number <= 96)
		{
			//program ISER2 register
			*ISER2 |= (1 << IRQ_Number % 64);
		}
	}
	else
	{
		if(IRQ_Number <= 31)
		{
			//program ICER0 register
			*ICER0 |= (1 << IRQ_Number);
		}
		else if(IRQ_Number > 31 && IQR_Number < 64)
		{
			//program ICER1 register
			*ICER1 |= (1 << IRQ_Number % 32);
		}
		else if(IQR_Number >= 64 && IQR_Number <= 96)
		{
			//program ICER2 register
			*ICER2 |= (1 << IRQ_Number % 64);
		}
	}
}

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//Find out the ipr register based on IRQNumber
	uint32_t iprx = IRQNumber /4;
	uint32_t iprx_section = IRQNumber %4;
	uint8_t shift_amount = (8 * iprx_section) - (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDRESS +iprx ) |= (IRQPriority << shift_amount);
}


void GPIO_IRQHandler(uint8_t PinNumber)
{
	//clear the exti pr register  corresponding to the Pin number
	if(EXTI->PR & (1 << PinNumber))//if PIR is 1 then AND with 1 gives 1 which makes the statement true and the following executes
	{
		//clear
		EXTI->PR |= (1 << PinNumber);
	}
}








