/*
 * uart.c
 *
 *  Created on: Mar 11, 2025
 *      Author: banele
 */
#include <stdint.h>
#include "uart.h"

#define GPIOAEN (1U << 0)
#define UART2EN (1U << 17)
#define DBG_UART_BAUDRATE  (115200)
#define SYS_FREQ  (160000000)
#define APB1_CLK (SYS_FREQ / 4)
#define CR1_TE (1U << 3)
#define CR1_UE (1U << 13)
//#define CR1_RE (1U << 13)
#define SR_TXE (1U << 7)

static void uart_set_baudrate(uint32_t  pheriph_clock, uint32_t baudrate);
static void uart_write(int ch);

int __io_putchar(int ch) {
	uart_write(ch);
	return ch;
}


void uart_init(void) {

	RCC->AHB1ENR |= GPIOAEN;
	//SET MODE PA2 TO ALTERNATE
	GPIOA->MODER &= ~(1U << 4);
	GPIOA->MODER |= (1U << 5);

	//SET ALTERNATE FUNCTION TYPE TO AF7(UART_TX)

	GPIOA->AFR[0]  |= (1U << 8);
	GPIOA->AFR[0]  |= (1U << 9);
	GPIOA->AFR[0]  |= (1U << 10);
	GPIOA->AFR[0]  &= ~(1U << 1);
	//ENABLE CLK
	RCC->APB1ENR = UART2EN;
	//uart_set baudrate
	uart_set_baudrate(APB1_CLK,DBG_UART_BAUDRATE);
	//configure transfer direction
	USART2->CR1 |= CR1_TE;
	//enable uart
	USART2->CR1 |= CR1_UE;

}

static void uart_write(int ch)
{
    /*Make sure transmit data register is empty*/
    while(!(USART2->SR & SR_TXE)){}
    /*Write to transmit data register*/
    USART2->DR =(ch & 0xFF);
}


static uint16_t compute_uart_bd(uint32_t periph_clk,uint32_t baudrate)
{
    return((periph_clk + (baudrate/2U))/baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk,uint32_t baudrate)
{
    USART2->BRR = compute_uart_bd(periph_clk,baudrate);
}




