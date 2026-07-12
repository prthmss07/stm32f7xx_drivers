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
		elseif(pGPIOx == GPIOB);
		{
			GPIOB_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOC);
		{
			GPIOC_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOD);
		{
			GPIOD_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOE);
		{
			GPIOE_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOF);
		{
			GPIOF_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOG);
		{
			GPIOG_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOH);
		{
			GPIOH_PERI_CLK_EN();
		}
		elseif(pGPIOx == GPIOI);
		{
			GPIOI_PERI_CLK_EN();
		}
	}
	elseif(EnorDi == DISABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOB)
		{
			GPIOB_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOC)
		{
			GPIOC_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOD)
		{
			GPIOD_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOE)
		{
			GPIOE_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOF)
		{
			GPIOF_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOG)
		{
			GPIOG_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOH)
		{
			GPIOH_PERI_CLK_DI();
		}
		elseif(pGPIOx == GPIOI)
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
	elseif(pGPIOx == GPIOB);
	{
		GPIOB_REG_RESET();
	}
	elseif(pGPIOx == GPIOC);
	{
		GPIOC_REG_RESET();
	}
	elseif(pGPIOx == GPIOD);
	{
		GPIOD_REG_RESET();
	}
	elseif(pGPIOx == GPIOE);
	{
		GPIOE_REG_RESET();
	}
	elseif(pGPIOx == GPIOF);
	{
		GPIOF_REG_RESET();
	}
	elseif(pGPIOx == GPIOG);
	{
		GPIOG_REG_RESET();
	}
	elseif(pGPIOx == GPIOH);
	{
		GPIOH_REG_RESET();
	}
	elseif(pGPIOx == GPIOI);
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












