/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include  <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "uart.h"
#include "systick.h"
#include "spi.h"
#include "imu.h"
#include "tim.h"
#include "i2c.h"
#include "gpio.h"
#include "exti.h"
#define SPI_CS_Pin GPIO_PIN_12
#define SPI_CS_GPIO_Port GPIOC
#define SD_SPI_HANDLE hspi3

void Error_Handler(void);
#endif /* __MAIN_H */
