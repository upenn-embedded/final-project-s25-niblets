#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define F_CPU 16000000UL
#define BIT_CLOCK_HZ 128000UL
#define UART_BAUD_RATE      9600
#define UART_BAUD_PRESCALER (((F_CPU / (UART_BAUD_RATE * 16UL))) - 1)
#define OCR1A_VAL ((F_CPU / BIT_CLOCK_HZ) - 1)

volatile uint32_t mic_sample = 0;
volatile uint8_t bit_count = 0;
volatile uint8_t sample_ready = 0;
volatile uint16_t counter = 0;
volatile char buff[20];

/*
 Connections Guide
 INMP 441 -> ATMEGA328PB
 GND -> GND
 VDD -> 3V3
 SDA -> PD2
 SCK -> PD1
 WS -> PB1
 L/R -> GND
 */

// BCK falling edge interrupt
ISR(PCINT2_vect) {
    if (!(PIND & (1 << PD5))) {          // Check falling edge
        uint8_t bit = (PIND >> PD2) & 1; // Read DATA pin
        mic_sample = (mic_sample << 1) | bit;
        bit_count++;
        
        if (bit_count >= 24) {           // Capture 24-bit sample
            sample_ready = 1;
            bit_count = 0;
        }
    }
}


void timer0_init() {
    DDRD |= (1 << PD5);                  // SCK output (PD5)
    TCCR0A = (1 << COM0B0) | (1 << WGM01); // CTC mode, toggle OC0B
    TCCR0B = (1 << CS00);                // No prescaler
    OCR0A = 61;                          // ~129 kHz BCK
}


void timer1_init() {
    DDRB |= (1 << PB1); // Set PB1 to output
    TCCR1A = (1 << COM1A0);              // Toggle OC1A on compare
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
    OCR1A = 499;                         // ~2 kHz WS
}


int main(void) {
    DDRD &= ~(1 << PD2);    // Set PD2 to input for SDA line
    
    // Initialize timers 0 and 1
    timer0_init();
    timer1_init();
    
    UART_init(UART_BAUD_PRESCALER);
    
    // Configure PCINT for SCK (PD5)
    PCICR |= (1 << PCIE2);               // Enable PCINT2 group
    PCMSK2 |= (1 << PCINT21);            // Enable PD5 interrupt
    sei();
    
    while (1) {
        if (sample_ready) {
            uint16_t sample = mic_sample;
            sample_ready = 0;
            if (sample != 0) {
            char buff[20];
            sprintf(buff, "Mic value:\t%d\n", sample); // for viewing on the serial terminal
            UART_putstring(buff); }
        }   
    }
    return 0;
}
