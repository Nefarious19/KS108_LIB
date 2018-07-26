/*
 * ks108.c
 *
 *  Created on: 20.06.2018
 *      Author: rafal
 */

#include "stm32f0xx.h"

#include "../inc/ks108.h"
#include "../inc/ks108_utilities.h"
#include "../inc/ks108_gfx.h"

#include "../../DELAY/delay.h"


static inline void KS108_DATA_OUT(void)
{
	DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFF0000) | 0x00005555;
}
static inline void KS108_DATA_IN(void)
{
	DATA_PORT->MODER  &=  0xFFFF0000;
}

#if USE_READ_FROM_PORT_FEATURES == 1

	static uint8_t KS108_ReadDsiplay ( CS_number CS_nr );

#endif



static inline void KS108_GPIO_INIT (void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	DATA_PORT->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0 |
						  GPIO_OSPEEDER_OSPEEDR1 |
						  GPIO_OSPEEDER_OSPEEDR2 |
						  GPIO_OSPEEDER_OSPEEDR3 |
						  GPIO_OSPEEDER_OSPEEDR4 |
						  GPIO_OSPEEDER_OSPEEDR5 |
						  GPIO_OSPEEDER_OSPEEDR6 |
						  GPIO_OSPEEDER_OSPEEDR7;

	KS108_DATA_OUT();

	CTRL_PORT->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 |
						  GPIO_OSPEEDER_OSPEEDR9 |
			              GPIO_OSPEEDER_OSPEEDR10 |
						  GPIO_OSPEEDER_OSPEEDR11 |
						  GPIO_OSPEEDER_OSPEEDR12 |
						  GPIO_OSPEEDER_OSPEEDR13;

	CTRL_PORT->ODR |=   SBBVAL(0x01,CTRL_CS1,1) |
					  	SBBVAL(0x01,CTRL_CS2,1) |
						SBBVAL(0x01, CTRL_DI, 1)|
						SBBVAL(0x01, CTRL_RST,1) |
						SBBVAL(0x01, CTRL_E,1) |
						SBBVAL(0x01, CTRL_RW, 1);


	CTRL_PORT->MODER |=   SBBVAL(0x01,CTRL_CS1,2)  |
			  	  	  	  SBBVAL(0x01,CTRL_CS2,2)  |
						  SBBVAL(0x01,CTRL_E,2)    |
						  SBBVAL(0x01, CTRL_DI, 2) |
						  SBBVAL(0x01, CTRL_RST,2) |
						  SBBVAL(0x01, CTRL_RW, 2);

	KS108_CLR_RST;
	_delay_ms(50);
	KS108_SET_RST;
	_delay_ms(50);

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	BACKL_PORT->OSPEEDR |= SBBVAL(0x03, BACKL_GPIO, 2);
	BACKL_PORT->MODER   |= SBBVAL(0x01, BACKL_GPIO, 2);
	KS108_Backlight_ON();
}




void KS108_WriteInstruction (uint8_t instruction, CS_number CS_nr )
{
	KS108_RW_LO;
	KS108_DI_LO;
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO; break;
		case CS2: KS108_CS2_LO; break;
	}
	KS108_DATA_OUT();
	_delay_us(1);
	DATA_PORT->ODR = (uint32_t)((DATA_PORT->ODR & 0x0000FF00) | instruction);
	_delay_us(1);
	KS108_SET_E;
	_delay_us(1);
	KS108_CLR_E;
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI; break;
		case CS2: KS108_CS2_HI; break;
	}
}

void KS108_WriteDsiplay (uint8_t data, CS_number CS_nr)
{
	KS108_RW_LO;
	KS108_DI_HI;
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO; break;
		case CS2: KS108_CS2_LO; break;
	}
	KS108_DATA_OUT();
	_delay_us(1);
	DATA_PORT->ODR = (uint32_t)((DATA_PORT->ODR & 0x0000FF00) | data);
	_delay_us(1);
	KS108_SET_E;
	_delay_us(1);
	KS108_CLR_E;
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI; break;
		case CS2: KS108_CS2_HI; break;
	}
}


#if USE_READ_FROM_PORT_FEATURES == 1

static uint8_t KS108_ReadDsiplay ( CS_number CS_nr )
{
	uint8_t data = 0;
	KS108_RW_HI;
	KS108_DI_HI;
	if (CS_nr == CS1) KS108_CS1_LO;
	else KS108_CS2_LO;
	KS108_DATA_IN();
	_delay_us(1);
	KS108_SET_E;
	_delay_us(3);
	data = (uint8_t)(DATA_PORT->IDR & 0x000000FF);
	KS108_CLR_E;
	_delay_us(1);
	if (CS_nr == CS1) KS108_CS1_HI;
	else KS108_CS2_HI;
	_delay_us(1);
	return data;
}

#endif





void KS108_InitDisplay (void)
{
	_delay_init(48000000UL);
	KS108_GPIO_INIT();
	_delay_ms(100);
	KS108_WriteInstruction(KS108_DISPLAY_OFF ,CS1);
	KS108_WriteInstruction(KS108_SET_X_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Z_ADR(0),CS1);
	KS108_WriteInstruction(KS108_DISPLAY_ON ,CS1);

	KS108_WriteInstruction(KS108_DISPLAY_OFF ,CS2);
	KS108_WriteInstruction(KS108_SET_X_ADR(0),CS2);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS2);
	KS108_WriteInstruction(KS108_SET_Z_ADR(0),CS2);
	KS108_WriteInstruction(KS108_DISPLAY_ON ,CS2);

	KS108_fillDisplay(0x00);
}
