#ifndef _LD3320_MAIN_H
#define LD_CHIP_H

#include "config.h"

extern char g_WeatherText[200];

void LD3320_Init(void);
void  LD3320_delay(unsigned long uldata);
uint8 RunASR(void);
void ASRSuccess_Handle(uint8 asr_code);

#endif