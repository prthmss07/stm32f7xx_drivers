/*
 * stm32f7xx.h
 *
 *  Created on: 03-Jul-2026
 *      Author: prathameshs
 */

#ifndef STM32F7XX_H_
#define STM32F7XX_H_

#include<stdint.h>

/* ARM Cortx-M7 NVIC ISER Register Addresses*/

#define NVIC_ISER0 ((volatile uint32_t*)0xE000E100)
#define NVIC_ISER1 ((volatile uint32_t*)0xE000E104)
#define NVIC_ISER2 ((volatile uint32_t*)0xE000E108)
#define NVIC_ISER3 ((volatile uint32_t*)0xE000E10C)

/*ARM Cortex-M7 NVIC ISER Register Addresses*/

#define NVIC_ICER0 ((volatile uint32_t*)0XE000E180)
#define NVIC_ICER1 ((volatile uint32_t*)0XE000E184)
#define NVIC_ICER2 ((volatile uint32_t*)0XE000E188)
#define NVIC_ICER3 ((volatile uint32_t*)0XE000E18C)

/*NVIC Priority Register Base Address*/
#define NVIC_PR_BASE_ADDRESS ((volatile uint32_t*)0XE000E400)

#define NO_PR_BITS_IMPLEMENTED		4
//refer stm32f7xx reference manual for these addresses
/*We target the following peripherals :
 *GPIOA, GPIOB, ... GPIOI,
 *SPI1, SPI2, SPI3, I2C1, I2C2, I2C3, EXTI, SYSCFG, USART 1, 2, 3, 6, UART 4, 5 */

//Base addresses of Flash and SRAM
#define FLASH_BASE_ADDRESS		0x08000000U
#define SRAM1_BASE_ADDRESS		0x20010000U
#define SRAM2_BASE_ADDRESS		0x2003C000U
#define ROM_BASE_ADDRESS		0x1FF00000U
#define SRAM					SRAM1_BASE_ADDRESS

//AHBx and APBx peripheral base addresses
#define PERIPHERAL_BASE				0x40000000U
#define APB1_PERIPHERAL_BASE		PERIPHERAL_BASE
#define APB2_PERIPHERAL_BASE		0x40010000U
#define AHB1_PERIPHERAL_BASE		0x40020000U
#define AHB2_PERIPHERAL_BASE		0x50000000U

//Base address of each peripherals
//First write the base addresses of peripherals hanging on AHB1 bus
#define GPIOA_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x0000)
#define GPIOB_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x0400)
#define GPIOC_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x0800)
#define GPIOD_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x0C00)
#define GPIOE_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x1000)
#define GPIOF_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x1400)
#define GPIOG_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x1800)
#define GPIOH_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x1C00)
#define GPIOI_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x2000)

#define RCC_BASE_ADDRESS		(AHB1_PERIPHERAL_BASE + 0x3800)

/*As we will be building drivers only for GPIP, I2C, USART, SPI, we will only include those peripheral addresses
 *AHB1 had only GPIO peripherals of our interest.
 *Now we move on to APB1 bus and the peripherals of our interest hanging on APB1
 */

#define I2C1_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x5400)
#define I2C2_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x5800)
#define I2C3_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x5C00)

#define SPI2_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x3800)
#define SPI3_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x3C00)

#define USART2_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x4400)
#define USART3_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x4800)
#define UART4_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x4C00)
#define UART5_BASE_ADDRESS		(APB1_PERIPHERAL_BASE + 0x5000)

/*Base address of peripherals hanging on APB2 bus */

#define EXTI_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x3C00)
#define SYSCFG_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0X3800)

#define USART1_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x1000)
#define USART6_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x1400)

#define SPI1_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x3000)
#define SPI4_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x3400)
#define SPI5_BASE_ADDRESS		(APB2_PERIPHERAL_BASE + 0x5000)

/***************************peripheral registers definition structure***********************/

/* first starting with all the registers used by the GPIO peripheral */
typedef struct
{
	volatile uint32_t MODER;		//Mode register used to select mode (i/p(00), o/p(01), alternate function (10) analog (11)
	volatile uint32_t OTYPER;		//Output type register, push-pull, open-drain modes
	volatile uint32_t OSPEEDR;		//output speed, low speed, high speed, very high speed etc
	volatile uint32_t PUPDR;		//pull-up or pull-down resistor activation register
	volatile uint32_t IDR;		//input data register (read only), used to read incoming data
	volatile uint32_t ODR;		//output data register used to read/write output which our micro-controller wants to send to whatever external device connected
	volatile uint32_t BSRR;		//
	volatile uint32_t LCKR;
	volatile uint32_t AFR[2];
}GPIO_RegDef_t;

typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t CRCPR;
	volatile uint32_t RXCRCR;
	volatile uint32_t TXCRCR;
	volatile uint32_t I2SCFGR;
	volatile uint32_t I2SPR;

}SPI_RegDef_t;

//RCC peripheral register structure
typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t Reserved0;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t Reserved1[2];
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t Reserved2;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t Reserved3[2];
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB3LPENR;
	volatile uint32_t Reserved4;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t Reserved5[2];
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t PLLSAICFGR;
	volatile uint32_t DCKCFGR1;
	volatile uint32_t DCKCFGR2;

}RCC_RegDef_t;

//EXTI Register structure
typedef struct
{
    volatile uint32_t IMR;    /*!< Interrupt Mask Register - Enables/disables interrupt requests for EXTI lines */
    volatile uint32_t EMR;    /*!< Event Mask Register - Enables/disables event generation for EXTI lines */
    volatile uint32_t RTSR;   /*!< Rising Trigger Selection Register - Selects rising-edge detection */
    volatile uint32_t FTSR;   /*!< Falling Trigger Selection Register - Selects falling-edge detection */
    volatile uint32_t SWIER;  /*!< Software Interrupt Event Register - Generates interrupt/event by software */
    volatile uint32_t PR;     /*!< Pending Register - Indicates and clears pending interrupt flags */
} EXTI_RegDef_t;

typedef struct
{
	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t EXTICR[4];
    uint32_t RESERVED2[2];
	volatile uint32_t CMPCR;
	uint32_t RESERVED1[2];
	volatile uint32_t CFGR;

}SYSCFG_RegDef_t;

/******** Peripheral base addresses type-casted to xxx_RegDef_t *************/
#define GPIOA ((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB ((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC ((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)
#define GPIOD ((GPIO_RegDef_t*)GPIOD_BASE_ADDRESS)
#define GPIOE ((GPIO_RegDef_t*)GPIOE_BASE_ADDRESS)
#define GPIOF ((GPIO_RegDef_t*)GPIOF_BASE_ADDRESS)
#define GPIOG ((GPIO_RegDef_t*)GPIOG_BASE_ADDRESS)
#define GPIOH ((GPIO_RegDef_t*)GPIOH_BASE_ADDRESS)
#define GPIOI ((GPIO_RegDef_t*)GPIOI_BASE_ADDRESS)

#define RCC ((RCC_RegDef_t*)RCC_BASE_ADDRESS)
#define SYSCFG ((SYSCFG_RegDef_t*)SYSCFG_BASE_ADDRESS)
#define EXTI ((EXTI_RegDef_t*)EXTI_BASE_ADDRESS)

//Clock enable Macro for GPIOx peripheral
#define GPIOA_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 0)
#define GPIOB_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 1)
#define GPIOC_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 2)
#define GPIOD_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 3)
#define GPIOE_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 4)
#define GPIOF_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 5)
#define GPIOG_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 6)
#define GPIOH_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 7)
#define GPIOI_PERI_CLK_EN()		RCC->AHB1ENR |= (1 << 8)

//Clock enable macro for IC2x peripheral
#define I2C1_PERI_CLK_EN()		RCC->APB1ENR |= (1 << 21)
#define I2C2_PERI_CLK_EN()		RCC->APB1ENR |= (1 << 22)
#define I2C3_PERI_CLK_EN()		RCC->APB1ENR |= (1 << 23)

//Clock enable macro for SPIx peripheral
#define SPI1_PCLK_EN()		RCC->APB2ENR |= (1 << 12)
#define SPI2_PCLK_EN()		RCC->APB1ENR |= (1 << 14)
#define SPI3_PCLK_EN()		RCC->APB1ENR |= (1 << 15)
#define SPI4_PCLK_EN()		RCC->APB2ENR |= (1 << 13)


//Clock enable macro for USARTx peripheral
#define USART1_PCLK_EN()		RCC->APB2ENR |= (1 << 4)
#define USART2_PCLK_EN()		RCC->APB1ENR |= (1 << 17)
#define USART3_PCLK_EN()		RCC->APB1ENR |= (1 << 18)
#define UART4_PCLK_EN()			RCC->APB1ENR |= (1 << 19)
#define UART5_PCLK_EN()			RCC->APB1ENR |= (1 << 20)
#define USART6_PCLK_EN()		RCC->APB2ENR |= (1 << 5)

