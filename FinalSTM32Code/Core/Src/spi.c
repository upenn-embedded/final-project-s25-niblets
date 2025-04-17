/*
 * spi.c
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */
#include "spi.h"

void spi_setSpeed(SPI_TypeDef* SPIx,uint8_t speed) {

	SPIx->CR1&=0xFFC7;

	if(speed ==  1) {
		SPIx->CR1 |= SPI_BAUDRATEPRESCALER_2;

	} else {
		SPIx->CR1|= SPI_BAUDRATEPRESCALER_32;
	}
	SPIx->CR1 |= 1<<6;
}

void spi3_gpio_init(void)
{
	/* Enable clock access to GPIOB and GPIOC */
	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	    /* Set PB0, PC10, PC11 to alternate function mode (10 in binary) */
	    GPIOB->MODER &= ~(3U << 0);  // Clear both bits for PB0
	    GPIOB->MODER |= (2U << 0);   // Set to 10 (AF mode) for PB0

	    GPIOC->MODER &= ~(3U << 20); // Clear both bits for PC10
	    GPIOC->MODER |= (2U << 20);  // Set to 10 (AF mode) for PC10

	    GPIOC->MODER &= ~(3U << 22); // Clear both bits for PC11
	    GPIOC->MODER |= (2U << 22);  // Set to 10 (AF mode) for PC11

	    /* Set PC12 (CS) as general-purpose output (01 in binary) */
	    GPIOC->MODER &= ~(3U << 24); // Clear both bits for PC12
	    GPIOC->MODER |= (1U << 24);  // Set to 01 (output mode) for PC12

	    /* Set AF6 for SPI3 pins */
	    // AFR[0] handles pins 0-7, AFR[1] handles pins 8-15
	    // Each pin uses 4 bits in the register

	    // For PB0 (AF6 = 0110 binary)
	    GPIOB->AFR[0] &= ~(0xFU << 0);  // Clear 4 bits for PB0
	    GPIOB->AFR[0] |= (6U << 0);     // Set AF6 for PB0

	    // For PC10 (AF6 = 0110 binary)
	    GPIOC->AFR[1] &= ~(0xFU << 8);  // Clear 4 bits for PC10
	    GPIOC->AFR[1] |= (6U << 8);     // Set AF6 for PC10

	    // For PC11 (AF6 = 0110 binary)
	    GPIOC->AFR[1] &= ~(0xFU << 12); // Clear 4 bits for PC11
	    GPIOC->AFR[1] |= (6U << 12);    // Set AF6 for PC11
}

void spi3_config(void)
{
	/* Enable SPI3 clock */
	    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	    /* Set clock to fPCLK/16 (BR = 0b011) */
	    SPI3->CR1 &= ~(7U << 3);  // Clear all BR bits first
	    SPI3->CR1 |= (3U << 3);   // Set BR to 011 for fPCLK/16

	    /* CPOL = 1, CPHA = 1 */
	    SPI3->CR1 |= (1U << 1);   // CPOL = 1
	    SPI3->CR1 |= (1U << 0);   // CPHA = 1

	    /* Full duplex */
	    SPI3->CR1 &= ~(1U << 10); // Clear RXONLY bit

	    /* MSB first */
	    SPI3->CR1 &= ~(1U << 7);  // LSBFIRST = 0 (MSB first)

	    /* Master mode */
	    SPI3->CR1 |= (1U << 2);   // MSTR = 1

	    /* 8-bit data */
	    SPI3->CR1 &= ~(1U << 11); // DFF = 0 for 8-bit data

	    /* Software slave management: SSM = 1, SSI = 1 */
	    SPI3->CR1 |= (1U << 8) | (1U << 9);

	    /* Enable SPI3 */
	    SPI3->CR1 |= (1U << 6);   // SPE = 1
}

void spi3_transmit(uint8_t *data, uint32_t size)
{
	uint32_t i = 0;
	    volatile uint8_t temp;

	    while (i < size)
	    {
	        while (!(SPI3->SR & (1U << 1))) {} // Wait until TXE is set
	        SPI3->DR = data[i++];
	    }

	    while (!(SPI3->SR & (1U << 1))) {} // Wait for TXE
	    while ((SPI3->SR & (1U << 7))) {}  // Wait for BSY = 0

	    // Clear any potential overrun flag
	    temp = SPI3->DR;
	    temp = SPI3->SR;
}

void spi3_receive(uint8_t *data, uint32_t size)
{
    while (size--)
    {
        SPI3->DR = 0xFF; // Send dummy byte
        while (!(SPI3->SR & (1U << 0))) {} // Wait for RXNE
        *data++ = SPI3->DR;
    }
    while ((SPI3->SR & (1U << 7))) {}
}

void cs3_enable(void)
{
    GPIOC->ODR &= ~(1U << 12);
}

void cs3_disable(void)
{
    GPIOC->ODR |= (1U << 12);
}
