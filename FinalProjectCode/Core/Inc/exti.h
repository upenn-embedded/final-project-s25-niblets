#ifndef  EXTI_H
#define EXTI_H
#include <stdint.h>
#include "stm32f4xx.h"
#define   LINE13        (1U<<13)
void pc13_exti_init(void);
void pb3_exti_init(void);
void pb5_exti_init(void);
void pa10_exti_init(void);
#endif
