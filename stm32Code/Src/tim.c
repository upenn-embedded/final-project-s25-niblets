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
	// enable timer
	TIM2->CR1 = CR1_CEN;

}



