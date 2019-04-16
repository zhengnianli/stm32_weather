#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H

#include "config.h"

extern uint8 g_connect_sta;

/* 本地服务器--用于测试TCP Client是否发送数据成功 */
#define My_Computer 	"10.48.90.54"//"192.168.199.237"  
#define My_Port 		"8080"

/* 心知天气（api.seniverse.com）--天气数据的来源 */
#define WEATHER_IP 		"116.62.81.138"
#define WEATHER_PORT 	"80"

/* 时间服务器--用于同步网络时间 */
#define TIME_SERVERIP	"www.beijing-time.org"
#define TIME_PORTNUM	"80"

/* 当天天气图标的坐标及图片ID */
#define Today_Icon_X		140
#define Today_Icon_Y		170
#define Today_Icon_ID		(atoi(weather.code_day[0])+12) //加上12是因为上位机软件中天气现象图片ID是从12开始的（可更改）

/* 明天天气图标的坐标及图片ID */
#define Tomorrow_Icon_X		250
#define Tomorrow_Icon_Y		170
#define Tomorrow_Icon_ID	(atoi(weather.code_day[1])+12) //加上12是因为上位机软件中天气现象图片ID是从12开始的（可更改）

/* 后天天气图标的坐标及图片ID */
#define AfterTomor_Icon_X	250
#define AfterTomor_Icon_Y	170
#define AfterTomor_Icon_ID	(atoi(weather.code_day[2])+12) //加上12是因为上位机软件中天气现象图片ID是从12开始的（可更改）

/* wifi图标的坐标及图片ID */
#define Wifi_Icon_X			4
#define Wifi_Icon_Y			11
#define Wifi_ConnectIcon_ID	51
#define Wifi_DisconnIcon_ID	52

/* 天气语音播报长文本 */
// 范例：知心天气为您播报最新天气，福州市今天白天到晚间多云，最高温25摄氏度，最低温18摄氏度，东南风2级，空气质量为轻度污染
#define WeatherText1 "知心天气为您播报最新天气，"
#define WeatherText2 "市今天白天到晚间"
#define WeatherText3 ",[3]最高温"
#define WeatherText4 ",[3]最低温"

/* 天气数据结构体 */
typedef struct
{
	/* 实况天气数据 */
	char name[32];				//地名
	char text[32];				//天气预报文字
	char code[32];				//天气预报代码
	char temperature[32];   	//气温
	
	/* 今天、明天、后天天气数据 */
	char date[3][32];			//日期
	char text_day[3][64];	    //白天天气现象文字
	char code_day[3][32];		//白天天气现象代码
	char code_night[3][64]; 	//晚间天气现象代码
	char high[3][32];			//最高温
	char low[3][32];			//最低温
	char wind_direction[3][64]; //风向
	char wind_speed[3][32];  	//风速，单位km/h（当unit=c时）
	char wind_scale[3][32];  	//风力等级
	
	char life_suggestion[128][2];	//生活建议
}Result_T;

/* 用于解析天气数据结构体中的date成员 */
typedef struct
{
	int date_year;
	int date_month;
	int date_day;
}date_T;

extern Result_T weather_data;
extern date_T g_date[3];
extern int g_week[3];
extern char g_page[32];

//函数的声明,供外部文件调用
void ESP8266_TCPClient_Test(void);
uint8 GET_NowWeather(void);
uint8 GET_DailyWeather(void);
//uint8 GET_LifeSuggestion(void);
uint8 GET_BeiJingTime(void);
void DisplayWeather(Result_T weather);
void DisplayWeatherIcon(Result_T weather);
void DisplayWifiIcon(uint8 connect_sta);
void DisplayTest(Result_T result);


//SwitchToGbk函数在utf8togbk.c中
extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

#endif
