/*
 * imu.c
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */

#include "imu.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include "main.h"
#include <stdio.h>    // for sprintf
#include <string.h>   // for strlen, memset, etc.

void imu_i2c_init()
{
	i2c1_init();
	gyro_init();
}

void gyro_init()
{
	uint8_t ctrl2_g_value = ODR_104Hz | FS_250;
   i2c1_burst_write(LSM6D50_ADDR, CTRL2_G, 1, &ctrl2_g_value);
   HAL_Delay(10);
}

void gyro_read(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{
   uint8_t gyroData[6];
   i2c1_burst_read(LSM6D50_ADDR, OUTX_L_G, 6, gyroData);
   HAL_Delay(10);
   *gyro_x = (int16_t)((gyroData[1] << 8) | gyroData[0]);
   *gyro_y = (int16_t)((gyroData[3] << 8) | gyroData[2]);
   *gyro_z = (int16_t)((gyroData[5] << 8) | gyroData[4]);
}
