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

//void I2S3_init(void);
//void I2S2_transmit(uint16_t * buffer, uint8_t len);
//void I2S3_transmit(uint16_t * buffer, uint8_t len);
//void I2S2_receive(uint16_t * buffer, uint8_t len);
//void I2S3_receive(uint16_t * buffer, uint8_t len);


//static void MX_I2S2_Init(void)
//{
//
//  /* USER CODE BEGIN I2S2_Init 0 */
//
//  /* USER CODE END I2S2_Init 0 */
//
//  /* USER CODE BEGIN I2S2_Init 1 */
//
//  /* USER CODE END I2S2_Init 1 */
//  hi2s2.Instance = SPI2;
//  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
//  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
//  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
//  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
//  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_44K;
//  hi2s2.Init.CPOL = I2S_CPOL_LOW;
//  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
//  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
//  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
//  {
//    Error_Handler();
//  }



#endif /* I2S_H_ */
