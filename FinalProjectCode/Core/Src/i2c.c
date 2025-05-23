/*
 * i2c.c
 *
 *  Created on: Mar 12, 2025
 *      Author: banele
 */


#include <stm32f446xx.h>
#include "i2c.h"
#define     GPIOBEN            (1U<<1)
#define     I2C1EN            (1U<<21)
#define     I2C_100KHZ            80
#define     SD_MODE_MAX_RISE_TIME    17
#define    CR1_PE            (1U<<0)
#define    SR2_BUSY            (1U<<1)
#define    CR1_START            (1U<<8)
#define    SR1_SB            (1U<<0)
#define    SR1_ADDR            (1U<<1)
#define    SR1_TXE            (1U<<7)
#define    CR1_ACK            (1U<<10)
#define    CR1_STOP            (1U<<9)
#define    SR1_RXNE            (1U<<6)
#define    SR1_BTF            (1U<<2)

#define PCLK1_MHZ          40U          /* 160 MHz / 4  */
#define I2C_CCR_STD_100k   ((PCLK1_MHZ * 1000000U) / (4U * 100000U))   /* 200 */
#define I2C_TRISE_STD      (PCLK1_MHZ + 1U)                            /* 41  */

void i2c1_init(void)
{
    RCC->AHB1ENR |= GPIOBEN;

    /* PB8, PB9 -> AF4, open-drain, pull-up ------------------------------ */
    GPIOB->MODER  &= ~((3U << 16) | (3U << 18));
    GPIOB->MODER  |=  ((2U << 16) | (2U << 18));        /* AF mode        */

    GPIOB->OTYPER |=  (1U << 8) | (1U << 9);            /* open-drain      */

    GPIOB->PUPDR  &= ~((3U << 16) | (3U << 18));
    GPIOB->PUPDR  |=  ((1U << 16) | (1U << 18));        /* pull-up         */

    GPIOB->AFR[1] &= ~((0xFU << 0) | (0xFU << 4));      /* AF4 = I2C1      */
    GPIOB->AFR[1] |=  ((4U  << 0) | (4U  << 4));

    /* 2. Enable I2C1 peripheral clock ---------------------------------- */
    RCC->APB1ENR |= I2C1EN;

    /* 3. Reset I2C1 ----------------------------------------------------- */
    I2C1->CR1 |=  (1U << 15);
    I2C1->CR1 &= ~(1U << 15);

    /* 4. Timing configuration for 100 kHz ------------------------------- */
    I2C1->CR2   = PCLK1_MHZ;           /* peripheral clock in MHz (≤45)     */
    I2C1->CCR   = I2C_CCR_STD_100k;    /* 200                               */
    I2C1->TRISE = I2C_TRISE_STD;       /* 41                                */

    /* 5. Enable I2C1 ---------------------------------------------------- */
    I2C1->CR1 |= CR1_PE;
}


void i2c1_byte_read(char saddr, char maddr, char* data) {

      volatile int tmp;
      /* Wait until bus not busy */
      while (I2C1->SR2 & (SR2_BUSY)){}
      /* Generate start */
      I2C1->CR1 |= CR1_START;
      /* Wait until start flag is set */
      while (!(I2C1->SR1 & (SR1_SB))){}
      /* Transmit slave address + Write */
      I2C1->DR = saddr << 1;
      /* Wait until addr flag is set */
      while (!(I2C1->SR1 & (SR1_ADDR))){}
      /* Clear addr flag */
      tmp = I2C1->SR2;
      /* Send memory address */
      I2C1->DR = maddr;
      /*Wait until transmitter empty */
      while (!(I2C1->SR1 & SR1_TXE)){}
      /*Generate restart */
      I2C1->CR1 |= CR1_START;
      /* Wait until start flag is set */
      while (!(I2C1->SR1 & SR1_SB)){}
      /* Transmit slave address + Read */
      I2C1->DR = saddr << 1 | 1;
      /* Wait until addr flag is set */
      while (!(I2C1->SR1 & (SR1_ADDR))){}
      /* Disable Acknowledge */
      I2C1->CR1 &= ~CR1_ACK;
      /* Clear addr flag */
      tmp = I2C1->SR2;
      /* Generate stop after data received */
      I2C1->CR1 |= CR1_STOP;
      /* Wait until RXNE flag is set */
      while (!(I2C1->SR1 & SR1_RXNE)){}
      /* Read data from DR */
      *data++ = I2C1->DR;
}

void i2c1_burst_read(char saddr, char maddr, int n, char* data) {
    volatile int tmp;
     /* Wait until bus not busy */
     while (I2C1->SR2 & (SR2_BUSY)){}
     /* Generate start */
     I2C1->CR1 |= CR1_START;
     /* Wait until start flag is set */
     while (!(I2C1->SR1 & SR1_SB)){}
     /* Transmit slave address + Write */
     I2C1->DR = saddr << 1;
     /* Wait until addr flag is set */
     while (!(I2C1->SR1 & SR1_ADDR)){}
    /* Clear addr flag */
     tmp = I2C1->SR2;
    /* Wait until transmitter empty */
    while (!(I2C1->SR1 & SR1_TXE)){}
    /*Send memory address */
    I2C1->DR = maddr;
    /*Wait until transmitter empty */
    while (!(I2C1->SR1 & SR1_TXE)){}
    /*Generate restart */
    I2C1->CR1 |= CR1_START;
    /* Wait until start flag is set */
    while (!(I2C1->SR1 & SR1_SB)){}
    /* Transmit slave address + Read */
    I2C1->DR = saddr << 1 | 1;
    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}
    /* Clear addr flag */
    tmp = I2C1->SR2;
    /* Enable Acknowledge */
      I2C1->CR1 |=  CR1_ACK;
    while(n > 0U)
    {
        /*if one byte*/
        if(n == 1U)
        {
            /* Disable Acknowledge */
            I2C1->CR1 &= ~CR1_ACK;
            /* Generate Stop */
            I2C1->CR1 |= CR1_STOP;
            /* Wait for RXNE flag set */
            while (!(I2C1->SR1 & SR1_RXNE)){}
            /* Read data from DR */
            *data++ = I2C1->DR;
            break;
        }
        else
        {
           /* Wait until RXNE flag is set */
           while (!(I2C1->SR1 & SR1_RXNE)){}
           /* Read data from DR */
           (*data++) = I2C1->DR;
           n--;
        }
    }
}

void i2c1_burst_write(char saddr, char maddr, int n, char* data) {
    volatile int tmp;
    /* Wait until bus not busy */
    while (I2C1->SR2 & (SR2_BUSY)){}
    /* Generate start */
    I2C1->CR1 |= CR1_START;
    /* Wait until start flag is set */
    while (!(I2C1->SR1 & (SR1_SB))){}
    /* Transmit slave address */
    I2C1->DR = saddr << 1;
    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}
    /* Clear addr flag */
    tmp = I2C1->SR2;
    /* Wait until data register empty */
    while (!(I2C1->SR1 & (SR1_TXE))){}
    /* Send memory address */
    I2C1->DR = maddr;
    for (int i = 0; i < n; i++) {
     /* Wait until data register empty */
        while (!(I2C1->SR1 & (SR1_TXE))){}
      /* Transmit memory address */
      I2C1->DR = *data++;
    }
    /* Wait until transfer finished */
    while (!(I2C1->SR1 & (SR1_BTF))){}
    /* Generate stop */
    I2C1->CR1 |= CR1_STOP;
}
