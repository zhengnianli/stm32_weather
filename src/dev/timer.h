#ifndef _TIMER_H
#define _TIMER_H

#include "config.h"

#define  MAX_TIMER  		5						// 最大定时器个数
EXT volatile unsigned long  g_Timer1[MAX_TIMER]; 
#define  GetWeatherTimer	g_Timer1[0]				// 定时GET天气数据包
#define  SendWeatherTimer  	g_Timer1[1]	 			// 定时发送天气数据至串口屏
#define  GetHMIDataTimer  	g_Timer1[2]	 			// 定时获取串口屏下发的数据
#define  TimeCalibraTimer  	g_Timer1[3]	 			// 定时校准时间
#define  DHT11CollectTimer  g_Timer1[4]	 			// 采集温湿度数据

#define  TIMER1_SEC  	(1)							// 秒
#define  TIMER1_MIN  	(TIMER1_SEC*60)				// 分
#define  TIMER1_HOUR  	(TIMER1_MIN*60)			 	// 时
#define  TIMER1_DAY  	(TIMER1_HOUR*24)			// 天

extern uint16 Uart2CharRecTimer;

void TIM1_Init(uint16 arr,uint16 psc);
void TIM2_Init(uint16 arr,uint16 psc);
void TIM3_Init(uint16 arr,uint16 psc);

#endif
