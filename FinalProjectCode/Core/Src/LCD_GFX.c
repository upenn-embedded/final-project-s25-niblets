/*
 * LCD_GFX.c
 *
 * Adapted from GFX.c for LCD rendering
 */
#include "stm32f4xx_hal.h"
#include "LCD_GFX.h"
#include "ST7735.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "spi.h"



 #define NUMBER_OF_SONGS 10

 /******************************************************************************
 * Global Variables
 ******************************************************************************/
 #define CHAR_WIDTH 5
 #define CHAR_HEIGHT 8

 int is_playing = 1;

 /******************************************************************************
 * Local Functions
 ******************************************************************************/


 void plotLineFast(short x0, short y0, short x1, short y1, uint32_t c, const uint16_t bitmap[]){
 	 short dx = x1 - x0;
 	 short dy = y1 - y0;
 	 short temp;

 	 /* horizontal line */
 	 if (dy == 0) {
 		 LCD_setAddr(x0, y0, x1, y0);         /* inclusive end-coords   */
 	     for (short i = 0; i <= dx; ++i)      /* send dx+1 pixels       */
 	    	 SPI_ControllerTx_16bit(c);
 	    }
 	 /* vertical line */
 	 else if (dx == 0) {
 	    LCD_setAddr(x0, y0, x0, y1);
 	    for (short i = 0; i <= dy; ++i)
 	    	SPI_ControllerTx_16bit(c);
 	 }

 	 if (dy < 0){
 		 LCD_setAddr(x0,y1,x1-1,y0-1);
 		 dy = -dy;
 		 for (short j = 0; j < dy; j++){
 			 for (short i = 0; i < dx; i++){
 				 // Current position given is (x0 + i, y1 + j)
 				 if ((dy * i / dx + x0) == (y0 - j)){
 		    	      SPI_ControllerTx_16bit(c);
 		    	 } else {
 		    		 uint16_t b = bitmap[(y1 + j) * LCD_WIDTH + (x0 + i)];
 			    	 SPI_ControllerTx_16bit(b);
 				 }
 			 }
 		 }
 	 } else {
 		 LCD_setAddr(x0,y0,x1-1,y1-1);
 		 for (short j = 0; j < dy; j++){
 			 for (short i = 0; i < dx; i++){
 				 // Current position given is (x0 + i, y1 + j)
 				 if ((dy * i / dx + x0) == (y0 + j)){
 		    	      SPI_ControllerTx_16bit(c);
 		    	 } else {
 		    		 uint16_t b = bitmap[(y0 + j) * LCD_WIDTH + (x0 + i)];
 			    	 SPI_ControllerTx_16bit(b);
 				 }
 			 }
 		 }
 	 }
  }


 void plotLineLow(short x0, short y0, short x1, short y1, uint32_t c)
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

 void plotLineHigh(short x0, short y0, short x1, short y1, uint32_t c)
 {
     short dx = x1 - x0;
     short dy = y1 - y0;
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

 /******************************************************************************
 * Global Functions
 ******************************************************************************/

 /**************************************************************************//**
 * @fn            void LCD_updateScreen(void)
 * @brief        Update the screen with the current pixel buffer
 * @note
 *****************************************************************************/

 /**************************************************************************//**
 * @fn            uint32_t rgb565_to_argb8888(uint16_t color)
 * @brief        Convert RGB565 value to ARGB8888 32-bit color data
 * @note
 *****************************************************************************/
 uint32_t rgb565_to_argb8888(uint16_t color)
 {
     uint8_t r = (color >> 11) & 0x1F;
     uint8_t g = (color >> 5) & 0x3F;
     uint8_t b = color & 0x1F;

     // Scale up to 8-bit per channel
     r = (r * 255) / 31;
     g = (g * 255) / 63;
     b = (b * 255) / 31;

     return (0xFF << 24) | (r << 16) | (g << 8) | b;
 }

 /**************************************************************************//**
 * @fn            uint32_t rgb(uint8_t red, uint8_t green, uint8_t blue)
 * @brief        Convert RGB888 value to ARGB8888 32-bit color data
 * @note
 *****************************************************************************/
 uint32_t rgb(uint8_t red, uint8_t green, uint8_t blue)
 {
     return (0xFF << 24) | (red << 16) | (green << 8) | blue;
 }

 /**************************************************************************//**
 * @fn            uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 * @brief        Convert RGB888 value to RGB565 16-bit color data
 * @note
 *****************************************************************************/
 uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 {
     return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
 }

 /**************************************************************************//**
 * @fn            void LCD_drawPixel(uint8_t x, uint8_t y, uint32_t color)
 * @brief        Draw a single pixel of 32-bit ARGB8888 color to the x & y coordinate
 * @note
 *****************************************************************************/
 void LCD_drawPixel(short x, short y, uint32_t color)
 {
     if ( (unsigned int) x < SCREEN_WIDTH && (unsigned int) y < SCREEN_HEIGHT) {
    	    LCD_setAddr(x,y,x,y);
    	    SPI_ControllerTx_16bit(color);
     }
 }

 /**************************************************************************//**
 * @fn            void LCD_drawPixel_565(uint8_t x, uint8_t y, uint16_t color)
 * @brief        Draw a single pixel of 16-bit RGB565 color to the x & y coordinate
 * @note         Converts RGB565 to ARGB8888 for LCD
 *****************************************************************************/
 void LCD_drawPixel_565(short x, short y, uint16_t color)
 {
     LCD_drawPixel(x, y, rgb565_to_argb8888(color));
 }

 /**************************************************************************//**
 * @fn            void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint32_t fColor, uint32_t bColor)
 * @brief        Draw a character starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 void LCD_drawChar(short x, short y, uint16_t character, uint32_t fColor, uint32_t bColor)
 {
     uint16_t row = character - 0x20;        // Determine row of ASCII table starting at space
     int i, j;
     if ((SCREEN_WIDTH-x>7) && (SCREEN_HEIGHT-y>7)) {
         for(i=0; i<5; i++) {
             short pixels = ASCII[row][i]; // Go through the list of pixels
             for(j=0; j<8; j++) {
                 if ((pixels>>j) & 1) {
                     LCD_drawPixel(x+i, y+j, fColor);
                 }
                 else {
                     LCD_drawPixel(x+i, y+j, bColor);
                 }
             }
         }
     }
 }

void LCD_drawCharFast(short x, short y, uint16_t character, uint32_t fColor, const uint16_t bitmap[])
{
    uint16_t row = character - 0x20;        // Determine row of ASCII table starting at space
    int i, j;
	LCD_setAddr(x,y,x+4,y+7);
    if ((SCREEN_WIDTH-x>7) && (SCREEN_HEIGHT-y>7)) {
//        for(i=0; i<5; i++) {
//            short pixels = ASCII[row][i]; // Go through the list of pixels
//            for(j=0; j<8; j++) {
//                if ((pixels>>j) & 1) {
//                	SPI_ControllerTx_16bit(fColor);
//                } else {
//                	SPI_ControllerTx_16bit(65535);
//                }
////                else {
////                    uint16_t bcolor = bitmap[(y + j) * LCD_WIDTH + (x + i)];
////                	SPI_ControllerTx_16bit(bcolor);
////                }
//            }
//        }
    	for (j = 0; j < 8; ++j) {       // for each row of the glyph…
    	  for (i = 0; i < 5; ++i) {     //   …feed each column in that row
    	    if ( ASCII[row][i] & (1 << j) ) {
    	      SPI_ControllerTx_16bit(fColor);
    	    } else {
    	      uint16_t b = bitmap[(y + j) * LCD_WIDTH + (x + i)];
    	      SPI_ControllerTx_16bit(b);
    	    }
    	  }
    	}
    }
}
 /**************************************************************************//**
 * @fn            void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint32_t color)
 * @brief        Draw a colored circle of set radius at coordinates
 * @note
 *****************************************************************************/
 void LCD_drawCircle(short x0, short y0, short radius, uint32_t color)
 {
     short t1 = radius / 16;  // Initialize error term
     short x = radius;
     short y = 0;   // Start at (r,0)

     while (x >= y) {  // Loop until x < y
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
 * @fn            void LCD_drawLine(short x0, short y0, short x1, short y1, uint32_t c)
 * @brief        Draw a line from and to a point with a color
 * @note
 *****************************************************************************/
 void LCD_drawLine(short x0, short y0, short x1, short y1, uint32_t c)
 {
     if (abs(y1 - y0) < abs(x1 - x0)) {
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
 * @fn            void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint32_t color)
 * @brief        Draw a colored block at coordinates
 * @note
 *****************************************************************************/
 void LCD_drawBlock(short x0, short y0, short x1, short y1, uint32_t color)
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
		for (int i = startx; i < endx; i++) {
			for (int j = starty; j < endy; j++) {
				SPI_ControllerTx_16bit(color);
			}
		}
 }

 /**************************************************************************//**
 * @fn            void LCD_setScreen(uint32_t color)
 * @brief        Draw the entire screen to a color
 * @note
 *****************************************************************************/
 void LCD_setScreen(uint32_t color)
 {
     LCD_drawBlock(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
 }

 /**************************************************************************//**
 * @fn            void LCD_drawString(uint8_t x, uint8_t y, char* str, uint32_t fg, uint32_t bg)
 * @brief        Draw a string starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 void LCD_drawString(short x, short y, char* str, uint32_t fg, uint32_t bg)
 {
     int i = 0;
     while (str[i] != '\0') {
         LCD_drawChar(x + (i * CHAR_WIDTH), y, str[i], fg, bg);
         i++;
     }
 }

 void LCD_drawStringFast(short x, short y, char* str, uint32_t fg, const uint16_t bitmap[])
 {
     int i = 0;
     while (str[i] != '\0') {
         LCD_drawCharFast(x + (i * CHAR_WIDTH), y, str[i], fg, bitmap);
         i++;
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

/*
This is a function to draw a play button.
A play button will be in the shape of an equilateral triangle
with the left vertical side start from the coordinates (top_x, top_y)
and have a height height. That is the bottom coordinates of this line
are (top_x, top_y - height). The third side is predetermined
*/

int LCD_DrawPlayButton(short top_x, short top_y, short height, short playing, uint32_t color) {
	if (playing) {
		LCD_drawLine(top_x, top_y, top_x, top_y - height, color);
		LCD_drawLine(top_x, top_y, top_x + (short) (0.866 * height), top_y - height/2, color);
		LCD_drawLine(top_x, top_y - height, top_x + (short) (0.866 * height), top_y - height/2, color);
	} else {
		LCD_drawLine(top_x, top_y, top_x, top_y - height, color);
		LCD_drawLine(top_x + 8, top_y, top_x + 8, top_y - height, color);
	}

    return 0;
}

int LCD_DrawNextButton(short top_x, short top_y, short height, uint32_t color) {
    short delta = 10;
    LCD_drawLine(top_x, top_y, top_x + (short) (0.866 * height), top_y - height/2, color);
    LCD_drawLine(top_x, top_y - height, top_x + (short) (0.866 * height), top_y - height/2, color);
    LCD_drawLine(top_x + delta, top_y, top_x + (short) (0.866 * height) + delta, top_y - height/2, color);
    LCD_drawLine(top_x + delta, top_y - height, top_x + (short) (0.866 * height) + delta, top_y - height/2, color);

    return 0;
}

int LCD_DrawBackButton(short top_x, short top_y, short height, uint32_t color) {
    short delta = 10;
    LCD_drawLine(top_x, top_y, top_x - (short) (0.866 * height), top_y - height/2, color);
    LCD_drawLine(top_x, top_y - height, top_x - (short) (0.866 * height), top_y - height/2, color);
    LCD_drawLine(top_x + delta, top_y, top_x - (short) (0.866 * height) + delta, top_y - height/2, color);
    LCD_drawLine(top_x + delta, top_y - height, top_x - (short) (0.866 * height) + delta, top_y - height/2, color);

    return 0;
}

int LCD_DrawVolumeUpButton(short x, short y, short height, uint32_t color) {
    LCD_drawLine(x + height / 2, y, x - height / 2, y, color);
    LCD_drawLine(x, y - height / 2, x, y + height / 2, color);

    return 0;
}

int LCD_DrawVolumeDownButton(short x, short y, short height, uint32_t color) {
    LCD_drawLine(x - height / 2, y, x + height / 2, y, color);

    return 0;
}

void LCD_DrawBitMap(uint16_t height, uint16_t width, const uint16_t bitmap[]) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint16_t color = bitmap[y * width + x];  // Get the pixel color in RGB565 format
            LCD_drawPixel(x, y, color);  // Draw the pixel using the RGB565 color format
        }
    }
}

void LCD_DrawBitMapFast(uint16_t height, uint16_t width, const uint16_t bitmap[]) {
    LCD_setAddr(0, 0, width  - 1,  height - 1);
    for (uint16_t y = 0; y < height; ++y) {
        for (uint16_t x = 0; x < width;  ++x) {
            uint16_t color = bitmap[y * LCD_WIDTH + x];
            SPI_ControllerTx_16bit(color);
        }
    }
}

int LCD_RenderSongList(int is_horizontal, SongEntry *song_list, int file_count, uint8_t selected_index, uint32_t selected_color, uint32_t default_color , const uint16_t bitmap[])
{
    // Horizontal uses NUMBER_OF_SONGS slots; vertical uses 7 slots
    const int horizSlots = NUMBER_OF_SONGS;
    const int vertSlots = 7;
    int slots = is_horizontal ? horizSlots : vertSlots;

    int total = file_count;
    int start = 0;
    // Ensure the highlighted song is always visible by computing a window start
    if (total > slots)
    {
        // Center selection if possible
        start = selected_index - (slots / 2);
        if (start < 0)
            start = 0;
        if (start > total - slots)
            start = total - slots;
    }
    int display_count = (total - start < slots) ? (total - start) : slots;

    if (is_horizontal)
    {
        // Borders
        plotLineFast(20, 20, 20, 20 + 15 * display_count, default_color, bitmap);
        plotLineFast(35, 20, 35, 20 + 15 * display_count, default_color, bitmap); // shifted from 30 to 35
        plotLineFast(140, 20, 140, 20 + 15 * display_count, default_color, bitmap);
        plotLineFast(20, 20, 140, 20, default_color, bitmap);
        plotLineFast(20, 20 + 15 * display_count, 140, 20 + 15 * display_count, default_color, bitmap);

        for (int i = 0; i < display_count; ++i)
        {
            int idx = start + i;
            int y = 20 + 15 * i;
            bool is_sel = (idx == selected_index);
            uint32_t line_color = is_sel ? selected_color : default_color;

            // Row separator
            plotLineFast(20, y, 140, y, line_color, bitmap);
            // Highlight borders if selected
            if (is_sel)
            {
            	plotLineFast(20, y, 20, y + 15, selected_color, bitmap);
            	plotLineFast(35, y, 35, y + 15, selected_color, bitmap); // shifted
                plotLineFast(140, y, 140, y + 15, selected_color, bitmap);
                // Bottom border for selected cell
                plotLineFast(20, y + 15, 140, y + 15, selected_color, bitmap);
            }

            // Draw index and song title
            char idx_str[12];
            snprintf(idx_str, sizeof(idx_str), "%d", idx + 1);
            LCD_drawStringFast(23, y + 3, idx_str,
                           is_sel ? selected_color : default_color,
                        		   bitmap);
            // Shift song title right by 5 more pixels (from 40 remains)
            LCD_drawStringFast(40, y + 3, song_list[idx].filename,
                           is_sel ? selected_color : default_color,
                        		   bitmap);
        }
    }
    else
    {
        // Vertical list with up to 7 slots
    	plotLineFast(20, 20, 20, 20 + 15 * display_count, default_color, bitmap);
    	plotLineFast(35, 20, 35, 20 + 15 * display_count, default_color, bitmap); // shifted
        plotLineFast(108, 20, 108, 20 + 15 * display_count, default_color, bitmap);
        plotLineFast(20, 20, 108, 20, default_color, bitmap);
        plotLineFast(20, 20 + 15 * display_count, 108, 20 + 15 * display_count, default_color, bitmap);

        for (int i = 0; i < display_count; ++i)
        {
            int idx = start + i;
            int y = 20 + 15 * i;
            bool is_sel = (idx == selected_index);
            uint32_t line_color = is_sel ? selected_color : default_color;

            // Row separator
            plotLineFast(20, y, 108, y, line_color, bitmap);
            // Highlight borders if selected
            if (is_sel)
            {
            	plotLineFast(20, y, 20, y + 15, selected_color, bitmap);
            	plotLineFast(35, y, 35, y + 15, selected_color, bitmap); // shifted
            	plotLineFast(108, y, 108, y + 15, selected_color, bitmap);
                // Bottom border for selected cell
            	plotLineFast(20, y + 15, 108, y + 15, selected_color, bitmap);
            }
            if (idx == selected_index + 1)
            {
            	plotLineFast(20, y, 108, y, selected_color, bitmap);
            }

            // Draw index and title
            char idx_str[12];
            snprintf(idx_str, sizeof(idx_str), "%d", idx + 1);
            LCD_drawStringFast(23, y + 3, idx_str,
                           is_sel ? selected_color : default_color,
                        		   bitmap);
            // Shift song title right by 5 more pixels
            LCD_drawStringFast(40, y + 3, song_list[idx].filename,
                           is_sel ? selected_color : default_color,
                        		   bitmap);
        }
    }

    return 0;
}

void LCD_drawWaveformAnimation(short x, short y, int barCount, int barWidth, int barSpacing, int maxHeight, uint32_t color)
{
    static int heights[256];
    static bool initialized = false;
    static bool seeded = false;
    static int frame = 0;

    if (!seeded) {
            // seed from the current millisecond tick
            srand((unsigned)HAL_GetTick());
            seeded = true;
        }
    if (!initialized)
    {
        for (int i = 0; i < barCount; ++i)
            heights[i] = rand() % (maxHeight + 1);
        initialized = true;
    }
    // Only update heights when playing
    if (is_playing)
    {
        frame++;
        if (frame % 3 == 0)
        {
            int maxDelta = maxHeight / 8 + 1;
            for (int i = 0; i < barCount; ++i)
            {
                int delta = (rand() % (2 * maxDelta + 1)) - maxDelta;
                int h = heights[i] + delta;
                heights[i] = (h < 0 ? 0 : (h > maxHeight ? maxHeight : h));
            }
        }
    }
    // Draw bars
    for (int i = 0; i < barCount; ++i)
    {
        int barX = x + i * (barWidth + barSpacing);
        int h = heights[i];
        for (int dx = 0; dx < barWidth; ++dx)
        {
            LCD_drawLine(barX + dx, y, barX + dx, y - h, color);
        }
    }
}

void LCD_drawScrollingString(short y, const char *str, uint32_t fg, uint32_t bg, int boxWidth, int speed)
{
    static int offset = 0;
    int len = strlen(str);
    int textWidth = len * CHAR_WIDTH;
    int startX = (SCREEN_WIDTH - boxWidth) / 2;

    if (textWidth <= boxWidth)
    {
        int centerX = (SCREEN_WIDTH - textWidth) / 2;
        LCD_drawString(centerX, y, (char *)str, fg, bg);
        return;
    }

    offset = (offset + speed) % (textWidth + boxWidth);
    int drawX = startX + boxWidth - offset;
    LCD_drawString(drawX, y, (char *)str, fg, bg);
    if (drawX + textWidth < startX + boxWidth)
    {
        LCD_drawString(drawX + textWidth + 1, y, (char *)str, fg, bg);
    }
}

void toggle_play(void) {
    if (is_playing == 1) {
        is_playing = 0;
    } else {
        is_playing = 1;
    }
}


