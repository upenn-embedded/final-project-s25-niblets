/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
* @attention
*
* Copyright (c) 2025 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/
#include "main.h"
#include "fatfs.h"
#include "image_data.h"
#include "LCD_GFX.h"
#include "ST7735.h"
#include <stdbool.h>
#include <stdio.h>
#include "image_data.h"
#include <unistd.h>
#include <time.h>
#include <string.h>

#define FLUORESCENT_BLUE rgb(0, 128, 0)
#define WHITE rgb(255, 255, 255)
#define ARTIST_LEN 32
#define NAME_LEN 128
#define MAX_FILES 64
#define NUMBER_OF_DISPLAYED_SONGS 5
#define NUMBER_OF_FILES 200
#define START_SCREEN 0
#define SELECT_SONG_SCREEN 1
#define CURRENT_SONG_PLAYING_SCREEN 2
#define LONG_PRESS_MS 1000
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))



I2S_HandleTypeDef hi2s2;
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_spi2_tx;
int16_t samples[32000];
TCHAR readBuf[90000];
FATFS FatFs;
FIL fil;
FRESULT fres;
UINT br;
DWORD chunkSize;
char   chunkID[4];
typedef enum {
	UNKNOWN,
	HALF_COMPLETED,
	FULL_COMPLETED
} CallBack_Result_t;
typedef enum {
	HOME,
	PLAYLIST,
	PAUSE,
	PLAYING,
} PodState_t;
#define ARTIST_LEN 32
#define NAME_LEN 128
#define MAX_FILES 64
#define NUMBER_OF_DISPLAYED_SONGS 5
typedef struct {
   char artist[ARTIST_LEN];
   char filename[NAME_LEN];
   uint32_t length;
} SongEntry_t;

static void MX_I2S2_Init(void);
static void MX_SPI3_Init(void);
static void MX_DMA_Init(void);
static void gpio_clock_enable(void);
static void MX_GPIO_I2S2_Init(void);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void renderScreen(int isHorizontal, uint8_t current_screen_status);
void populateSongList(void);

PodState_t current_state = HOME;
CallBack_Result_t callback_result = UNKNOWN;
volatile bool home_drawn = false;
volatile bool sd_card_err = false;
volatile bool playlist_not_drawn = true;
volatile bool play = false;
volatile bool song_not_started = true;

volatile bool playButtonPressed  = false;
volatile bool prevButtonPressed  = false;
volatile bool nextButtonPressed  = false;
volatile bool tim2_tick = false;

uint16_t txData;
uint8_t song_index = 0;
uint32_t fread_size     = 0;
uint32_t recording_size = 0;
uint32_t played_size    = 0;
int file_count = 0;
extern int is_playing;
uint8_t current_screen_status = 1;
SongEntry song_list[NUMBER_OF_FILES];
int active_song_idx = 0;
int prev_song_idx = -1;
char* playing_message = "Now playing ...";
int current_number_of_songs_stored = 5;
int isHorizontal;
int num_songs_in_sd = 5;
volatile uint32_t play_press_time;
volatile bool play_irq_fired;
volatile uint32_t play_button_press_time;



void rotationDecider(void){
	int16_t gx = 0, gy = 0, gz = 0;
	float sensitivity = 0.00875f;
	gyro_read(&gx, &gy, &gz);
	float angular_rate_x = gx * sensitivity;
	float angular_rate_y = gy * sensitivity;

	int rate_x = (int)(angular_rate_x * 100);
	int rate_y = (int)(angular_rate_y * 100);
	printf("Running!");
	if (rate_x > 20000){
//		LCD_rotate(2);
		printf("Rotate 2");
	} else if (rate_x < -20000) {
//		LCD_rotate(0);
		printf("Rotate 0");
	} else if (rate_y > 20000) {
//		LCD_rotate(1);
		printf("Rotate 1");
	} else if (rate_y < -20000){
//		LCD_rotate(3);
		printf("Rotate 3");
	}
}

