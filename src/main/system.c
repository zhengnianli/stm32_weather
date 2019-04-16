/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: system.c 
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**      系统的初始化
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "system.h"

char Init_buf[32] = {0};

void  LD3320_main(void);
extern int SwithToUtf_8(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);
//供本文件调用的函数
static void Task_GetWeather(void);		// 获取天气数据
static void Task_RecHMIDate(void);		// 接收处理串口屏下发的数据
static void Task_DisplayWeather(void);	// 更新天气数据至串口屏
static void Task_UpdateIcon(void);		// 更新天气现象图标至串口屏
static void Task_TimeCalibration(void);	// 时间校准
static void Task_DHT11Collect(void);	// 采集温湿度数据
static void Task_ASR(void);				// 语音识别
static void InitSuccess(void);			// 初始化成功

/*******************************************************************************************************
** 函数: CpuInit,配置系统信息
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
** 说明: system_stm32f10x.c内的SystemInit函数已经有对系统一些参数配置，并由汇编启动函数入口调用
		 本处做一些补充,改函数必须在其他函数之前调用
********************************************************************************************************/
void CpuInit( void )
{
  	//-------------------------------------------------------------------------------
	// 关闭调试端口重新映射 设置中断优先级
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  		// 关闭调试 端口重新映射
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);					// 选择第3组的中断优先级
}

/*******************************************************************************************************
** 函数: PrintfInfo，打印工程标题
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
********************************************************************************************************/
void PrintfInfo(void)
{
	printf("----------------------| 本科毕业设计 |----------------------\n");
	printf("\n\n");
	printf("                     桌面天气预报系统                        \n");
	printf("\n\n");
	printf("------------------------------------------------------------\n");
}

