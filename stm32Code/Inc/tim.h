/*
 * tim.h
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#ifndef TIM_H_
#define TIM_H_

/*TIMER REGISTERS TIMX_CNT, TIMX_PSC, TIMX_ARR, TIMX_CR1, TIMX_CR2,SR*/

#include <stm32f446xx.h>
#ifndef TIM_H_
#define TIM_H_
#define SR_UIF  (1U<<0)
void tim2_1hz_init(void);
#endif


#endif /* TIM_H_ */
