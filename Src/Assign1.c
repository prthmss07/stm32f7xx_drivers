/*
 * Assign1.c
 *
 *  Created on: 11-Jul-2026
 *      Author: prathameshs
 */

#include "stm32f7xx.h"
#include "stm32f7xx_gpio_driver.h"

int main(void){
	GPIO_Handle_t gpioled;

	gpioled.pGPIOx = GPIOD;
	gpioled.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	gpioled.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	gpioled.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	gpioled.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP
	gpioled.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;




	return 0;
}
