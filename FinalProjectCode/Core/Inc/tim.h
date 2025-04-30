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
void tim2_10Hz_init(void);
void tim2_172Hz_init(void);
void tim2_pwm80_pa5_init(void);
void tim2_set_duty_percent(uint8_t pct);
#endif


#endif /* TIM_H_ */
