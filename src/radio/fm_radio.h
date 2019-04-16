#ifndef __FM_RADIO_H
#define __FM_RADIO_H

#include "config.h"

#define FM_SAVE_COUNT	32			/* 最多保存几个电台 */

typedef struct
{
	uint8_t ucSearch;			/* 1 表示正在搜索中 */
	uint8_t ucSpkEn;			/* 0 : 耳机输出， 1: 扬声器输出 */
	uint8_t ucMuteOn;			/* 0 : 静音， 1: 放音 */
	uint8_t ucAutoMode;			/* 0 : 左右键用于修改频率 1: 左右键用于自动搜索下一个电台 */
	uint8_t ucVolume;			/* 当前音量 */
	uint8_t ucAdcLevelSet;		/* 自动搜索停止的ADC电平 */
	uint8_t ucAdcLevelNow;		/* 当前频道的ADC电平 */
	uint32_t ulFreq;			/* 当前频率 */	
	uint8_t ucDirection;		/* 自动搜索方向 */
	uint32_t ulSaveFreq[FM_SAVE_COUNT];	/* 保存的频点列表 */
	uint8_t ucSaveCount;			/* 保存的频点个数 */
	uint8_t ucSavePos;
}RADIO_T;
extern RADIO_T g_tRadio;

void RadioInit(void);

#endif
