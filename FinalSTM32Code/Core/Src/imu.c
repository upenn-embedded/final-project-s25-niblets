/*
 * imu.c
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */

#include "imu.h"
#include "systick.h"
#include "uart.h"
#include <stdio.h>

#define who_am_i (0x0F)

void imu_read(uint8_t address, uint8_t * rxdata, uint8_t len) {
      /*Set read operation*/
      address |= LMSDSO_READ_OPERATION;
      /*Enable multi-byte*/
      //address |= LMSDSO_MULTI_BYTE_ENABLE;
      /*Pull cs line low to enable slave*/

      cs3_enable();
      /*Send address*/
      spi3_transmit(&address,len);
      /*Read 6 bytes */
      spi3_receive(rxdata,len);
      /*Pull cs line high to disable slave*/
      cs3_disable();
}

void imu_write (uint8_t address, uint8_t value) {

  uint8_t data[2];
  /*Enable multi-byte, place address into buffer*/
  data[0] = address & 0x7F;
  /*Place data into buffer*/
  data[1] = value;
  /*Pull cs line low to enable slave*/
  cs3_enable();
  /*Transmit data and address*/
  spi3_transmit(data, 2);
  /*Pull cs line high to disable slave*/
  cs3_disable();
}

void imu_init (void) {
    /*Enable SPI gpio*/
    spi3_gpio_init();
    /*Config SPI*/
    spi3_config();

    /*Set data format range to +-4g*/
    /* turn on the accelerometer*/
    printf("writing init in\r\n");

    imu_write(CTRL1_XL, LMSDSO_CTRL1_XL_CONFIG);
    /*Reset all bits*/
    imu_write (CTRL3_C, LMSDSO_RESET);
    /*Configure power control measure bit*/
    imu_write(LMSDSO_REG_POWER_CTL, LMSDSO_MEASURE_BIT);
    imu_write(CTRL4_C, DISABLE_I2C);
}

rotation getRotation(void){
	return SCREEN_HORIZONTAL_1;
}