/*******************************************************************************************************
** 函数: SysInit，上电初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void SysInit(void)
{
	SysTick_Init(72);									// 初始化系统时钟
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);		// 选择第3组的中断优先级
	SYN6288_UART4_Init(9600);							// 初始化TTS模块
	delay_ms(1000);										// 初始化后延时1S左右，否则语音可能播报不了
	TTSPlay(0,"我是一个桌面天气预报系统，[2]我叫[2]小天");	// 播报
	HMI_USART3_init(9600);								// 串口屏初始化,默认波特率为9600!!
	my_mem_init(SRAMIN);								// 初始化内部内存池
	USART1_Init(9600);      							// 串口1初始化
	PrintfInfo();										// 打印工程标题
	RTC_Init();											// RTC初始化
	ESP8266_USART2_Init(115200);						// ESP8266 WIFI初始化,默认波特率为115200!!
	TIM1_Init(2000-1, 36000-1);							// 1s溢出一次
	RadioInit();										// 收音机初始化
	DHT11_Init();										// DHT11温湿度传感器GPIO初始化
	LD3320_Init();										// 初始化语音识别模块
	InitSuccess();										// 初始化结束
}

/*******************************************************************************************************
** 函数: BackTask，后台任务
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void BackTask(void)
{
	Task_RecHMIDate();		// 接收串口屏下发的数据
	Task_GetWeather();		// 获取天气预报数据
	Task_DisplayWeather();  // 更新天气数据至串口屏
	Task_UpdateIcon();		// 更新天气图标
	Task_TimeCalibration(); // 时间校准
	Task_DHT11Collect();	// 采集温湿度数据
	Task_ASR();				// 语音识别
}

/*******************************************************************************************************
** 函数: TaskGetWeather，后台任务--获取天气数据,每一个小时抓一次天气预报数据包
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 1:成功 0:失败
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_GetWeather(void)
{
	if( GetWeatherTimer ) return;
	GetWeatherTimer = TIMER1_HOUR;
	memset(&weather_data, 0, sizeof(weather_data));
	GET_NowWeather();
	GET_DailyWeather();
}

/*******************************************************************************************************
** 函数: Task_RecHMIDate，接收串口屏下发的数据
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char *p = NULL;
static void Task_RecHMIDate(void)
{
//	if(GetHMIDataTimer) return;
//	GetHMIDataTimer = 1;
	if(USART3_RX_STA&0x8000)
	{
		/* 页面标识 */
		if((p=strstr((char*)USART3_RX_BUF,"page:"))!=NULL)
		{
			if(sscanf(p,"page:%s",g_page)==1)
			{
				printf("page:%s\n",g_page);
				DisplayWeather(weather_data);
//				DisplayWeatherIcon(weather_data);
				if((g_connect_sta==1)&&(strcmp(g_page,"Desk")==0))
				{
					DisplayWifiIcon(g_connect_sta);
				}
			}
		}
		/* wifi名称 */
		else if((p=strstr((char*)USART3_RX_BUF,"ssid:"))!=NULL)
		{
			if(sscanf(p,"ssid:%s",g_wifi_ssid)==1)
			{
				printf("ssid:%s\n",g_wifi_ssid);
			}
		}
		/* wifi密码 */
		else if((p=strstr((char*)USART3_RX_BUF,"passwd:"))!=NULL)
		{
			if(sscanf(p,"passwd:%s",g_wifi_password)==1)
			{
				printf("passwd:%s\n",g_wifi_password);
				g_connect_sta = ESP8266_ConnectAP(g_wifi_ssid, g_wifi_password);
				printf("g_connect_sta = %d\n",g_connect_sta);
				while(!g_connect_sta);
				DisplayWifiIcon(g_connect_sta);
				GET_BeiJingTime();								// 获取北京时间
				printf("wifi connect success!\n");
				TTSPlay(0, "[t3][2]网络连接成功！");
			}
		}
		/* 搜索方法:China or English */
		else if((p=strstr((char*)USART3_RX_BUF,"SerchMethod:"))!=NULL)
		{
			memset(g_serch_method, 0, 32);
			if(sscanf(p,"SerchMethod:%s",g_serch_method)==1)
			{
				printf("SerchMethod:%s\n",g_serch_method);
			}
		}
		/* 地名 */
		else if((p=strstr((char*)USART3_RX_BUF,"place:"))!=NULL)
		{
			if(sscanf(p,"place:%s",g_place)==1)
			{
				if(strcmp(g_serch_method,"China")==0)
				{
					SwithToUtf_8((const unsigned char*)g_place, strlen((const char*)g_place),(unsigned char*) g_place_utf8, &utf8_len);
					printf("place:%s\n",g_place_utf8);
					printf("place:%X%X%X %X%X%X\n",g_place_utf8[0],g_place_utf8[1],g_place_utf8[2],
					g_place_utf8[3],g_place_utf8[4],g_place_utf8[5]);
					sprintf(g_city,"%%%X%%%X%%%X%%%X%%%X%%%X",g_place_utf8[0],g_place_utf8[1],
					g_place_utf8[2],g_place_utf8[3],g_place_utf8[4],g_place_utf8[5]);
				}
				else if(strcmp(g_serch_method,"English")==0)
				{
					printf("place:%s\n",g_place);
					memcpy(g_city,g_place,sizeof(g_place));
				}
				memset(&weather_data, 0, sizeof(weather_data));
				GET_NowWeather();
				GET_DailyWeather();
				GetWeatherTimer = TIMER1_HOUR;	
				TTSPlay(0, "[t3][2]正在为您显示天气信息！");
				DisplayWeather(weather_data);
				DisplayWeatherIcon(weather_data);
				
			}
		}
		/* 频率整数 */
		else if((p=strstr((char*)USART3_RX_BUF,"freq_int:"))!=NULL)
		{
			if(sscanf(p,"freq_int:%s",g_freq_int)==1)
			{
				printf("freq_int:%s(%d)\n",g_freq_int,g_freq_int[0]);
			}
		}
		/* 频率小数 */
		else if((p=strstr((char*)USART3_RX_BUF,"freq_deci:"))!=NULL)
		{
			if(sscanf(p,"freq_deci:%s",g_freq_deci)==1)
			{
				printf("freq_deci:%s(%d)\n",g_freq_deci,g_freq_deci[0]);
				g_tRadio.ulFreq = g_freq_int[0]*1000000 + g_freq_deci[0]*100000;
				printf("g_tRadio.ulFreq=%d\n",g_tRadio.ulFreq);
				tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, ADC_LEVEL_7);
			}
		}
		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
		p = NULL;
		USART3_RX_STA = 0;
	}
}

