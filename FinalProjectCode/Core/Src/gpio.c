/*
 * gpio.c
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */

#include <stm32f446xx.h>
#include "gpio.h"


#define GPIOAEN (1U << 0)
#define GPIOBEN (1U << 1)
#define GPIOCEN (1U << 2)
#define GPIODEN (1U << 3)
#define GPIOEEN (1U << 4)
#define GPIOFEN (1U << 5)
#define GPIOGEN (1U << 6)
#define GPIOHEN (1U << 7)
#define BTN_PIN (1U << 13)

#define LED_BS5 (1U << 5)
#define LED_BR5 (1U << 21)


void led_init() {
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
}

void led_on(void) {
	//turn on the led
	GPIOA->BSRR = LED_BS5;
}

void led_off(void) {
	//turn of the led
	GPIOA->BSRR = LED_BR5;
}

void led_toggle(void) {
	GPIOA->ODR ^= (1U << 5);
}

void button_init(void) {
	// enable clock access
	RCC->AHB1ENR |= GPIOCEN;
	//Set PC13 as an input
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);

}

bool get_button_state(void) {

	if (GPIOC->IDR & BTN_PIN) {
		return false;
	} else {
		return true;
	}
}
