/**************************************************************************//**
* @file        ST7735.c
* @ingroup 	   ST7735
* @brief       Basic display driver for Adafruit 358 1.8" TFT LCD with ST7735R chip
*
* @details     Basic display driver for Adafruit 358 1.8" TFT LCD with ST7735R chip
*
*
* @copyright
* @author	   J. Ye
* @date        April 19, 2021
* @version		1.0
*****************************************************************************/

#include "main.h"
#include "ST7735.h"
#include <string.h>   // for strlen, memset, etc.
#include <stdio.h>    // for sprintf


#define SPI1EN            (1U<<12)
#define SPI4EN			  (1U<<13)
#define GPIOAEN            (1U<<0)
#define SR_TXE            (1U<<1)
#define SR_RXNE            (1U<<0)
#define SR_BSY            (1U<<7)

/******************************************************************************
* Local Functions
******************************************************************************/

/**************************************************************************//**
* @fn			static void lcd_pin_init(void)
* @brief		Initialize SPI for LCD
* @note
*****************************************************************************/
static void lcd_pin_init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Set PA5,PA6,PA7 mode to alternate function*/
    /*PA5 This is our SCK line*/
    GPIOA->MODER &=~(1U<<10);
    GPIOA->MODER |=(1U<<11);
    /*PA7 This is our MOSI line*/
    GPIOA->MODER &=~(1U<<14);
    GPIOA->MODER |=(1U<<15);

    /*Set PA8 as output pin - use as our D/C line*/
    GPIOA->MODER |=(1U<<16);
    GPIOA->MODER &=~(1U<<17);

    /*Set PA9 as output pin - use as our chip select line*/
    GPIOA->MODER |=(1U<<18);
    GPIOA->MODER &=~(1U<<19);

    /* Set PB4 as an output pin - use as our LCD reset line */
    GPIOB->MODER |=(1U<<8);
    GPIOB->MODER &=~(1U<<9);

    /* Set PB10 as an output pin - use as our LCD LITE line */
    GPIOB->MODER |=(1U<<20);
    GPIOB->MODER &=~(1U<<21);

    //Set PA5,PA6,PA7 alternate function type to SPI1
    //PA5 SCK Line
    GPIOA->AFR[0] |=(1U<<20);
    GPIOA->AFR[0] &= ~(1U<<21);
    GPIOA->AFR[0] |=(1U<<22);
    GPIOA->AFR[0] &= ~(1U<<23);
    //PA7 MOSI Line
    GPIOA->AFR[0] |=(1U<<28);
    GPIOA->AFR[0] &= ~(1U<<29);
    GPIOA->AFR[0] |=(1U<<30);
    GPIOA->AFR[0] &= ~(1U<<31);

	//Enable LCD by setting RST high
	HAL_Delay(50);
	HAL_GPIO_WritePin(LCD_PORT_B, LCD_RST, GPIO_PIN_SET);
}

/**************************************************************************//**
* @fn			static void SPI_Controller_Init(void)
* @brief		Initialize SPI for LCD
* @note
*****************************************************************************/
static void SPI_Controller_Init(void)
{
    //Enable clock access to SPI1 module
    RCC->APB2ENR |= SPI1EN;

    //Set 8 bit data mode
    SPI1->CR1 &= ~(1U<<11);

    //Enable full duplex
    SPI1->CR1 &=~(1U<<10);

    //Select software slave management by
    //setting SSM=1 and SSI=1
    SPI1->CR1 |= (1<<8);
    SPI1->CR1 |= (1<<9);

    //Set MSB first
    SPI1->CR1 &= ~(1U<<7);

    //Set clock to fPCLK/64
    SPI1->CR1 &= ~(1U<<3);
    SPI1->CR1 &= ~(1U<<4);
    SPI1->CR1 &= ~(1U<<5);

    //Set mode to MASTER
    SPI1->CR1 |= (1U<<2);

    //Set CPOL to 1 and CPHA to 1
    SPI1->CR1 &= ~(1U << 0); // CPHA = 0
    SPI1->CR1 &= ~(1U << 1); // CPOL = 0

    //Enable SPI module
    SPI1->CR1 |= (1<<6);
}

/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			void Delay_ms(unsigned int n)
* @brief		Delay function using variables
* @note
*****************************************************************************/
void Delay_ms(unsigned int n)
{
	while (n--)
	{
		HAL_Delay(1);
	}
}