/*******************************************************************************************************
** 函数: Task_DisplayWeather，定时更新天气数据至串口屏
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 1:成功 0:失败
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_DisplayWeather(void)
{
	if(SendWeatherTimer) return;
	SendWeatherTimer = 10*TIMER1_SEC;
	DisplayWeather(weather_data);
}

/*******************************************************************************************************
** 函数: Task_UpdateIcon，特定时间更新天气图标至串口屏
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 说明:判断当前时间为凌晨3点至4点间并且处于主界面，并且3点到4点的更新标志为FALSE，
		则执行更新图标操作。更新完之后 ，3点到4点的更新标志改为TRUE，即使还在这个时间
		段也不会更新。主程序可以在凌晨0点左右 修改3点至4点的更新标志为FLASE，即可启动
		下一次更新，要到达对应的时间段，并在主界面
** 作者: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_UpdateIcon(void)
{
	if( 0==calendar_t.hour )
	{
		WeatherIconFlag = FALSE;
	}
	else if( (3==calendar_t.hour) && (FALSE==WeatherIconFlag) )
	{
		DisplayWeatherIcon(weather_data);
		WeatherIconFlag = TRUE;
	}
}

/*******************************************************************************************************
** 函数: Task_TimeCalibration，时间校准，一天校准一次
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 1:成功 0:失败
** 作者: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_TimeCalibration(void)
{
	if(TimeCalibraTimer) return;
	TimeCalibraTimer = TIMER1_DAY;
	GET_BeiJingTime();
}

/*******************************************************************************************************
** 函数: Task_DHT11Collect，DHT11温湿度数据采集，20s采集一次
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 1:成功 0:失败
** 作者: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 temperature=0,humidity=0;
static void Task_DHT11Collect(void)
{
	if(DHT11CollectTimer) return;
	DHT11CollectTimer = 20*TIMER1_SEC;
	DHT11_Read_Data(&temperature, &humidity);
//	printf("temperatur:%d;humidity:%d\n",temperature,humidity);
}

/*******************************************************************************************************
** 函数: Task_ASR，语音识别任务
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 1:成功 0:失败
** 说明:	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
			LD_ASR_NONE:		表示没有在作ASR识别
			LD_ASR_RUNING：		表示LD3320正在作ASR识别中
			LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
			LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
			LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
** 作者: 2018.1.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 nAsrStatus = 0;
uint8 nAsrRes=0;
static void Task_ASR(void)
{
	switch(nAsrStatus)
	{
		case LD_ASR_RUNING:		
		case LD_ASR_ERROR:		
			break;
		case LD_ASR_NONE:
			nAsrStatus=LD_ASR_RUNING; 
			if (RunASR()==0)	//	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
			{		
				nAsrStatus = LD_ASR_ERROR;
			}
			break;

		case LD_ASR_FOUNDOK:
			nAsrRes = LD_GetResult( );	//	一次ASR识别流程结束，去取ASR识别结果	
			ASRSuccess_Handle(nAsrRes);
			nAsrStatus = LD_ASR_NONE;	
		break;
		
		case LD_ASR_FOUNDZERO:
		default:
			nAsrStatus = LD_ASR_NONE;	
			break;
	}
}

/*******************************************************************************************************
** 函数: InitSuccess，系统初始化结束
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2018.1.11 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void InitSuccess(void)
{
	PrintfToHMI("Init Success!" ,"page_Init", "InitSccessTxt", "txt");
	delay_ms(1000);
	HMISendStr("page Desk");
	HMISendByte(0xff);
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
