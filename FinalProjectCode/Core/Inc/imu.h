/*
 * imu.h
 *	IMU: ICM20948
 *  Created on: Mar 13, 2025
 *  Author: banele
 */

#ifndef IMU_H_
#define IMU_H_
#include <stdint.h>
#include "i2c.h"

#define LSM6D50_ADDR   0x6B
#define CTRL2_G          0x11  // Gyroscope control register (11h)
#define OUTX_L_G         0x22  // Starting register for gyro X low byte
#define ODR_104Hz        0x40  // Output Data Rate: 104 Hz (example)
#define FS_250           0x00  // Full-scale: ±250 dps (example)

void imu_i2c_init();
void gyro_init();
void gyro_read(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);

#endif /* IMU_H_ */