/**************************************************************************//**
* @fn			void SPI_ControllerTx(uint8_t data)
* @brief		Send 8-bit SPI data to peripheral
* @note
*****************************************************************************/
void SPI_ControllerTx(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_RESET);	//CS pulled low to start communication

	SPI_ControllerTx_stream(data);

	HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_SET);	//set CS to high
}

/**************************************************************************//**
* @fn			void SPI_ControllerTx_stream(uint8_t stream)
* @brief		Send a command to LCD through SPI without setting CS or DC
* @note
*****************************************************************************/

void SPI_ControllerTx_stream(uint8_t stream)
{
    uint8_t temp;

    /* Wait until TXE (Transmit Buffer Empty) is set */
    while(!(SPI1->SR & SR_TXE)){}

    /* Write the single byte to the data register */
    SPI1->DR = stream;

    /* Wait again until TXE is set (and optionally until RXNE if needed) */
    while(!(SPI1->SR & SR_TXE)){}

    /* Wait for BUSY flag to reset */
    while((SPI1->SR & SR_BSY)){}

    /* Clear any overrun flag by reading DR and SR */
    temp = SPI1->DR;
    temp = SPI1->SR;
}


/**************************************************************************//**
* @fn			void SPI_ControllerTx_16bit(uint16_t data)
* @brief		Send 16-bit data to peripheral
* @note			Used for color information
*****************************************************************************/
void SPI_ControllerTx_16bit(uint16_t data)
{
    // Split 16-bit data into two bytes
    uint8_t highByte = (data >> 8) & 0xFF;
    uint8_t lowByte  = data & 0xFF;

    // Pull the chip select (CS) pin low (assuming it's on PA9)
    HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_RESET);

    /* --- Transmit the high byte --- */

    // Wait until transmit buffer is empty (TXE = 1)
    while (!(SPI1->SR & SR_TXE)) {}

    // Write the high byte to the SPI data register
    SPI1->DR = highByte;

    // Wait for TXE again to confirm it’s been moved to the shift register
    while (!(SPI1->SR & SR_TXE)) {}

    // Wait until the SPI is no longer busy (BSY = 0)
    while (SPI1->SR & SR_BSY) {}

    // Clear overrun if needed by reading DR and SR
    volatile uint8_t dummy = SPI1->DR;
    dummy = SPI1->SR;

    /* --- Transmit the low byte --- */

    while (!(SPI1->SR & SR_TXE)) {}
    SPI1->DR = lowByte;
    while (!(SPI1->SR & SR_TXE)) {}
    while (SPI1->SR & SR_BSY) {}
    dummy = SPI1->DR;
    dummy = SPI1->SR;

    // Pull CS high to end the transaction
    HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_SET);
}



/**************************************************************************//**
* @fn			void SPI_ControllerTx_16bit_stream(uint16_t data)
* @brief		Send 16 bit data to LCD through SPI without setting CS or DC
* @note			Used for color information
*****************************************************************************/
void SPI_ControllerTx_16bit_stream(uint16_t data)
{
    // Break 16-bit data into two bytes
    uint8_t highByte = (data >> 8) & 0xFF;
    uint8_t lowByte  = data & 0xFF;

    volatile uint8_t dummy;

    /* --- Transmit the high byte --- */
    // Wait until TXE = 1
    while (!(SPI1->SR & SR_TXE)) {}

    // Write the high byte
    SPI1->DR = highByte;

    // Wait until TXE = 1 again
    while (!(SPI1->SR & SR_TXE)) {}
    // Optionally also wait for BSY to clear if you need to ensure full completion:
    while (SPI1->SR & SR_BSY) {}

    // Clear overrun by reading DR and SR
    dummy = SPI1->DR;
    dummy = SPI1->SR;

    /* --- Transmit the low byte --- */
    while (!(SPI1->SR & SR_TXE)) {}
    SPI1->DR = lowByte;
    while (!(SPI1->SR & SR_TXE)) {}
    while (SPI1->SR & SR_BSY) {}
    dummy = SPI1->DR;
    dummy = SPI1->SR;
}

