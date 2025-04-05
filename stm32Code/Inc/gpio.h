/*
 * gpio.h
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <stdbool.h>

void led_init(void);
void led_on(void);
void led_off(void);
bool get_button_state(void);
void button_init(void);

#endif /* GPIO_H_ */
