/*
 * gui.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: Praise
 */


#include <string.h>
#include <stdlib.h>
#include "gui.h"

/******************************************************************************
* Local Functions
******************************************************************************/


uint16_t myabs(int16_t x)
{
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

void plotLineLow(short x0, short y0, short x1, short y1, uint16_t c)
{
	short dx = x1 - x0;
	short dy = y1 - y0;
	short yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	short D = (2 * dy) - dx;
	short y = y0;

	for (short x = x0; x <= x1; x++) {
		LCD_drawPixel(x, y, c);
		if (D > 0) {
			y = y + yi;
			D = D + (2 * (dy - dx));
		} else {
			D = D + 2*dy;
		}
	}
}

void plotLineHigh(short x0, short y0, short x1, short y1, uint16_t c)
{
	short dx = x1 - x0;	//delta x
	short dy = y1 - y0;	//delta y
	short xi = 1;

	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	short D = (2 * dx) - dy;
	short x = x0;

	for (short y = y0; y <= y1; y++) {
		LCD_drawPixel(x, y, c);
		if (D > 0) {
			x = x + xi;
			D = D + (2 * (dx - dy));
		} else {
			D = D + 2*dx;
		}
	}
}

void drawCharacter(uint16_t x, uint16_t y, const char character[CHAR_WIDTH], uint16_t color) {
	for (int row = 0; row < CHAR_HEIGHT; row++) {
		for (int col = 0; col < CHAR_WIDTH; col++) {
			if (character[col] & (1 << row)) {
				LCD_drawPixel(x + col, y + row, color);
			}
		}
	}
}

/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

uint16_t rgb(uint8_t red, uint8_t green, uint8_t blue) {
    return rgb565(red,green,blue);
}
/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)& (1==1)){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
	short t1 = radius / 16;  // Initialize error term
	short x, y = 0;   // Start at (r,0)
	x = radius;

	while (x >= y)  // Loop until x < y
	{
		// Draw 8-way symmetric pixels
		LCD_drawPixel(x0 + x, y0 + y, color);
		LCD_drawPixel(x0 - x, y0 + y, color);
		LCD_drawPixel(x0 + x, y0 - y, color);
		LCD_drawPixel(x0 - x, y0 - y, color);
		LCD_drawPixel(x0 + y, y0 + x, color);
		LCD_drawPixel(x0 - y, y0 + x, color);
		LCD_drawPixel(x0 + y, y0 - x, color);
		LCD_drawPixel(x0 - y, y0 - x, color);

		y += 1;
		t1 += y;
		short t2 = t1 - x;
		if (t2 >= 0) {
			t1 = t2;
			x -= 1;
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(int16_t x0,int16_t y0,int16_t x1,int16_t y1,uint16_t c)
{
	if (myabs(y1 - y0) < myabs(x1 - x0)) {
		if (x0 > x1) {
			plotLineLow(x1, y1, x0, y0, c);
		} else {
			plotLineLow(x0, y0, x1, y1, c);
		}
	} else {
		if (y0 > y1) {
			plotLineHigh(x1, y1, x0, y0, c);
		} else {
			plotLineHigh(x0, y0, x1, y1, c);
		}
	}
}


/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	int startx, starty, endx, endy;

	// Fill this out
	if (x0 < x1) {
		startx = x0;
		endx = x1;
	} else {
		startx = x1;
		endx = x0;
	}

	if (y0 < y1) {
		starty = y0;
		endy = y1;
	} else {
		starty = y1;
		endy = y0;
	}

    LCD_setAddr(x0,y0,x1,y1);
	for (int i = startx; i <= endx; i++) {
		for (int j = starty; j <= endy; j++) {
			SPI_ControllerTx_16bit(color);
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color)
{
	LCD_drawBlock(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	// Fill this out
    uint16_t len = strlen(str);
    uint16_t col = 0;
    LCD_drawBlock(x, y, x + CHAR_WIDTH * len, y + CHAR_HEIGHT, bg);

    for (int i = 0; i < len; i++) {
        col = x + (i * CHAR_WIDTH);
        drawCharacter(col, y, ASCII[str[i] - 0x20], fg);
    }
}

void LCD_drawButton(short x, short y, char * label) {
    short len = strlen(label);
    short buttonWidth = 5 * len + 20;
    LCD_drawLine(x,y, x, y + 25, rgb(255,255,255));
    LCD_drawLine(x + buttonWidth,y, x + buttonWidth, y + 25, rgb(255,255,255));
    LCD_drawLine(x,y, x + buttonWidth, y, rgb(255,255,255));
    LCD_drawLine(x, y + 25, x + buttonWidth, y + 25, rgb(255,255,255));
    LCD_drawString(x+10, y + 10, label, rgb(255,255,255), rgb(0,0,0));
 }

 void LCD_DrawBitMap(uint16_t height, uint16_t width, uint16_t bitmap[] ) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint16_t color = bitmap[y * width + x];  // Get the pixel color in RGB565 format
            LCD_drawPixel(x, y, color);  // Draw the pixel on the screen
        }
    }
}
