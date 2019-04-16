#ifndef __TEA5767_H
#define __TEA5767_H

#include "config.h"

#define TEA5767_SLAVE_ADDRESS    0xC0

#define FM_FREQ_INIT	87500000	/* 搜索频率初值 */

#define FM_FREQ_MIN		87500000	/* 最小频率 */
#define FM_FREQ_MAX		108000000	/* 最大频率 */
#define FREQ_STEP		100000		/* 0.1MHz = 100000KHz */
#define FREQ_COUNT		((FM_FREQ_MAX - FM_FREQ_MIN) / FREQ_STEP + 1)	/* 频点个数 */

/* 静音选项 */
enum
{
	MUTE_OFF = 0,
	MUTE_ON = 1
};

/* 搜索选项 */
enum
{
	SEARCH_OFF = 0,
	SEARCH_ON = 1
};
enum
{
	SEARCH_DOWN = 0,
	SEARCH_UP = 1
};
enum
{
	ADC_LEVEL_5 = 1,
	ADC_LEVEL_7 = 2,
	ADC_LEVEL_10 = 3
};

/* TEA5767芯片的状态 */
typedef struct
{
	uint8_t ucReady;				/* 电台就绪标志，1表示就绪 */
	uint8_t ucBandLimit;			/* 频带极限标志， 1表示到了极限 */
	uint16_t usPll;					/* 当前PLL值 */
	uint8_t ucStereo;				/* 立体声标志 1表示立体声 0表示单声道 */
	uint8_t ucIFCount;				/* IF计数器结果 */
	uint8_t ucAdcLevel;				/* ADC电平 */
	uint32_t ulFreq;				/* 当前频率 */
}TEA5767_T;

void tea5767_Set(uint32_t _Freq, uint8_t _ucMuteEn, uint8_t _ucSerchEn, uint8_t _ucSearchUp, uint8_t _ucAdcLevel);
void tea5767_ReadStatus(TEA5767_T *_tStatus);

#endif

