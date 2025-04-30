/*
 * systick.h
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>
#include <stm32f446xx.h>
//#include "stm32f4xx_it.h"

//void SysTick_Handler(void);
void SysTick_Init(void);


//void SPI_Timer_On(uint32_t waitTicks);
void setSysClockTo160MHz(void);

//uint8_t SPI_Timer_Status(void);
void delay_ms(uint32_t delay);
void delay_us(uint32_t delay);
uint32_t millis(void);

extern volatile uint32_t sysTickCounter;
extern uint32_t spiTimerTickStart;   // Declaration
extern uint32_t spiTimerTickDelay;

#endif /* SYSTICK_H_ */
