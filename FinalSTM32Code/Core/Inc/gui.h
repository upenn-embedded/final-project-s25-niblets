/*
 * gui.h
 *
 *  Created on: Apr 5, 2025
 *      Author: banele
 */

#ifndef GUI_H_
#define GUI_H_
#include <stdint.h>
#include "ASCII_LUT.h"
#include "ST7735.h"

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define LCD_WIDTH 180
#define LCD_HEIGHT 128

void plotLineLow(short x0, short y0, short x1, short y1, uint16_t c);
void drawCharacter(uint16_t x, uint16_t y, const char character[CHAR_WIDTH], uint16_t color);
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor);
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color);
void LCD_drawLine(int16_t x0,int16_t y0,int16_t x1,int16_t y1,uint16_t c);
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color);
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg);
void LCD_setScreen(uint16_t color);
void LCD_drawButton(short x, short y, char * label);
void LCD_DrawBitMap(uint16_t height, uint16_t width, uint16_t bitmap[]);

#endif /* GUI_H_ */
