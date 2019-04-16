#ifndef __CONFIG_H 
#define __CONFIG_H

//----------------------------------------------------------------
//无符号类型宏定义
#define uchar 	unsigned char													 
#define uint 	unsigned int

#define uint8 	unsigned char 
#define uint16 	unsigned short int
#define uint32 	unsigned int

//----------------------------------------------------------------
// MAIN_CONFIG宏	main.c 定义#define  MAIN_CONFIG
#ifdef 	 MAIN_CONFIG										// 头文件被多个C文件调用,避免变量冲突的问题 
	#define  EXT											// 主文件中定义的变量
#else
	#define  EXT	extern									// 全局文件变量的声明
#endif
	
//----------------------------------------------------------------
//包含常用头文件  
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include "system.h"
#include "SysTick.h"
	
//---------------------------------------------------------------
//包含驱动头文件
#include "usart1.h"
#include "serial_screen.h"
#include "rtc.h"
#include "dht11.h"
#include "esp8266_drive.h"
#include "http_client.h"
#include "iic.h"
#include "tea5767.h"
#include "fm_radio.h"
#include "timer.h"
#include "cJSON.h"
#include "malloc.h"
#include "LDchip.h"
#include "Reg_RW.h"
#include "LD3320_main.h"
#include "spi.h"
#include "syn6288.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "sd.h"

	
//---------------------------------------------------------------
//包含全局变量

//---------------------------------------------------------------
//包含全局宏定义
#define CPU_INT_DISABLE()   { __set_PRIMASK(1); }		// 关闭总中断 
#define CPU_INT_ENABLE()    { __set_PRIMASK(0);	} 		// 开启总中断

//---------------------------------------------------------------
//包含全局类型定义
typedef enum{FALSE=0,TRUE=!FALSE,}BOOL;					// 成功or失败

////---------------------------------------------------------------
////包含全局系统定时器
//#define  MAX_TIMER  		2							// 最大定时器个数
//EXT volatile unsigned long  g_dwSysTimer[MAX_TIMER];	// 定义系统定时器 
//#define  Uart2CharRecTimer		g_dwSysTimer[0]			// 串口2数据接收
//#define  Uart3CharRecTimer  	g_dwSysTimer[1]	 		// 串口3数据接收


#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/


