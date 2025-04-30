/*
 * tim.c
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#include "tim.h"

#define TIM2EN (1U << 0)
#define CR1_CEN (1U << 0)


void tim2_1hz_init(void) {

	RCC->APB1ENR |= TIM2EN;
	//set prescaling
	TIM2->PSC = 1600 - 1;
	//set auto reload
	TIM2->ARR = 10000 - 1;
	//clear counter
	TIM2->CNT = 0;

	TIM2->DIER |= TIM_DIER_UIE;

	// 4) NVIC: set priority and enable IRQ
	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_EnableIRQ(TIM2_IRQn);

	// 5) Start the timer (CEN bit in CR1)
	TIM2->CR1 |= TIM_CR1_CEN;
	// enable timer
	TIM2->CR1 = CR1_CEN;

}

void tim2_172Hz_init(void) {
	RCC->APB1ENR |= TIM2EN;
		//set prescaling
		TIM2->PSC = 16;
		//set auto reload
		TIM2->ARR = 5620;
		//clear counter
		TIM2->CNT = 0;

		TIM2->DIER |= TIM_DIER_UIE;

		// 4) NVIC: set priority and enable IRQ
		NVIC_SetPriority(TIM2_IRQn, 1);
		NVIC_EnableIRQ(TIM2_IRQn);

		// 5) Start the timer (CEN bit in CR1)
		TIM2->CR1 |= TIM_CR1_CEN;
		// enable timer
		TIM2->CR1 = CR1_CEN;
}


void tim2_10Hz_init(void) {
	RCC->APB1ENR |= TIM2EN;
		//set prescaling
		TIM2->PSC = 16000 - 1;
		//set auto reload
		TIM2->ARR = 10000 - 1;
		//clear counter
		TIM2->CNT = 0;

		TIM2->DIER |= TIM_DIER_UIE;

		// 4) NVIC: set priority and enable IRQ
		NVIC_SetPriority(TIM2_IRQn, 1);
		NVIC_EnableIRQ(TIM2_IRQn);

		// 5) Start the timer (CEN bit in CR1)
		TIM2->CR1 |= TIM_CR1_CEN;
		// enable timer
		TIM2->CR1 = CR1_CEN;
}