void initialize(void) {
	HAL_Init();
	setSysClockTo160MHz();
	gpio_clock_enable();
	MX_DMA_Init();
	__HAL_DMA_ENABLE_IT(&hdma_spi2_tx, DMA_IT_HT);
	__HAL_DMA_ENABLE_IT(&hdma_spi2_tx, DMA_IT_TC);
	lcd_init();
	uart_init();
	MX_GPIO_I2S2_Init();
	MX_SPI3_Init();
	MX_I2S2_Init();
	MX_FATFS_Init();
	pb3_exti_init();
	pb5_exti_init();
	pa10_exti_init();
	LCD_setScreen(rgb(00,00,00));
	delay_ms(1000);
	printf("\r\n~ Nibble Pod starting ~\r\n\r\n");
}
int main(void) {
 initialize();
 int retries = 0;
 play_button_press_time = 0;
  do {
	   fres = f_mount(&FatFs, "", 1);
	   if (fres != FR_OK) {
		   sd_card_err = true;
		   break;
	   } else {
		   sd_card_err = false;
	   }
	   retries++;
	   delay_ms(500);
  } while (retries < 20);
  file_count = current_number_of_songs_stored;
  bool running = true;
  populateSongList();
  isHorizontal = 1;
  LCD_drawLine(10, 10, 10, 50, WHITE);

  while (running) {
	   switch (current_state) {
	   	   case HOME:
				if (!home_drawn) {
					   LCD_DrawBitMapFast(128, 160, image_data);
					   LCD_drawStringFast(40, 50, "hi there!", 65535, image_data);
					   home_drawn = true;
				}

				if (playButtonPressed) {
	   				current_state = PLAYLIST;
	   				playButtonPressed = false;
	   				printf("play pressed switching to playlist !! \r\n");
				}
	   		   break;
	   	   case PLAYLIST:
	   		   if (playlist_not_drawn) {
	   			playlist_not_drawn = false;
				LCD_DrawBitMapFast(128, 160, image_data);
	   			LCD_RenderSongList(isHorizontal, song_list, current_number_of_songs_stored, active_song_idx, FLUORESCENT_BLUE, WHITE, image_data);
	   		   }

	   		   if (nextButtonPressed) {
	              	active_song_idx = min(num_songs_in_sd, active_song_idx + 1);
	              	song_index +=1;
	              	song_index %= num_songs_in_sd;
	              	playlist_not_drawn = true;
	              	nextButtonPressed = false;
	              	printf("next working in the fsm\r\n");
	   		   }

	   		 if (playButtonPressed) {
					current_state = PLAYING;
					song_not_started = true;
					printf("pressed play button \r\n");
					printf("Switch to Playing from Playlist \r\n");
	   			}

	   		 if (prevButtonPressed) {
	   			prev_song_idx = active_song_idx;
	   			active_song_idx = max(0, active_song_idx - 1);
	   			song_index -=1;
	   			song_index %= num_songs_in_sd;
	   			playlist_not_drawn = true;
	   			prevButtonPressed = false;
	   			printf("prev in the fsm playlist \r\n");
	   			}
	   		   break;
	   	   case PLAYING:
	   		   if (song_not_started) {
	   			   song_not_started = false;
	   			   renderScreen(1, 2);
	   			   switch (song_index) {
					   case 0:
						   printf("IMAGINE DRAGONS!!");
						   fres = f_open(&fil, "imagine.wav", FA_READ);
						   f_lseek(&fil, 12);
						   if (fres != FR_OK) {
						   	   printf("f_open error (%i)\r\n", fres);
						   	   while(1);
						   	 }
						   while (1) {
						   	    // read 4-byte ID + 4-byte size
						   	   	f_read(&fil, chunkID, 4, &br);
						   	    f_read(&fil, &chunkSize, 4, &br);
						   	    if (br != 4) {
						   	    	printf("Unexpected EOF while scanning chunks\r\n");
						   	    	break;
						   	   	}
						   	    if (memcmp(chunkID, "data", 4) == 0) {
						   	   		printf("Found “data” chunk at offset %lu, size = %lu bytes\r\n",
						   	   			  (unsigned long)f_tell(&fil) - 4,(unsigned long)chunkSize);
						   	   			   break;
						   	   	}
						   	   	// skip over this entire chunk (size may be odd—RFC says pad to even)
						   	   	f_lseek(&fil, f_tell(&fil) + chunkSize + (chunkSize & 1));
						   	   	}
						   		recording_size = chunkSize / 2;
						   		f_read(&fil,samples,64000,(UINT *) fread_size);
						   		HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t *) samples, 32000);
						   		printf("opened the wav and the first 640000 bytes file\r\n");
						   break;
					   case 1:
						   printf("KUDADE!!");
						   fres = f_open(&fil, "kudade.wav", FA_READ);
						   f_lseek(&fil, 12);
						   if (fres != FR_OK) {
						   	   printf("f_open error (%i)\r\n", fres);
						   	   while(1);
						   	 }
						   while (1) {
						   	    // read 4-byte ID + 4-byte size
						   	   	f_read(&fil, chunkID, 4, &br);
						   	    f_read(&fil, &chunkSize, 4, &br);
						   	    if (br != 4) {
						   	    	printf("Unexpected EOF while scanning chunks\r\n");
						   	    	break;
						   	   	}
						   	    if (memcmp(chunkID, "data", 4) == 0) {
						   	   		printf("Found “data” chunk at offset %lu, size = %lu bytes\r\n",
						   	   			  (unsigned long)f_tell(&fil) - 4,(unsigned long)chunkSize);
						   	   			   break;
						   	   	}
						   	   	// skip over this entire chunk (size may be odd—RFC says pad to even)
						   	   	f_lseek(&fil, f_tell(&fil) + chunkSize + (chunkSize & 1));
						   	   	}
						   		recording_size = chunkSize / 2;
						   		f_read(&fil,samples,64000,(UINT *) fread_size);
						   		HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t *) samples, 32000);
						   		printf("opened the wav and the first 640000 bytes file\r\n");
						   break;
					   case 2:
						   printf("NEVER!!!");
						   fres = f_open(&fil, "never.wav", FA_READ);
						   f_lseek(&fil, 12);
						   if (fres != FR_OK) {
						   	   printf("f_open error (%i)\r\n", fres);
						   	   while(1);
						   	 }
						   while (1) {
						   	    // read 4-byte ID + 4-byte size
						   	   	f_read(&fil, chunkID, 4, &br);
						   	    f_read(&fil, &chunkSize, 4, &br);
						   	    if (br != 4) {
						   	    	printf("Unexpected EOF while scanning chunks\r\n");
						   	    	break;
						   	   	}
						   	    if (memcmp(chunkID, "data", 4) == 0) {
						   	   		printf("Found “data” chunk at offset %lu, size = %lu bytes\r\n",
						   	   			  (unsigned long)f_tell(&fil) - 4,(unsigned long)chunkSize);
						   	   			   break;
						   	   	}
						   	   	// skip over this entire chunk (size may be odd—RFC says pad to even)
						   	   	f_lseek(&fil, f_tell(&fil) + chunkSize + (chunkSize & 1));
						   	   	}
						   		recording_size = chunkSize / 2;
						   		f_read(&fil,samples,64000,(UINT *) fread_size);
						   		HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t *) samples, 32000);
						   		printf("opened the wav and the first 640000 bytes file\r\n");
						   break;
					   case 3:
						   printf("NLU!!!");
						   fres = f_open(&fil, "nlu.wav", FA_READ);
						   f_lseek(&fil, 12);
						   if (fres != FR_OK) {
						   	   printf("f_open error (%i)\r\n", fres);
						   	   while(1);
						   	 }
						   while (1) {
						   	    // read 4-byte ID + 4-byte size
						   	   	f_read(&fil, chunkID, 4, &br);
						   	    f_read(&fil, &chunkSize, 4, &br);
						   	    if (br != 4) {
						   	    	printf("Unexpected EOF while scanning chunks\r\n");
						   	    	break;
						   	   	}
						   	    if (memcmp(chunkID, "data", 4) == 0) {
						   	   		printf("Found “data” chunk at offset %lu, size = %lu bytes\r\n",
						   	   			  (unsigned long)f_tell(&fil) - 4,(unsigned long)chunkSize);
						   	   			   break;
						   	   	}
						   	   	// skip over this entire chunk (size may be odd—RFC says pad to even)
						   	   	f_lseek(&fil, f_tell(&fil) + chunkSize + (chunkSize & 1));
						   	   	}
						   		recording_size = chunkSize / 2;
						   		f_read(&fil,samples,64000,(UINT *) fread_size);
						   		HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t *) samples, 32000);
						   		printf("opened the wav and the first 640000 bytes file\r\n");
						   break;
					   case 4:
						   printf("PTS!!!");
						   fres = f_open(&fil, "pts.wav", FA_READ);
						   f_lseek(&fil, 12);
						   if (fres != FR_OK) {
						   	   printf("f_open error (%i)\r\n", fres);
						   	   while(1);
						   	 }
						   while (1) {
						   	    // read 4-byte ID + 4-byte size
						   	   	f_read(&fil, chunkID, 4, &br);
						   	    f_read(&fil, &chunkSize, 4, &br);
						   	    if (br != 4) {
						   	    	printf("Unexpected EOF while scanning chunks\r\n");
						   	    	break;
						   	   	}
						   	    if (memcmp(chunkID, "data", 4) == 0) {
						   	   		printf("Found “data” chunk at offset %lu, size = %lu bytes\r\n",
						   	   			  (unsigned long)f_tell(&fil) - 4,(unsigned long)chunkSize);
						   	   			   break;
						   	   	}
						   	   	// skip over this entire chunk (size may be odd—RFC says pad to even)
						   	   	f_lseek(&fil, f_tell(&fil) + chunkSize + (chunkSize & 1));
						   	   	}
						   		recording_size = chunkSize / 2;
						   		f_read(&fil,samples,64000,(UINT *) fread_size);
						   		HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t *) samples, 32000);
						   		printf("opened the wav and the first 640000 bytes file\r\n");
						   break;

					   default:
						   fres = f_open(&fil, "billy.wav", FA_READ);
						   break;
	   			   	   }
	   		   	   }

					 if(callback_result == HALF_COMPLETED) {
							f_read(&fil, samples, 32000, (UINT *)fread_size);
							callback_result = UNKNOWN;
						}

					 if (callback_result == FULL_COMPLETED) {
							f_read(&fil, &samples[16000], 32000, (UINT *)fread_size);
							callback_result = UNKNOWN;
					 }

					 if (played_size >= recording_size) {
						 	f_close(&fil);
							HAL_I2S_DMAStop(&hi2s2);
							MX_DMA_Init();
							__HAL_DMA_ENABLE_IT(&hdma_spi2_tx, DMA_IT_HT);
							__HAL_DMA_ENABLE_IT(&hdma_spi2_tx, DMA_IT_TC);
							current_state = HOME;
							played_size = 0;
							recording_size = 0;
							song_not_started = true;
							printf("DONE\r\n");
					 }

					 if (nextButtonPressed) {
					 	    active_song_idx = min(num_songs_in_sd, active_song_idx + 1);
					 	    f_close(&fil);
					 	    HAL_I2S_DMAStop(&hi2s2);
					 	    song_index +=1;
					 	    song_index %= num_songs_in_sd;
					 	    playlist_not_drawn = true;
					 	    nextButtonPressed = false;
					 	    delay_ms(100);
					 	    current_state = PLAYING;
					 	    song_not_started = true;
					 	    printf("next working in the fsm\r\n");
					 	   		   }

					if (prevButtonPressed) {
					 	prev_song_idx = active_song_idx;
					 	active_song_idx = max(0, active_song_idx - 1);
					 	f_close(&fil);
					    HAL_I2S_DMAStop(&hi2s2);
					 	song_index -=1;
					 	song_index %= num_songs_in_sd;
					 	playlist_not_drawn = true;
					 	prevButtonPressed = false;
					 	delay_ms(100);
					 	current_state = PLAYING;
					    song_not_started = true;
					 	printf("prev in the fsm playing \r\n");
					 }
	   		 break;
	   	   case PAUSE:
					if (prevButtonPressed) {
					 	prev_song_idx = active_song_idx;
					 	active_song_idx = max(0, active_song_idx - 1);
					 	song_index -=1;
					 	song_index %= num_songs_in_sd;
					 	playlist_not_drawn = true;
					 	prevButtonPressed = false;
					 	EXTI->PR = (1U << 3);
					 	delay_ms(100);
					 	current_state = PLAYING;
					    song_not_started = true;
					 	printf("prev in the fsm playing \r\n");
					 }

					 if (nextButtonPressed) {
						active_song_idx = min(num_songs_in_sd, active_song_idx + 1);
						song_index +=1;
						song_index %= num_songs_in_sd;
						playlist_not_drawn = true;
						nextButtonPressed = false;
						current_state = PLAYING;
						song_not_started = true;
						printf("next working in the fsm\r\n");
						}
	   		    break;
	   	   default:
	   		   break;
	   }
  }
  return (0);
}
void renderScreen(int isHorizontal, uint8_t current_screen_status) {
   switch (current_screen_status) {
       case 0:
       	if (!home_drawn) {
				LCD_DrawBitMapFast(128, 160, image_data);
				LCD_drawStringFast(40, 50, "hi there!", 65535, image_data);
				if (sd_card_err) {
					LCD_drawStringFast(30, 70, "SD card not detected, no music :(", 65535, image_data);
				} else {
					LCD_drawStringFast(30, 90, "Press OK to start :)", 65535, image_data);
				}
				home_drawn = true;
			   }
           break;
       case 1:
           if (isHorizontal) {
               LCD_DrawBitMapFast(128, 160, image_data);
               LCD_DrawPlayButton(75, 115, 10, !is_playing, FLUORESCENT_BLUE);
               LCD_DrawNextButton(100, 115, 10, FLUORESCENT_BLUE);
               LCD_DrawBackButton(50, 115, 10, FLUORESCENT_BLUE);
               LCD_RenderSongList(isHorizontal, song_list, NUMBER_OF_DISPLAYED_SONGS, active_song_idx, FLUORESCENT_BLUE, WHITE, image_data);
           } else {
           	LCD_rotate(0);
               LCD_DrawBitMapFast(160, 128, image_data);
               LCD_DrawPlayButton(59, 160, 10, !is_playing, FLUORESCENT_BLUE);
               LCD_DrawNextButton(74, 160, 10, FLUORESCENT_BLUE);
               LCD_DrawBackButton(44, 160, 10, FLUORESCENT_BLUE);
               LCD_RenderSongList(isHorizontal, song_list, NUMBER_OF_DISPLAYED_SONGS + 2, active_song_idx, FLUORESCENT_BLUE, WHITE, image_data);
           }
           current_screen_status = 2;
           break;
       case 2:
           // Replace the images below with the desired images for the current screen state
           if (isHorizontal) {
               LCD_DrawBitMapFast(128, 160, image_data);
               LCD_DrawPlayButton(85, 115, 10, !is_playing, FLUORESCENT_BLUE);
               LCD_DrawNextButton(110, 115, 10, FLUORESCENT_BLUE);
               LCD_DrawBackButton(60, 115, 10, FLUORESCENT_BLUE);
               LCD_drawString(20, 25, song_list[active_song_idx].filename, FLUORESCENT_BLUE, rgb(0, 0, 0));

           } else {
               LCD_DrawBitMapFast(160, 128, image_data);
               LCD_DrawPlayButton(59, 160, 10, !is_playing, FLUORESCENT_BLUE);
               LCD_DrawNextButton(74, 160, 10, FLUORESCENT_BLUE);
               LCD_DrawBackButton(44, 160, 10, FLUORESCENT_BLUE);
               LCD_drawString(20, 25, song_list[active_song_idx].filename, FLUORESCENT_BLUE, rgb(0, 0, 0));

           }
           break;
   }
}