/**************************************************************************//**
* @fn			void lcd_init(void)
* @brief		Initialize LCD settings
* @note
*****************************************************************************/
void lcd_init(void)
{
	lcd_pin_init();
	SPI_Controller_Init();
	HAL_Delay(5);

	static uint8_t ST7735_cmds[]  =
	{
		ST7735_SWRESET, 0, 150,       // Software reset. This first one is needed because of the RC reset.
		ST7735_SLPOUT, 0, 255,       // Exit sleep mode
		ST7735_FRMCTR1, 3, 0x01, 0x2C, 0x2D, 0,  // Frame rate control 1
		ST7735_FRMCTR2, 3, 0x01, 0x2C, 0x2D, 0,  // Frame rate control 2
		ST7735_FRMCTR3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0,  // Frame rate control 3
		ST7735_INVCTR, 1, 0x07, 0,	// Display inversion
		ST7735_PWCTR1, 3, 0x0A, 0x02, 0x84, 5, // Power control 1
		ST7735_PWCTR2, 1, 0xC5, 5,       // Power control 2
		ST7735_PWCTR3, 2, 0x0A, 0x00, 5,	// Power control 3
		ST7735_PWCTR4, 2, 0x8A, 0x2A, 5,	// Power control 4
		ST7735_PWCTR5, 2, 0x8A, 0xEE, 5,	// Power control 5
		ST7735_VMCTR1, 1, 0x0E, 0, // Vcom control 1
		ST7735_INVOFF, 0, 0,	//Inversion off
		ST7735_MADCTL, 1, 0xC8, 0,	//Memory Access control
		ST7735_COLMOD, 1, 0x05, 0,	//Interface pixel format
		ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,		//Column
		ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,		//Page
		ST7735_GMCTRP1, 16, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
		0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, 0, // Positive Gamma
		ST7735_GMCTRN1, 16, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29,0x2D,
		0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, 0, // Negative Gamma
		ST7735_NORON, 0, 10,	//Normal display on
		ST7735_DISPON, 0, 100,            // Set display on
		ST7735_MADCTL, 1, MADCTL_MX | MADCTL_MV | MADCTL_RGB, 10		//Default to rotation 3
	};

	sendCommands(ST7735_cmds, 22);
}

/**************************************************************************//**
* @fn			void sendCommands (const uint8_t *cmds, uint8_t length)
* @brief		Parse and send array of commands thru SPI
* @note
*****************************************************************************/
void sendCommands (const uint8_t *cmds, uint8_t length)
{
	//Command array structure:
	//Command Code, # of data bytes, data bytes (if any), delay in ms
	uint8_t numCommands, numData, waitTime;

	numCommands = length;	// # of commands to send

	HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_RESET);	//CS pulled low to start communication

	while (numCommands--)	// Send each command
	{
		HAL_GPIO_WritePin(LCD_PORT_A, LCD_DC, GPIO_PIN_RESET);	//D/C pulled low for command

		SPI_ControllerTx_stream(*cmds++);

		numData = *cmds++;	// # of data bytes to send

		HAL_GPIO_WritePin(LCD_PORT_A, LCD_DC, GPIO_PIN_SET);	//D/C set high for data
		while (numData--)	// Send each data byte...
		{
			SPI_ControllerTx_stream(*cmds++);

		}

		waitTime = *cmds++;     // Read post-command delay time (ms)
		if (waitTime!=0)
		{
			Delay_ms((waitTime==255 ? 500 : waitTime));
		}
	}

	HAL_GPIO_WritePin(LCD_PORT_A, LCD_TFT_CS, GPIO_PIN_SET);	//set CS to high
}

/**************************************************************************//**
* @fn			void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
* @brief		Set pixel memory address to write to
* @note
*****************************************************************************/
void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t ST7735_cmds[]  =
	{
		ST7735_CASET, 4, 0x00, x0, 0x00, x1, 0,		// Column
		ST7735_RASET, 4, 0x00, y0, 0x00, y1, 0,		// Page
		ST7735_RAMWR, 0, 5				// Into RAM
	};
	sendCommands(ST7735_cmds, 3);
}

/**************************************************************************//**
* @fn			void LCD_brightness(uint8_t intensity)
* @brief		Changes the intensity of the LCD screen (max = 255)
* @note
*****************************************************************************/
void LCD_brightness(uint8_t intensity)
{
	int dummy = intensity;	//Set PWM value
}

/**************************************************************************//**
* @fn			void LCD_rotate(uint8_t r)
* @brief		Rotate display to another orientation
* @note
*****************************************************************************/
void LCD_rotate(uint8_t r)
{
	uint8_t madctl = 0;
	uint8_t rotation = r % 4; // can't be higher than 3

	switch (rotation) {
		case 0:
			madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
			break;
		case 1:
			madctl = MADCTL_MY | MADCTL_MV | MADCTL_RGB;
			break;
		case 2:
			madctl = MADCTL_RGB;
			break;
		case 3:
			madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
			break;
		default:
			madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
			break;
	}

	uint8_t ST7735_cmds[]  =
	{
		ST7735_MADCTL, 1, madctl, 0
	};

	sendCommands(ST7735_cmds, 1);
}
