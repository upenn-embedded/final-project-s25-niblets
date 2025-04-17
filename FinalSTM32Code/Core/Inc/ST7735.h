/*
 * ST7735.h
 *
 *  Created on: Apr 6, 2025
 *  Author: banele adaptationfrom ws4
 */
#include "stm32f4xx.h"
#ifndef ST7735_STM32_DRIVER_H
#define ST7735_STM32_DRIVER_H

#define ST7735_SCREEN_HEIGHT 	240
#define ST7735_SCREEN_WIDTH 	320

/* PIN Configuration */
#define LCD_CS_PORT				GPIOB
#define LCD_CS_PIN				GPIO_PIN_10
#define LCD_DC_PORT				GPIOB
#define LCD_DC_PIN				GPIO_PIN_0
#define	LCD_RST_PORT			GPIOB
#define	LCD_RST_PIN				GPIO_PIN_1

#define BURST_MAX_SIZE 			500
#define BLACK      				0x0000
#define NAVY        			0x000F
#define DARKGREEN   			0x03E0
#define DARKCYAN    			0x03EF
#define MAROON      			0x7800
#define PURPLE      			0x780F
#define OLIVE       			0x7BE0
#define LIGHTGREY   			0xC618
#define DARKGREY    			0x7BEF
#define BLUE        			0x001F
#define GREEN       			0x07E0
#define CYAN        			0x07FF
#define RED         			0xF800
#define MAGENTA     			0xF81F
#define YELLOW      			0xFFE0
#define WHITE       			0xFFFF
#define ORANGE      			0xFD20
#define GREENYELLOW 			0xAFE5
#define PINK        			0xF81F

// ST7735 registers
#define ST7735_NOP      0x00
#define ST7735_SWRESET  0x01
#define ST7735_RDDID    0x04
#define ST7735_RDDST    0x09
#define ST7735_SLPIN    0x10
#define ST7735_SLPOUT   0x11
#define ST7735_PTLON    0x12
#define ST7735_NORON    0x13
#define ST7735_INVOFF   0x20
#define ST7735_INVON    0x21
#define ST7735_DISPOFF  0x28
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E
#define ST7735_PTLAR    0x30
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36
#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_DISSET5  0xB6
#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5
#define ST7735_RDID1    0xDA
#define ST7735_RDID2    0xDB
#define ST7735_RDID3    0xDC
#define ST7735_RDID4    0xDD
#define ST7735_PWCTR6   0xFC
#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1
#define MADCTL_MY  	 	0x80
#define MADCTL_MX  		0x40
#define MADCTL_MV  		0x20
#define MADCTL_ML  		0x10
#define MADCTL_RGB 		0x00
#define MADCTL_BGR 		0x08
#define MADCTL_MH  		0x04

#define SCREEN_VERTICAL_1		0
#define SCREEN_HORIZONTAL_1		1
#define SCREEN_VERTICAL_2		2
#define SCREEN_HORIZONTAL_2		3


void lcd_Reset(void);
void SPI_ControllerTx(uint8_t data);
void SPI_ControllerTx_stream(uint8_t stream);
void SPI_ControllerTx_16bit(uint16_t data);
void SPI_ControllerTx_16bit_stream(uint16_t data);
void lcd_init(void);
void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void sendCommands(const uint8_t *cmds, uint8_t length);
#endif