//Clock enable macro for SYSCFG peripheral
#define SYSCFG_PCLK_EN()		RCC->APB2ENR |= (1 << 14)

//Clock disabling macro for GPIOx peripheral
#define GPIOA_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 0)
#define GPIOB_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 1)
#define GPIOC_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 2)
#define GPIOD_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 3)
#define GPIOE_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 4)
#define GPIOF_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 5)
#define GPIOG_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 6)
#define GPIOH_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 7)
#define GPIOI_PERI_CLK_DI()		RCC->AHB1ENR &= ~(1 << 8)

//Clock disable macro for I2Cx peripheral
#define I2C1_PCLK_DI()		RCC->APB1ENR &= ~(1 << 21)
#define I2C2_PCLK_DI()		RCC->APB1ENR &= ~(1 << 22)
#define I2C3_PCLK_DI()		RCC->APB1ENR &= ~(1 << 23)

//clock disable macro for SPIx peripheral
#define SPI1_PCLK_DI()		RCC->APB2ENR &= ~(1 << 12)
#define SPI2_PCLK_DI()		RCC->APB1ENR &= ~(1 << 14)
#define SPI3_PCLK_DI()		RCC->APB1ENR &= ~(1 << 15)
#define SPI4_PCLK_DI()		RCC->APB2ENR &= ~(1 << 13)

//clock disable macro for SYSCFG peripheral
#define SYSCFG_PCLK_DI()		RCC->APB2ENR &= ~(1 << 14)

//clock disable macro for USART peripheral

//GPIO Peripheral reset macro
#define GPIOA_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 0));	(RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 1));	(RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 2));	(RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 3));	(RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 4));	(RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOF_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 5));	(RCC->AHB1RSTR &= ~(1 << 5)); }while(0)
#define GPIOG_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 6));	(RCC->AHB1RSTR &= ~(1 << 6)); }while(0)
#define GPIOH_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 7));	(RCC->AHB1RSTR &= ~(1 << 7)); }while(0)
#define GPIOI_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 8));	(RCC->AHB1RSTR &= ~(1 << 8)); }while(0)

#define SPI1_REG_RESET()		do{ (RCC->APB2RSTR |= (1 << 12));	(RCC->APB2RSTR &= ~(1 << 12)); }while(0)
#define SPI2_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 14));	(RCC->APB1RSTR &= ~(1 << 14)); }while(0)
#define SPI3_REG_RESET()		do{ (RCC->APB1RSTR |= (1 << 15));	(RCC->APB1RSTR &= ~(1 << 15)); }while(0)
#define SPI4_REG_RESET()		do{ (RCC->AHB2RSTR |= (1 << 13));	(RCC->AHB1RSTR &= ~(1 << 13)); }while(0)


#define GPIO_BASE_ADDRESS_TO_CODE(x)    ((x == GPIOA)?0:\
                                        (x == GPIOB)?1:\
                                        (x == GPIOC)?2:\
                                        (x == GPIOB)?3:\
                                        (x == GPIOA)?4:\
                                        (x == GPIOB)?5:\
                                        (x == GPIOA)?6:\
                                        (x == GPIOB)?7:0 )
#define IQR_NO_EXTI0		6
#define IQR_NO_EXTI1		7
#define IQR_NO_EXTI2		8
#define IQR_NO_EXTI3		9
#define IQR_NO_EXTI4		10
#define IQR_NO_EXTI9_5		23
#define IQR_NO_EXTI15_10	40

#define SPI1	((SPI_RegDef_t*)SPI1_BASE_ADDRESS)
#define SPI2	((SPI_RegDef_t*)SPI2_BASE_ADDRESS)
#define SPI3	((SPI_RegDef_t*)SPI3_BASE_ADDRESS)
#define SPI4	((SPI_RegDef_t*)SPI4_BASE_ADDRESS)




//Some generic macros
#define ENABLE 1
#define DISABLE 0

#define SET ENABLE
#define RESET DISABLE

#define GPIO_PIN_SET SET
#define GPIO_PIN_RESET RESET



#include "stm32f7xx_gpio_driver.h"
#include "stm32f7xx_spi_driver.h"



#endif /* STM32F7XX_H_ */
