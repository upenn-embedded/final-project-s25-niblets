/*
 * imu.c
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */

#include "imu.h"

void imu_read(uint8_t address, uint8_t * rxdata) {
      /*Set read operation*/
      address |= ICM20948_READ_OPERATION;
      /*Enable multi-byte*/
      //address |= ICM20948_MULTI_BYTE_ENABLE;
      /*Pull cs line low to enable slave*/
      cs1_enable();
      /*Send address*/
      spi1_transmit(&address,1);
      /*Read 6 bytes */
      spi1_receive(rxdata,6);
      /*Pull cs line high to disable slave*/
      cs1_disable();
}
void imu_write (uint8_t address, uint8_t value) {
  uint8_t data[2];
  /*Enable multi-byte, place address into buffer*/
  data[0] = address & 0x7F;
  /*Place data into buffer*/
  data[1] = value;
  /*Pull cs line low to enable slave*/
  cs1_enable();
  /*Transmit data and address*/
  spi1_transmit(data, 2);
  /*Pull cs line high to disable slave*/
  cs1_disable();
}

void imu_init (void) {
    /*Enable SPI gpio*/
    spi1_gpio_init();
    /*Config SPI*/
    spi1_config();
    /*Set data format range to +-4g*/
    /* turn on the accelerometer*/
    imu_write (CTRL1_XL, ICM20948_CTRL1_XL_CONFIG);
    /*Reset all bits*/
    imu_write (CTRL3_C, ICM20948_RESET);
    /*Configure power control measure bit*/
    //imu_write (ICM20948_REG_POWER_CTL, ICM20948_MEASURE_BIT);
    imu_write(CTRL4_C, DISABLE_I2C);

    imu_write (CTRL1_XL, ICM20948_CTRL1_XL_CONFIG);
}


