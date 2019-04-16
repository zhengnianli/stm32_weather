#ifndef __RTC_H
#define __RTC_H

#include "config.h"

/* 日历结构体 */
typedef struct 
{
	uint16 w_year;
	uint8  w_month;
	uint8  w_date;
	uint8  hour;
	uint8  min;
	uint8  sec;
	uint8  week;
}Calendar_T;

extern Calendar_T calendar_t;//声明一个“日历结构体”为外部变量
extern u8 g_time_flag;       //声明“秒中断标志位”为外部变量
extern char *wday[];		 //声明一个关于星期的指针数组变量
extern u8 WeatherIconFlag;

/* 初始化RTC,返回0（成功），返回1（失败） */
uint8 RTC_Init(void);

/* 平、闰年判断 */
uint8 IsLeapYear(uint16 year);

/* 获得时间 */
uint8 RTC_Get(void);
 
/* 获得星期 */
uint8 RTC_GetWeek(uint16 year,uint8 month,uint8 day);

/* 设置时间 */
uint8 RTC_Set(uint16 year,uint8 mon,uint8 day,uint8 hour,uint8 min,uint8 sec);	

/* 打印时间至串口屏 */
void DisplayTime(Calendar_T time);

#endif