// next
void EXTI15_10_IRQHandler(void) {
   if (EXTI->PR & (1U << 10)) {
       EXTI->PR = (1U << 10);
       nextButtonPressed = true;
       delay_ms(200);
   }
}

// play
void EXTI3_IRQHandler(void) {
   if (EXTI->PR & (1U << 3)) {
       EXTI->PR = (1U << 3);

    	   playButtonPressed = true;
    	   delay_ms(200);
   }
}
// previous
void EXTI9_5_IRQHandler(void) {
   if (EXTI->PR & (1U << 5)) {
       EXTI->PR = (1U << 5);
       prevButtonPressed = true;
       delay_ms(200);
       }
}


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	callback_result = HALF_COMPLETED;
}
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){
	callback_result = FULL_COMPLETED;
	played_size += 32000;
}
void populateSongList(void) {
   strncpy(song_list[0].filename,  "Thunder - Imagine Dragons",     NAME_LEN);
   song_list[0].length  = 210000;
   strncpy(song_list[1].filename,  "Kudade",      NAME_LEN);
   song_list[1].length  = 320000;
   strncpy(song_list[2].filename,  "Never Gonna Give You Up",     NAME_LEN);
   song_list[2].length  = 150000;
   strncpy(song_list[3].filename,  "Not Like Us",     NAME_LEN);
   song_list[3].length  = 180000;
   strncpy(song_list[4].filename,  "Push to Start",     NAME_LEN);
   song_list[4].length  = 250000;
}
static void MX_I2S2_Init(void)
{
hi2s2.Instance = SPI2;
hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_32K;
hi2s2.Init.CPOL = I2S_CPOL_LOW;
hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
if (HAL_I2S_Init(&hi2s2) != HAL_OK)
{
  Error_Handler();
}
}
static void MX_DMA_Init(void)
{
 __HAL_RCC_DMA1_CLK_ENABLE();
 HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
 HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}
static void MX_SPI3_Init(void)
{
hspi3.Instance = SPI3;
hspi3.Init.Mode = SPI_MODE_MASTER;
hspi3.Init.Direction = SPI_DIRECTION_2LINES;
hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
hspi3.Init.NSS = SPI_NSS_SOFT;
hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
hspi3.Init.CRCPolynomial = 10;
if (HAL_SPI_Init(&hspi3) != HAL_OK)
{
  Error_Handler();
}
}
static void gpio_clock_enable(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |=RCC_AHB1ENR_GPIOCEN;
}
static void MX_GPIO_I2S2_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitTypeDef gp = {0};
  gp.Mode      = GPIO_MODE_AF_PP;
  gp.Pull      = GPIO_NOPULL;
  gp.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gp.Alternate = GPIO_AF5_SPI2;
  gp.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &gp);
  gp.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &gp);
}
void Error_Handler(void)
{
__disable_irq();
while (1)
{
}
}
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{}
#endif
