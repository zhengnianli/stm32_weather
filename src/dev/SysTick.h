#ifndef __SysTick_H
#define __SysTick_H

#include "config.h"

void SysTick_Init(uint8 SYSCLK);
void delay_ms(uint16 nms);
void delay_us(uint32 nus);

#endif
