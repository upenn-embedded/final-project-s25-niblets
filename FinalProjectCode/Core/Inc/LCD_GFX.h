/*
 * LCD_GFX.h
 *
 * Adapted from LCD_GFX.h for LCD rendering
 */

 #ifndef LCD_GFX_H_
 #define LCD_GFX_H_

 #include <stdint.h>
 #include <stdbool.h>
 #include <string.h>
#include <stdio.h>

#define ARTIST_LEN 32
#define NAME_LEN 128
#define MAX_FILES 64

typedef struct SongEntry {
    char artist[ARTIST_LEN];
    char filename[NAME_LEN];
    uint32_t length;
} SongEntry;

 static const char ASCII[96][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00} // 20  (space)
	,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
	,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
	,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
	,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
	,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
	,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
	,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
	,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
	,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
	,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
	,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
	,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
	,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
	,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
	,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
	,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
	,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
	,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
	,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
	,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
	,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
	,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
	,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
	,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
	,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
	,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
	,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
	,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
	,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
	,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
	,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
	,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
	,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
	,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
	,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
	,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
	,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
	,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
	,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
	,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
	,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
	,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
	,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
	,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
	,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
	,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
	,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
	,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
	,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
	,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
	,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
	,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
	,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
	,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
	,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
	,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
	,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
	,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
	,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
	,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
	,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
	,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
	,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
	,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
	,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
	,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
	,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
	,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
	,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
	,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
	,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
	,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
	,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
	,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
	,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
	,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
	,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
	,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
	,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
	,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
	,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
	,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
	,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
	,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
	,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
	,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
	,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
	,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
	,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
	,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
	,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
	,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
	,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
	,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ?
	,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f ?
};

 // Screen dimensions
 #define SCREEN_WIDTH 160
 #define SCREEN_HEIGHT 128

 // Function declarations
 bool LCD_init(void);
 void LCD_cleanup(void);
 void LCD_updateScreen(void);

 uint32_t rgb(uint8_t red, uint8_t green, uint8_t blue);
 uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue);
 uint32_t rgb565_to_argb8888(uint16_t color);

 void plotLineFast(short x0, short y0, short x1, short y1, uint32_t c, const uint16_t bitmap[]);
 void LCD_drawPixel(short x, short y, uint32_t color);
 void LCD_drawPixel_565(short x, short y, uint16_t color);
 void LCD_drawChar(short x, short y, uint16_t character, uint32_t fColor, uint32_t bColor);
 void LCD_drawCharFast(short x, short y, uint16_t character, uint32_t fColor, const uint16_t bitmap[]);
 void LCD_drawCircle(short x0, short y0, short radius, uint32_t color);
 void LCD_drawLine(short x0, short y0, short x1, short y1, uint32_t c);
 void LCD_drawBlock(short x0, short y0, short x1, short y1, uint32_t color);
 void LCD_setScreen(uint32_t color);
 void LCD_drawString(short x, short y, char* str, uint32_t fg, uint32_t bg);
 void LCD_drawStringFast(short x, short y, char* str, uint32_t fg, const uint16_t bitmap[]);
 void LCD_drawButton(short x, short y, char * label);
 void LCD_DrawBitMap(uint16_t height, uint16_t width, const uint16_t bitmap[]);
 void LCD_DrawBitMapFast(uint16_t height, uint16_t width, const uint16_t bitmap[]);
 int LCD_DrawPlayButton(short top_x, short top_y, short height, short playing, uint32_t color);
 int LCD_DrawNextButton(short top_x, short top_y, short height, uint32_t color);
 int LCD_DrawBackButton(short top_x, short top_y, short height, uint32_t color);
 int LCD_DrawVolumeUpButton(short x, short y, short height, uint32_t color);
 int LCD_DrawVolumeDownButton(short x, short y, short height, uint32_t color);
 int LCD_RenderSongList(int is_horizontal, SongEntry *song_list, int file_count, uint8_t selected_index, uint32_t selected_color, uint32_t default_color , const uint16_t bitmap[]);
 void LCD_drawWaveformAnimation(short x, short y, int barCount, int barWidth, int barSpacing, int maxHeight, uint32_t color);
 void LCD_drawScrollingString(short y, const char* str, uint32_t fg, uint32_t bg, int boxWidth, int speed);
 void toggle_play(void);
 #endif /* LCD_GFX_H_ */

