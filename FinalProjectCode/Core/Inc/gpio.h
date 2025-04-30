/*
 * gpio.h
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <stdbool.h>

//#define GPIOAEN(GPIOX) (RCC-AHB1ENR |= GPIOx)
//#define OUTPIN(GPIOX, PINx)  (GPIOX->MODER |= (1U << PINX) GPIOx-<MOD)

void led_init(void);
void led_on(void);
void led_off(void);
bool get_button_state(void);
void button_init(void);
void led_toggle(void);
#endif /* GPIO_H_ */
