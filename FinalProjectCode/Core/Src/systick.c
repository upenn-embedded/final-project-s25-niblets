/*
 * systick.c
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#include "systick.h"
#include "main.h"
#define SYS_TICK_LOAD_VALUE 160000  // 1ms delay with 160 MHz clock
#define CTRL_ENABLE        (1U<<0)
#define CTRL_CLCKSRC    (1U<<2)
#define CTRL_COUNTFLAG    (1U<<16)
#define ONE_MSEC_LOAD     160000
#define ONE_MICRO_LOAD    160

volatile uint32_t sysTickCounter = 0;
volatile uint32_t delay_counter = 0;

// Initialize SysTick for 1ms tick
void SysTick_Init(void) {
    // Load the SysTick counter with the value for 1ms delay

    SysTick->LOAD = SYS_TICK_LOAD_VALUE - 1;  // Subtract 1 to make the count correct
    SysTick->VAL = 0;  // Clear the current value
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk |   // Enable SysTick interrupt
                    SysTick_CTRL_ENABLE_Msk |
					SysTick_CTRL_CLKSOURCE_Msk; // Enable SysTick timer
}


void delay_ms(uint32_t delay) {
		uint32_t ctrl_backup = SysTick->CTRL;
		uint32_t load_backup = SysTick->LOAD;

	   /*Load number of clock cycles per millisecond*/
	    SysTick->LOAD =  ONE_MSEC_LOAD - 1;
	    /*Clear systick current value register*/
	    SysTick->VAL = 0;
	    /*Select internal clock source*/
	    SysTick->CTRL = CTRL_CLCKSRC;
	    /*Enable systick*/
	    SysTick->CTRL |=CTRL_ENABLE;
	    for(int i = 0; i < delay; i++)
	    {
	        while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
	    }
	    SysTick->CTRL = ctrl_backup;
	   	SysTick->LOAD = load_backup;
}


//void delay_us(uint32_t delay) {
//		uint32_t ctrl_backup = SysTick->CTRL;
//		uint32_t load_backup = SysTick->LOAD;
//	   /*Load number of clock cycles per millisecond*/
//	    SysTick->LOAD =  ONE_MICRO_LOAD - 1;
//	    /*Clear systick current value register*/
//	    SysTick->VAL = 0;
//	    /*Select internal clock source*/
//	    SysTick->CTRL = CTRL_CLCKSRC;
//	    /*Enable systick*/
//	    SysTick->CTRL |=CTRL_ENABLE;
//	    for(int i = 0; i < delay; i++)
//	    {
//	        while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
//	    }
//	    SysTick->CTRL = ctrl_backup;
//	   	SysTick->LOAD = load_backup;
//	   	SysTick->VAL = 0;
//}

uint32_t millis(void) {
    return sysTickCounter;
}

void setSysClockTo160MHz(void) {
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos)  |
                   (160 << RCC_PLLCFGR_PLLN_Pos) |
                   (0 << RCC_PLLCFGR_PLLP_Pos)   |
                   (0 << RCC_PLLCFGR_PLLSRC_Pos);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 5. Set APB1 = 40 MHz (160/4), APB2 = 80 MHz (160/2)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV8;

    // 6. Select PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


