/*
 * imu.h
 *	IMU: LMSDSO
 *  Created on: Mar 13, 2025
 *  Author: banele
 */

#ifndef IMU_H_
#define IMU_H_
#include <stdint.h>
#include "spi.h"

//6Ch
#define LMSDSO_REG_DEVID               (0x0F)
#define LMSDSO_REG_DATA_FORMAT         (0x14)
#define LMSDSO_REG_POWER_CTL           (0x06)
#define LMSDSO_REG_DATA_START          (0x2D)
#define LMSDSO_CTRL1_XL_CONFIG         (0x68)
#define LMSDSO_TEMP_CONFIG			 (0x83)
#define LMSDSO_RESET                   (0x01)
#define LMSDSO_MEASURE_BIT             (0x08)
#define LMSDSO_MULTI_BYTE_ENABLE       (0x40)
#define LMSDSO_READ_OPERATION          (0x80)
#define CTRL1_XL						 (0x10)
#define CTRL3_C 						 (0x12)
#define DISABLE_I2C					     (0x03)
#define CTRL4_C							 (0x13)

typedef enum {
	SCREEN_VERTICAL_1,
	SCREEN_VERTICAL_2,
	SCREEN_HORIZONTAL_1,
	SCREEN_HORIZONTAL_2
} rotation;

void imu_init (void);
void imu_read(uint8_t address, uint8_t * rxdata, uint8_t len);
void imu_write (uint8_t address, uint8_t value);
#endif /* IMU_H_ */

