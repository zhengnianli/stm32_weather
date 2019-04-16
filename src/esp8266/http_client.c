#include "http_client.h"

/* 定义一些全局变量 */
//------------------------------------------------------------------------------------------
//所需要的天气数据
Result_T weather_data;	

//存储获取得到的日期，用于解析。举例：2017-12-8解析出2017 12 8作为“RTC_GetWeek”的实参以计算出星期供串口屏显示。
date_T g_date[3] = {0};

//存储日期对应的星期
int g_week[3] = {0};

//串口屏的页面标识
char g_page[32] = {0};

//WiFi连接的状态 1:未连接   0：连接
uint8 g_connect_sta = 0;

//本文件函数声明，只能供本文件调用
static int cJSON_NowWeatherParse(char *JSON, Result_T *result);
static int cJSON_DailyWeatherParse(char *JSON, Result_T *result);
//static int cJSON_LifeSuggestionParse(char *JSON, Result_T *result);
static uint8 ParseBeijingTime(char* time_data);
static uint8 ParseDate(char *buf, date_T *date);

/********************************************************************************************************
** 函数: ESP8266_TCPClient_Test,测试TCP Client是否发送数据成功
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void	
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void ESP8266_TCPClient_Test(void)
{
	char *str="hello world!\r\n";
	
	printf ( "\r\nESP8266 TCP Client Test\r\n" );
	ESP8266_CH_PD_Pin_SetH;
	ESP8266_LinkServer(enumTCP, (uint8*)My_Computer, (uint8*)My_Port);
	ESP8266_ExitTran();//退出透传
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //开始透传	
	ESP8266_Printf( "%s", str );
}

/********************************************************************************************************
** 函数: GET_NowWeather, GET 天气实况数据包now.json
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:获取成功 其他：获取失败	
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_NowWeather(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //开始透传
	ESP8266_Printf("GET https://api.seniverse.com/v3/weather/now.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans&unit=c\r\n\r\n",g_city); //不要忘记\r\n\r\n,参数language、uint为可选参数	
	delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART2_RX_STA=0;	//清零串口2数据
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
	} 
//	printf("Receive now data:%s\n",USART2_RX_BUF);
//	printf("---------Now len = %d\n",strlen((char*)USART2_RX_BUF));  //266Byte
	
	cJSON_NowWeatherParse((char*)USART2_RX_BUF, &weather_data);	

	ESP8266_ExitTran();//退出透传
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //关闭连接
	
	return 0;
}

/********************************************************************************************************
** 函数: GET_DailyWeather, GET 未来几天的天气预报daily.json
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:获取成功 其他：获取失败	
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_DailyWeather(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //开始透传
	ESP8266_Printf("GET https://api.seniverse.com/v3/weather/daily.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans&unit=c&start=0&days=5\r\n\r\n",g_city); //不要忘记\n\n,参数language、uint为可选参数	
	delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART2_RX_STA=0;	//清零串口3数据
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
	} 
//	printf("Receive daily data:%s\n",USART2_RX_BUF);
//	printf("---------Daily len = %d\n",strlen((char*)USART2_RX_BUF));  //904Byte
	
	cJSON_DailyWeatherParse((char*)USART2_RX_BUF, &weather_data);
	
	ESP8266_ExitTran();//退出透传
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //关闭连接
	
	return 0;
}
#if 0
/********************************************************************************************************
** 函数: GET_LifeSuggestion, GET 一些建议
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:获取成功 其他：获取失败	
** 作者: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_LifeSuggestion(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //开始透传
	ESP8266_Printf("GET https://api.seniverse.com/v3/life/suggestion.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans\r\n\r\n",g_city); //不要忘记\n\n,参数language、uint为可选参数	
	delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART2_RX_STA=0;	//清零串口2数据
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
	} 
//	printf("Receive daily data:%s\n",USART2_RX_BUF);
//	printf("---------Daily len = %d\n",strlen((char*)USART2_RX_BUF));  //
	
	cJSON_LifeSuggestionParse((char*)USART2_RX_BUF, &weather_data);
	
	ESP8266_ExitTran();//退出透传
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //关闭连接
	
	return 0;
}
#endif

/********************************************************************************************************
** 函数: GET_BeiJingTime, 获取北京时间
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:获取成功 其他：获取失败	
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_BeiJingTime(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)TIME_SERVERIP, (uint8*)TIME_PORTNUM);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //开始透传
	ESP8266_Printf("GET http://www.beijing-time.org/time15.asp\r\n\r\n");
	delay_ms(20);
	USART2_RX_STA=0;	
	delay_ms(1000);
	printf("\r\n\r\n");
	printf("Receive BeiJing Time:%s\n",USART2_RX_BUF);
	
	ParseBeijingTime((char*)USART2_RX_BUF);
	
	ESP8266_ExitTran();//退出透传
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //关闭连接
	return 0;
}

/********************************************************************************************************
** 函数: cJSON_NowWeatherParse, 解析天气实况数据包now.json
**------------------------------------------------------------------------------------------------------
** 参数: JSON:天气实况数据包  Result_T:存储解析得到的有用的数据
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:解析成功 其他：解析失败		
** 作者: 2017.12.6 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_NowWeatherParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	
	memset(utf8str, 0, 32);
	memset(gbkstr, 0, 32);
	
//	printf("Receive now data:%s\n",JSON);
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
//			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("--------------------------------Now Weather Data------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //地名--需要用到的数据
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->name, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
				}
				/* 匹配子对象2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					printf("---------------------------------subobject2-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//天气预报文字--需要用到的数据
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->text, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//天气预报代码--需要用到的数据
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->code, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //温度--需要用到的数据
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->temperature, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
				}
				/* 匹配子对象3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					printf("---------------------------------subobject3-------------------------------\n");
					printf("cJSON_GetObjectItem: %s:%s\n\n",subobject->string,subobject->valuestring);
				}
			} 
		}
	}
	
	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;

}

/********************************************************************************************************
** 函数: cJSON_NowWeatherParse, 解析未来几天的天气预报daily.json
**------------------------------------------------------------------------------------------------------
** 参数: JSON:未来几天的天气预报数据包  Result_T:存储解析得到的有用的数据
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:解析成功 其他：解析失败			
** 作者: 2017.12.6 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_DailyWeatherParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*item,*sub_child_object,*child_Item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	int i;
	
	utf8str = (char*)mymalloc(SRAMIN, 64*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 64*sizeof(char));
	
	memset(utf8str, 0, 64);
	memset(gbkstr, 0, 64);
	
	
//	printf("Receive daily data:%s\n",JSON);
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			printf("Get Array Size: size=%d\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1------结构体location */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("---------------------------Daily Weather Data-------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //匹配子对象1成员"name"
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n", item->string, gbkstr);
					}
				}
				/* 匹配子对象2------数组daily */
				if((subobject = cJSON_GetObjectItem(object,"daily")) != NULL)
				{
					int sub_array_size = cJSON_GetArraySize(subobject);
					printf("---------------------------------subobject2-------------------------------\n");
					printf("Get Sub Array Size: sub_array_size=%d\n",sub_array_size);
					for(i=0;i<sub_array_size;i++)
					{
						if((sub_child_object = cJSON_GetArrayItem(subobject,i))!=NULL)
						{
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"date")) != NULL)//日期
							{
								memcpy(result->date[i], child_Item->valuestring, strlen(child_Item->valuestring));
								ParseDate(result->date[i], &g_date[i]);			//解析日期得到年月日
								g_week[i] = RTC_GetWeek(g_date[i].date_year, g_date[i].date_month, g_date[i].date_day); //获得星期
								printf("daliy[%d]--%s:%s(After parse:%d %d %d %s)\n",i ,child_Item->string, child_Item->valuestring,
								g_date[i].date_year, g_date[i].date_month, g_date[i].date_day,wday[g_week[i]]);
							}
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"text_day")) != NULL)//白天天气现象文字
							{
								utf8str = child_Item->valuestring;
								SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, gbkstr);
								memcpy(result->text_day[i], gbkstr, strlen(gbkstr));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"code_day")) != NULL)//白天天气现象代码
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->code_day[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"code_night")) != NULL)//夜间天气现象代码
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->code_night[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"high")) != NULL)//最高温度
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->high[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"low")) != NULL)//最低温度
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->low[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_direction")) != NULL)//风向
							{
								utf8str = child_Item->valuestring;
								SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, gbkstr);
								memcpy(result->wind_direction[i], gbkstr, strlen(gbkstr));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_speed")) != NULL)//风速，单位km/h（当unit=c时）
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->wind_speed[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_scale")) != NULL)//风力等级
							{
								printf("daliy[%d]--%s:%s\n\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->wind_scale[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
						}
					}
				}
			} 
		}
		/* 天气文本组包 */
		sprintf(g_WeatherText,"知心天气为您播报最新天气，%s市今天白天到晚间[2]%s，[3]最高温%s[3]摄氏度，[3]最低温%s[3]摄氏度，[3]%s[2]%s级",
		result->name, result->text[0], result->high[0], result->low[0], result->wind_direction[0], result->wind_scale[0]);
	}
	
	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;
}

