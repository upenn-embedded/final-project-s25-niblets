/**
 * I2S_DMA.c
 *
 * Created on: Apr 10, 2025
 * Author: banele
 */

#include <i2s.h>

// Configure GPIO pins for I2S2:
// - PC1 as I2S2_SD (data)
// - PB12 as I2S2_WS (word select)
// - PB10 as I2S2_CK (clock)
// Using medium output speed, alternate function 5

void I2S2_init(void) {
    I2S2_gpio_config();
    I2S_config();
    I2S2_DMA_config(); // Add DMA configuration
}

void I2S2_gpio_config(void) {
    // Enable clocks for GPIOB and GPIOC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // --- PC1: I2S2_SD (AF5) ---
    GPIOC->MODER &= ~(3U << (1 * 2));         // Clear mode bits for PC1
    GPIOC->MODER |= (2U << (1 * 2));          // Set to alternate function mode
    GPIOC->AFR[0] &= ~(0xF << (1 * 4));       // Clear AF bits
    GPIOC->AFR[0] |= (0x5 << (1 * 4));        // Set AF5 (I2S2_SD)
    GPIOC->OSPEEDR |= (1U << (1 * 2));        // Set medium speed (missing in original)

    // --- PB10: I2S2_CK (AF5) ---
    GPIOB->MODER &= ~(3U << (10 * 2));        // Clear mode bits for PB10
    GPIOB->MODER |= (2U << (10 * 2));         // Set to alternate function mode
    GPIOB->AFR[1] &= ~(0xF << ((10 - 8) * 4)); // Clear AF bits
    GPIOB->AFR[1] |= (0x5 << ((10 - 8) * 4));  // Set AF5 (I2S2_CK)
    GPIOB->OSPEEDR |= (1U << (10 * 2));       // Set medium speed (missing in original)

    // --- PB12: I2S2_WS (AF5) ---
    GPIOB->MODER &= ~(3U << (12 * 2));        // Clear mode bits for PB12
    GPIOB->MODER |= (2U << (12 * 2));         // Set to alternate function mode
    GPIOB->AFR[1] &= ~(0xF << ((12 - 8) * 4)); // Clear AF bits
    GPIOB->AFR[1] |= (0x5 << ((12 - 8) * 4));  // Set AF5 (I2S2_WS)
    GPIOB->OSPEEDR |= (1U << (12 * 2));       // Set medium speed (missing in original)
}

void I2S_config(void) {
    // 1. Configure I2S PLL for 44.1kHz (this was missing)
    // Disable PLL I2S before configuration
    RCC->CR &= ~RCC_CR_PLLI2SON;

    // Configure PLL I2S values for 44.1kHz
    // Assuming 8MHz HSE clock
    // For 44.1kHz: PLLI2SN = 271, PLLI2SR = 2
    RCC->PLLI2SCFGR = (271 << RCC_PLLI2SCFGR_PLLI2SN_Pos) |
                       (2 << RCC_PLLI2SCFGR_PLLI2SR_Pos);

    // Enable PLL I2S
    RCC->CR |= RCC_CR_PLLI2SON;

    // Wait until PLL I2S is locked
    while(!(RCC->CR & RCC_CR_PLLI2SRDY));

    // 2. Enable SPI2 clock
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    // 3. Disable SPI2 before configuring
    SPI2->I2SCFGR &= ~(SPI_I2SCFGR_I2SE);

    // 4. Configure I2S
    SPI2->I2SCFGR = 0;                                // Reset I2SCFGR
    SPI2->I2SCFGR |= (1U << SPI_I2SCFGR_I2SMOD_Pos);  // Enable I2S mode
    SPI2->I2SCFGR |= (0b01U << SPI_I2SCFGR_I2SCFG_Pos); // Master Transmitter
    SPI2->I2SCFGR |= (0b00U << SPI_I2SCFGR_I2SSTD_Pos); // Philips standard
    SPI2->I2SCFGR |= (0b00U << SPI_I2SCFGR_DATLEN_Pos); // 16-bit data length
    SPI2->I2SCFGR |= (0b00U << SPI_I2SCFGR_CHLEN_Pos);  // 16-bit channel length
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CKPOL;              // Clock polarity: low when idle

    // 5. Configure prescaler
    SPI2->I2SPR = 0;
    SPI2->I2SPR |= (13U << SPI_I2SPR_I2SDIV_Pos);     // I2SDIV = 13
    SPI2->I2SPR |= SPI_I2SPR_ODD;                     // ODD = 1
    SPI2->I2SPR &= ~SPI_I2SPR_MCKOE;                  // MCLK output disabled

    // 6. Enable DMA for transmit
    SPI2->CR2 |= SPI_CR2_TXDMAEN;                     // Enable TX DMA (missing in original)

    // 7. Enable I2S
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

// Remove these functions - they're incorrect when using I2S in hardware mode
// void ws2_enable(void) {
//     GPIOB->ODR &= ~(1U << 12); // Active low - Don't manually control this pin!
// }
//
// void ws2_disable(void) {
//     GPIOB->ODR |= (1U << 12); // Inactive high - Don't manually control this pin!
// }

// Add DMA configuration (missing from original)
void I2S2_DMA_config(void) {
    // Enable DMA1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    // Configure DMA1 Stream 4, Channel 0 for SPI2_TX
    // Disable stream first
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;
    while(DMA1_Stream4->CR & DMA_SxCR_EN); // Wait until stream is disabled

    // Configure DMA stream
    DMA1_Stream4->CR = 0;                  // Reset register
    DMA1_Stream4->CR |= (0 << DMA_SxCR_CHSEL_Pos); // Channel 0
    DMA1_Stream4->CR |= DMA_SxCR_MINC;     // Memory increment mode
    DMA1_Stream4->CR |= DMA_SxCR_DIR_0;    // Memory to peripheral
    DMA1_Stream4->CR |= (1 << DMA_SxCR_MSIZE_Pos);  // 16-bit memory size
    DMA1_Stream4->CR |= (1 << DMA_SxCR_PSIZE_Pos);  // 16-bit peripheral size
    DMA1_Stream4->CR |= DMA_SxCR_PL_1;     // High priority

    // Set peripheral address
    DMA1_Stream4->PAR = (uint32_t)&(SPI2->DR);

    // Note: Memory address and number of data items will be set in transmit function
}

void I2S2_transmit_DMA(uint16_t *buffer, uint16_t len) {
    // Wait for any ongoing DMA transfer to complete
    while(DMA1_Stream4->CR & DMA_SxCR_EN);

    // Configure DMA
    DMA1_Stream4->M0AR = (uint32_t)buffer;  // Set memory address
    DMA1_Stream4->NDTR = len;              // Set number of data items

    // Clear flags
    DMA1->HIFCR = DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 |
                 DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4;

    // Enable DMA
    DMA1_Stream4->CR |= DMA_SxCR_EN;

    // Note: The function returns immediately.
    // You can check DMA1->HISR & DMA_HISR_TCIF4 to see if transfer is complete
}

// Keep the original polling-based transmit function for reference
void I2S2_transmit(uint16_t *buffer, uint8_t len) {
    for (uint8_t i = 0; i < len; ++i) {
        // Wait until TXE (Transmit buffer empty) is set
        while (!(SPI2->SR & SPI_SR_TXE)) {
            // wait
        }
        // Write 16-bit data to the data register
        SPI2->DR = buffer[i];
    }
    // Wait until BSY (Busy flag) is cleared: last frame is fully sent
    while (SPI2->SR & SPI_SR_BSY) {
        // wait
    }
}
