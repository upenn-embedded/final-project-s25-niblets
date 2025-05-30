/*
 * spi.h
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */

#ifndef SPI_H_
#define SPI_H_
#include "stm32f446xx.h"
#include <stdint.h>


//SPI 1 FUNCTIONS

void spi1_gpio_init(void);
void spi1_config(void);
void spi1_transmit(uint8_t *data,uint32_t size);
void spi1_receive(uint8_t *data,uint32_t size);
void cs1_enable(void);
void cs1_disable(void);

#endif /* SPI_H_ */