#if 0
/********************************************************************************************************
** 函数: cJSON_LifeSuggestionParse, 解析生活指数及生活建议
**------------------------------------------------------------------------------------------------------
** 参数: JSON:未来几天的天气预报数据包  Result_T:存储解析得到的有用的数据
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:解析成功 其他：解析失败			
** 作者: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_LifeSuggestionParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*childobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 200*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 200*sizeof(char));
	
	memset(utf8str, 0, 200);
	memset(gbkstr, 0, 200);
	
//	printf("Receive LifeSuggestion data:%s\n",JSON);
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetObjectItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				if((subobject = cJSON_GetObjectItem(object,"suggestion")) != NULL)
				{
					if((childobject = cJSON_GetObjectItem(subobject,"flu"))!= NULL)
					{
						if((item=cJSON_GetObjectItem(childobject,"brief"))!=NULL)
						{
							utf8str = item->valuestring;
							SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
							printf("cJSON_GetObjectItem: %s:%s\n\n",item->string,gbkstr);
							memcpy(result->life_suggestion, gbkstr, strlen(gbkstr));
						}
					}
				}
			}
		}
	}
}
#endif

/********************************************************************************************************
** 函数: ParseBeijingTime, 解析获取得到的北京时间
**------------------------------------------------------------------------------------------------------
** 参数: time_data:网络时间
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:解析成功 其他：解析失败	
** 作者: 2017.12.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static uint8 ParseBeijingTime(char* time_data)
{
	uint8 res = 0;
	
	if(sscanf((char*)time_data,"t0=new Date().getTime();\r\nnyear=%d;\r\nnmonth=%d;"
		"\r\nnday=%d;\r\nnwday=%d;\r\nnhrs=%d;\r\nnmin=%d;\r\nnsec=%d;",
		(int*)&calendar_t.w_year, (int*)&calendar_t.w_month, (int*)&calendar_t.w_date,
			(int*)&calendar_t.week,(int*)&calendar_t.hour,(int*)&calendar_t.min,(int*)&calendar_t.sec)!=7)
	{
		return -1; //解析失败
	}
	RTC_Set(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec); 
	//解析结果
	printf("Time after update:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
	calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec);
	
	return res;		//解析成功
}

/********************************************************************************************************
** 函数: ParseDate, 解析date,如从"2017-12-14"中解析出年月日,目的是要计算出星期供串口屏显示
**------------------------------------------------------------------------------------------------------
** 参数: buf:从天气数据解析得来的日期date, date_T date:保存解析得到的结果
** 返回: 0:解析成功 其他：解析失败		
** 作者: 2017.12.13 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static uint8 ParseDate(char *buf, date_T *date)
{
	uint8 res = 0;
	
//	printf("-----------------------%s %s\n", __FUNCTION__, buf);
	if(sscanf((char*)buf,"%d-%d-%d",&date->date_year,&date->date_month,&date->date_day)!=3)
	{
		printf("%s:Parse Date Error!\n",__FUNCTION__);
		return -1;
	}
	//解析结果
//	printf("-----------------------%d %d %d\n",date->date_year,date->date_month,date->date_day);
	
	return res;
}

/********************************************************************************************************
** 函数: DisplayWeather, 更新天气数据至液晶触摸串口屏
**------------------------------------------------------------------------------------------------------
** 参数: weather：获取到的天气数据
** 返回: 无		
** 说明: 举例说明：“Desk.DeskPlaceTxt.txt”中Desk为页面名称，DeskPlaceTxt为文本控件名称，txt为控件的txt属性。
**		其中页面名称与控件名称是可更改的，属性名称是不可更改的。		
** 作者: 2017.12.13 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char pbuf[64] = {0};
void DisplayWeather(Result_T weather)
{
	//------------------------------------------------------------------------------------------
	//显示当天天气数据
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskPlaceTxt.txt=\"%s\"", weather.name);	
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNowTempTxt.txt=\"%s℃\"", weather.temperature);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskHumiTxt.txt=\"%d%%\"", humidity);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskWindDirect.txt=\"%s\"", weather.wind_direction[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskWindScale.txt=\"%s级\"", weather.wind_scale[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTxt.txt=\"%s\"", wday[g_week[0]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTempH.txt=\"%s℃\"", weather.high[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTempL.txt=\"%s℃\"", weather.low[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayWeath.txt=\"%s\"", weather.text_day[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//显示明天天气数据
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTxt.txt=\"%s\"", wday[g_week[1]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTempH.txt=\"%s℃\"", weather.high[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTempL.txt=\"%s℃\"", weather.low[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorWeath.txt=\"%s\"", weather.text_day[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//显示后天天气数据
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTxt.txt=\"%s\"", wday[g_week[2]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTempH.txt=\"%s℃\"", weather.high[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTempL.txt=\"%s℃\"", weather.low[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextWeath.txt=\"%s\"", weather.text_day[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//显示温馨提示
	if(atoi(weather.temperature)>=20)
	{
		memset(pbuf, 0, sizeof(pbuf));
		sprintf(pbuf, "Desk.DeskRemindTxt.txt=\"%s\"", g_high_suggestion);
		HMISendStr(pbuf);
		HMISendByte(0xff);
	}
	else
	{
		memset(pbuf, 0, sizeof(pbuf));
		sprintf(pbuf, "Desk.DeskRemindTxt.txt=\"%s\"", g_low_suggestion);
		HMISendStr(pbuf);
		HMISendByte(0xff);
	}
}

/********************************************************************************************************
** 函数: DisplayWeatherIcon, 更新天气图标至液晶触摸串口屏
**------------------------------------------------------------------------------------------------------
** 参数: weather：获取到的天气数据
** 返回: 无		
** 说明: 刷图指令：pic x,y,picid  (x:起始点x坐标，y:起始点y坐标，picid:图片ID)
** 作者: 2017.12.18 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char icon_buf[32] = {0};
void DisplayWeatherIcon(Result_T weather)
{
	if(strcmp(g_page,"page_Desk")==0)
	{
		printf("page Desk!\n");
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", Today_Icon_X, Today_Icon_Y, Today_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", Tomorrow_Icon_X, Tomorrow_Icon_Y, Tomorrow_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", AfterTomor_Icon_X, AfterTomor_Icon_Y, AfterTomor_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
	}
}

/********************************************************************************************************
** 函数: DisplayWifiIcon, 更新wifi图标至液晶触摸串口屏
**------------------------------------------------------------------------------------------------------
** 参数: weather：获取到的天气数据
** 返回: 无		
** 说明: 刷图指令：pic x,y,picid  (x:起始点x坐标，y:起始点y坐标，picid:图片ID)
** 作者: 2018.1.7 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char wifi_icon_buf[32] = {0};
void DisplayWifiIcon(uint8 connect_sta)
{
	memset(wifi_icon_buf, 0, sizeof(wifi_icon_buf));
	if(connect_sta)
	{
		sprintf(wifi_icon_buf, "pic %d,%d,%d", Wifi_Icon_X, Wifi_Icon_Y, Wifi_ConnectIcon_ID);
	}
	else
	{
		sprintf(wifi_icon_buf, "pic %d,%d,%d", Wifi_Icon_X, Wifi_Icon_Y, Wifi_DisconnIcon_ID);
	}
	HMISendStr(wifi_icon_buf);
	HMISendByte(0xff);
}

/********************************************************************************************************
** 函数: Display, 测试用--用于检测天气数据是否保存成功
**------------------------------------------------------------------------------------------------------
** 参数: result:解析后所需的天气数据
** 返回: 无	
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void DisplayTest(Result_T result)
{
	printf("---------------------------------Display Test-------------------------------\n");
	printf("name:%s\n",weather_data.name);
	printf("text:%s\n",weather_data.text);
	printf("code:%s\n",weather_data.code);
	printf("temperature:%s\n",weather_data.temperature);
	
	printf("date[0]:%s\n",weather_data.date[0]);
	printf("text_day[0]:%s\n",weather_data.text_day[0]);
	printf("code_day[0]:%s\n",weather_data.code_day[0]);
	printf("code_night[0]:%s\n",weather_data.code_night[0]);
	printf("high[0]:%s\n",weather_data.high[0]);
	printf("low[0]:%s\n",weather_data.low[0]);
	printf("wind_speed[0]:%s\n",weather_data.wind_speed[0]);
	printf("wind_scale[0]:%s\n",weather_data.wind_scale[0]);
	
	printf("date[1]:%s\n",weather_data.date[1]);
	printf("text_day[1]:%s\n",weather_data.text_day[1]);
	printf("code_day[1]:%s\n",weather_data.code_day[1]);
	printf("code_night[1]:%s\n",weather_data.code_night[1]);
	printf("high[1]:%s\n",weather_data.high[1]);
	printf("low[1]:%s\n",weather_data.low[1]);
	printf("wind_speed[1]:%s\n",weather_data.wind_speed[1]);
	printf("wind_scale[1]:%s\n",weather_data.wind_scale[1]);
	
	printf("date[2]:%s\n",weather_data.date[2]);
	printf("text_day[2]:%s\n",weather_data.text_day[2]);
	printf("code_day[2]:%s\n",weather_data.code_day[2]);
	printf("code_night[2]:%s\n",weather_data.code_night[2]);
	printf("high[2]:%s\n",weather_data.high[2]);
	printf("low[2]:%s\n",weather_data.low[2]);
	printf("wind_speed[2]:%s\n",weather_data.wind_speed[2]);
	printf("wind_scale[2]:%s\n",weather_data.wind_scale[2]);
}
