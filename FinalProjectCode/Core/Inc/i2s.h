/*
 * I2S_DMA.h
 *
 *  Created on: Apr 10, 2025
 *      Author: banele
 */

#ifndef I2S_H_
#define I2S_H_

#include "stm32f4xx.h"


void I2S2_init(void);
void I2S2_gpio_config(void);
void I2S_config(void);
void ws2_enable(void);
void ws2_disable(void);
void I2S2_DMA_config(void);
void I2S2_transmit_DMA(uint16_t *buffer, uint16_t len);
void I2S2_transmit(uint16_t * buffer, uint8_t len);

#endif /* I2S_H_ */
