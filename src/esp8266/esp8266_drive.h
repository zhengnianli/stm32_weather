#ifndef _esp8266_drive_H
#define _esp8266_drive_H

#include "config.h"


//#define WIFI_SSID	  	  "HiWiFi_cekong"		//要建立的热点的名称
//#define WIFI_PASSWORD  	  "cekong2015"      //要建立的热点的密钥
//#define WIFI_SSID	  	  "MYTEST"		  //要建立的热点的名称
//#define WIFI_PASSWORD  	  "pppp1234"      //要建立的热点的密钥

#define USART2_MAX_RECV_LEN		1500				//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		600					//最大发送缓存字节数

extern uint8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 	//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern uint8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern uint16 USART2_RX_STA;   						//接收数据状态

#define ESP8266_RST_Pin          	GPIO_Pin_6		//复位引脚
#define ESP8266_RST_Pin_Port     	GPIOE
#define ESP8266_RST_Pin_Periph_Clock  RCC_APB2Periph_GPIOE

#define ESP8266_CH_PD_Pin     		GPIO_Pin_7		//使能引脚,高电平有效
#define ESP8266_CH_PD_Pin_Port      GPIOG
#define ESP8266_CH_PD_Pin_Periph_Clock  RCC_APB2Periph_GPIOG

#define ESP8266_RST_Pin_SetH     GPIO_SetBits(ESP8266_RST_Pin_Port,ESP8266_RST_Pin)
#define ESP8266_RST_Pin_SetL     GPIO_ResetBits(ESP8266_RST_Pin_Port,ESP8266_RST_Pin)

#define ESP8266_CH_PD_Pin_SetH     GPIO_SetBits(ESP8266_CH_PD_Pin_Port,ESP8266_CH_PD_Pin)
#define ESP8266_CH_PD_Pin_SetL     GPIO_ResetBits(ESP8266_CH_PD_Pin_Port,ESP8266_CH_PD_Pin)

//传输协议类型定义
typedef enum
{
	enumTCP,
	enumUDP
}ENUM_NetPro_TypeDef;	

//函数的声明
void ESP8266_USART2_Init(uint32 bound);
void ESP8266_Printf(char *fmt,...);
uint8 ESP8266_SendATCmd(uint8 *cmd, uint8 *ack, uint16 wait_time);
uint8 ESP8266_ExitTran(void);
uint8 ESP8266_LinkServer(ENUM_NetPro_TypeDef enumN,uint8 *ip, uint8 *port);
void ESP8266_Config(void);
uint8 ESP8266_ConnectAP(char *ssid, char *password);

#endif
