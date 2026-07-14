/*
 * stm32f7xx_gpio_driver.h
 *
 *  Created on: 08-Jul-2026
 *      Author: prathameshs
 */

#ifndef STM32F7XX_GPIO_DRIVER_H_
#define STM32F7XX_GPIO_DRIVER_H_

#include "stm32f7xx.h"

typedef struct {
    uint8_t GPIO_PinNumber;           // e.g., Pin 0 to 15
    uint8_t GPIO_PinMode;             // e.g., Input, Output, Alt Function, Analog
    uint8_t GPIO_PinSpeed;            // e.g., Low, Medium, High, Very High
    uint8_t GPIO_PinOPType;           // e.g., Push-Pull or Open-Drain
    uint8_t GPIO_PinPuPdControl;      // e.g., No Pull, Pull-up, Pull-down
    uint8_t GPIO_PinAltFunMode;       // e.g., AF0 to AF15
} GPIO_PinConfig_t;

typedef struct {
    GPIO_RegDef_t    *pGPIOx;          // Holds the physical base address (e.g., GPIOA, GPIOB)
    GPIO_PinConfig_t GPIO_PinConfig;   // Holds the user's custom settings for that pin
} GPIO_Handle_t;

//GPIO Pin Numbers

#define GPIO_PIN_NO_0		0
#define GPIO_PIN_NO_1		1
#define GPIO_PIN_NO_2		2
#define GPIO_PIN_NO_3		3
#define GPIO_PIN_NO_4		4
#define GPIO_PIN_NO_5		5
#define GPIO_PIN_NO_6		6
#define GPIO_PIN_NO_7		7
#define GPIO_PIN_NO_8		8
#define GPIO_PIN_NO_9		9
#define GPIO_PIN_NO_10		10
#define GPIO_PIN_NO_11		11
#define GPIO_PIN_NO_12		12
#define GPIO_PIN_NO_13		13
#define GPIO_PIN_NO_14		14
#define GPIO_PIN_NO_15		15

//GPIO modes
//1. Non interrupt modes
#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT 		1
#define GPIO_MODE_ALTFN 	2
#define GPIO_MODE_ANALOG 	3
//2. Interrupt modes
#define GPIO_MODE_IT_FT		4 //Input falling edge
#define GPIO_MODE_IT_RT		5 //Rising edge
#define GPIO_MODE_IT_RFT	6 //Rising edge falling edge

//GPIO possible output types
#define GPIO_OP_TYPE_PP //push-pull
#define GPIO_OP_TYPE_OD //open drain

//GPIO possible speeds
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_HIGH			2
#define GPIO_SPEED_VERY_HIGH	3

//GPIO pull-up and pull-down configuration macro
#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2

// ******************APIs Supported by this driver***************************

//peripheral clock control
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

//Init and de-init
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

//data read and write
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value); //uint16_t because there are 16 pins in a port
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

//IRQ configuration and ISR handling
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQHandler(uint8_t PinNumber);

#endif /* STM32F7XX_GPIO_DRIVER_H_ */
