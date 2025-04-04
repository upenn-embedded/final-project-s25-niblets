/*
 * imu.h
 *	IMU: ICM20948
 *  Created on: Mar 13, 2025
 *  Author: banele
 */

#ifndef IMU_H_
#define IMU_H_
#include <stdint.h>
#include "spi.h"
#define ICM20948_REG_DEVID               (0x0F)
#define ICM20948_REG_DATA_FORMAT         (0x14)
#define ICM20948_REG_POWER_CTL           (0x06)
#define ICM20948_REG_DATA_START          (0x2D)
#define ICM20948_CTRL1_XL_CONFIG         (0x68)
#define ICM20948_TEMP_CONFIG			 (0x83)
#define ICM20948_RESET                   (0x01)
#define ICM20948_MEASURE_BIT             (0x08)
#define ICM20948_MULTI_BYTE_ENABLE       (0x40)
#define ICM20948_READ_OPERATION          (0x80)
#define CTRL1_XL						 (0x10)
#define CTRL3_C 						 (0x12)
#define DISABLE_I2C					     (0x03)
#define CTRL4_C							 (0x13)

void imu_init (void);
void imu_read(uint8_t address, uint8_t * rxdata);
void imu_write (uint8_t address, uint8_t value);
#endif /* IMU_H_ */

