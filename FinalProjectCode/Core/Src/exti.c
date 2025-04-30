#include "exti.h"

#define GPIOCEN         (1U<<2)
#define SYSCFGEN        (1U<<14)


void pc13_exti_init(void)
{
    /*Disable global interrupts*/
    __disable_irq();
    /*Enable clock access for GPIOC*/
    RCC->AHB1ENR |=GPIOCEN;
    /*Set PC13 as input*/
    GPIOC->MODER &=~(1U<<26);
    GPIOC->MODER &=~(1U<<27);
    /*Enable clock access to SYSCFG*/
    RCC->APB2ENR |=SYSCFGEN;
    /*Select PORTC for EXTI13*/
    SYSCFG->EXTICR[3] |=(1U<<5);
    /*Unmask EXTI13*/
    EXTI->IMR |=(1U<<13);
    /*Select falling edge trigger*/
    EXTI->FTSR |=(1U<<13);
    /*Enable EXTI13 line in NVIC*/
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    /*Enable global interrupts*/
    __enable_irq();
}

// EXTI on PB3 — rising edge
void pb3_exti_init(void)
{
    __disable_irq();

    // 1) Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // 2) Set PB3 as input (MODER3 = 00)
    GPIOB->MODER &= ~(3U << (3 * 2));

    // 2a) Pull‑down so PB3 idles low
    GPIOB->PUPDR  &= ~(3U << (3 * 2));
    GPIOB->PUPDR  |=  (2U << (3 * 2));   // 10 = pull‑down

    // 3) Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // 4) Map PB3 to EXTI3 (EXTICR[0], bits [15:12] = 1 for port B)
    SYSCFG->EXTICR[0] &= ~(0xF << 12);
    SYSCFG->EXTICR[0] |=  (1U  << 12);

    // 5) Clear any pending EXTI3 flag
    EXTI->PR |= (1U << 3);

    // 6) Unmask EXTI3
    EXTI->IMR |= (1U << 3);

    // 7) Rising‑edge trigger only
    EXTI->FTSR &= ~(1U << 3);
    EXTI->RTSR |=  (1U << 3);

    // 8) Enable EXTI3 IRQ in NVIC
    NVIC_SetPriority(EXTI3_IRQn, 1);
    NVIC_EnableIRQ(EXTI3_IRQn);

    __enable_irq();
}

// EXTI on PB5 — rising edge
void pb5_exti_init(void)
{
    __disable_irq();

    // 1) Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // 2) Set PB5 as input (MODER5 = 00)
    GPIOB->MODER &= ~(3U << (5 * 2));

    // 2a) Pull‑down so PB5 idles low
    GPIOB->PUPDR  &= ~(3U << (5 * 2));
    GPIOB->PUPDR  |=  (2U << (5 * 2));   // 10 = pull‑down

    // 3) Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // 4) Map PB5 to EXTI5 (EXTICR[1], bits [7:4] = 1 for port B)
    SYSCFG->EXTICR[1] &= ~(0xF << 4);
    SYSCFG->EXTICR[1] |=  (1U  << 4);

    // 5) Clear any pending EXTI5 flag
    EXTI->PR |= (1U << 5);

    // 6) Unmask EXTI5
    EXTI->IMR |= (1U << 5);

    // 7) Rising‑edge trigger only
    EXTI->FTSR &= ~(1U << 5);
    EXTI->RTSR |=  (1U << 5);

    // 8) Enable EXTI5–9 IRQ in NVIC
    NVIC_SetPriority(EXTI9_5_IRQn, 1);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    __enable_irq();
}

// EXTI on PA10
void pa10_exti_init(void)
{
    __disable_irq();
    // 1) Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // 2) Set PA10 as input (MODER10 = 00)
    GPIOA->MODER &= ~(1U << 20);
    GPIOA->MODER &= ~(1U << 21);
    // 3) Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 4) Select PA for EXTI10 (EXTICR[2], bits [11:8] = 0)
    SYSCFG->EXTICR[2] &= ~(0xF << 8);
    SYSCFG->EXTICR[2] |=  (0   << 8);
    // 5) Unmask EXTI10 and configure falling‑edge trigger
    EXTI->IMR  |= (1U << 10);
    EXTI->FTSR &= ~(1U << 10);  // disable falling
       EXTI->RTSR |=  (1U << 10);
    // 6) Enable EXTI15_10 IRQ (covers lines 10–15)
    NVIC_SetPriority(EXTI15_10_IRQn, 1);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    __enable_irq();
}
