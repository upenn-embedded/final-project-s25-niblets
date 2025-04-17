/*
 * spi.h
 *
 *  Created on: Mar 13, 2025
 *      Author: banele
 */

#ifndef SPI_H_
#define SPI_H_
#include "stm32f4xx_hal.h"
#include <stdint.h>

//#define SPI_BAUDRATEPRESCALER_2    (0x0 << 3)  // fPCLK / 2
//#define SPI_BAUDRATEPRESCALER_4    (0x1 << 3)  // fPCLK / 4
//#define SPI_BAUDRATEPRESCALER_8    (0x2 << 3)  // fPCLK / 8
//#define SPI_BAUDRATEPRESCALER_16   (0x3 << 3)  // fPCLK / 16
//#define SPI_BAUDRATEPRESCALER_32   (0x4 << 3)  // fPCLK / 32
//#define SPI_BAUDRATEPRESCALER_64   (0x5 << 3)  // fPCLK / 64
//#define SPI_BAUDRATEPRESCALER_128  (0x6 << 3)  // fPCLK / 128
//#define SPI_BAUDRATEPRESCALER_256  (0x7 << 3)  // fPCLK / 256
#define SPI1EN            (1U<<12)
#define SPI4EN			  (1U<<13)
#define GPIOAEN            (1U<<0)
#define SR_TXE            (1U<<1)
#define SR_RXNE            (1U<<0)
#define SR_BSY            (1U<<7)


//#define SD_CS_GPIO_Port  GPIOB
//#define SD_CS_PIN 		6
//#define SD_SPI_HANDLE  SPI1

#define SET_PIN(GPIO_PORT, PIN)     ((GPIO_PORT)->ODR |= (1U << (PIN)))
#define CLEAR_PIN(GPIO_PORT, PIN)   ((GPIO_PORT)->ODR &= ~(1U << (PIN)))

//GENERIC FUNCTIONS
void spi_setSpeed(SPI_TypeDef* SPIx,uint8_t speed);

//SPI 1 FUNCTIONS
//void spi2_gpio_init(void);
//void spi2_config(void);
//void spi2_transmit(uint8_t *data,uint32_t size);
//void spi2_receive(uint8_t *data,uint32_t size);
//void cs2_enable(void);
//void cs2_disable(void);



//SPI 4 FUNCTIONS
void spi3_gpio_init(void);
void spi3_config(void);
void spi3_transmit(uint8_t *data,uint32_t size);
void spi3_receive(uint8_t *data,uint32_t size);
void cs3_enable(void);
void cs3_disable(void);

#endif /* SPI_H_ */
