/*************端口信息********************
 * 接线说明
 * P2/SDCK  PB3
 * P1/SDO   PB4
 * P0/SDI   PB5
 * RST      PB1
 * A0		PB2
 * CS   	PB8
 * WR/SPIS  PB9
 * IRQ      PF6
 * RD       PA0
*****************************************/

#include "LDchip.h"
#include "Reg_RW.h"	
#include "LD3320_main.h"

char g_WeatherText[200] = {0};


static void LD3320_EXTI_Cfg(void);
static void LD3320_Spi_cfg(void);
static void LD3320_GPIO_Cfg(void);
static void CheckReadWrite(void);

extern int SwithToUtf_8(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

/***********************************************************
* 名    称：LD3320_Init(void)
* 功    能：模块驱动端口初始配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_Init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_EXTI_Cfg();
	LD3320_Spi_cfg();
//	CheckReadWrite();	//测试LD3320读写的函数，可屏蔽
	LD_reset();
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	PrintfToHMI("ASR Init..." ,"page_Init", "InitASRTxt", "txt");
}

/*******************************************************************************************************
** 函数: CheckReadWrite，测试通信是否正常
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2018.1.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void CheckReadWrite(void)
{
	uint8 RegValue;
	
	LD_reset();
	/* 检测通信是否正常 */
	printf("检测通信是否正常：\n");
	RegValue = LD_ReadReg(0x06);	// 0x06寄存器：读FIFO状态
	LD_WriteReg(0x35,0x33);			// 0x35寄存器：ADC增益
	LD_WriteReg(0x1b,0x55);			// 0x1b寄存器：时钟频率设置
	LD_WriteReg(0xb3,0xaa);			// 0xb3寄存器：语音断点检测
	RegValue = LD_ReadReg(0x35);
	printf("RegValue(0x35) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0x1b);
	printf("RegValue(0x1b) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0xb3);
	printf("RegValue(0xb3) = %#x\n",RegValue);
	LD_reset();
	/* 检查寄存器初值 */
	printf("寄存器初值为：\n");
	RegValue = LD_ReadReg(0x06);
	printf("RegValue(0x06) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0x35);
	printf("RegValue(0x35) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0xb3);
	printf("RegValue(0xb3) = %#x\n",RegValue);
}

/***********************************************************
* 名    称： void Delay_( int i)
* 功    能： 短延时
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void Delay_( int i)
{     
	while( i--)
	{	

	}
}	
/***********************************************************
* 名    称：	LD3320_delay(unsigned long uldata)
* 功    能：	长延时函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void  LD3320_delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<5;j++)
	{
		for (g=0;g<uldata;g++)
		{
			Delay_(120);
		}
	}
}

/***********************************************************
* 名    称：	RunASR(void)
* 功    能：	运行ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_AsrStart();			//(1)初始化ASR
		LD3320_delay(100);
		if (LD_AsrAddFixed()==0)//(2)添加关键词语到LD3320芯片中
		{
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			LD3320_delay(50);	//	并从初始化开始重新ASR识别流程
			continue;
		}

		LD3320_delay(10);

		if (LD_AsrRun() == 0)	//(3)启动ASR运算
		{
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			LD3320_delay(50);	//	并从初始化开始重新ASR识别流程
			continue;
		}

		asrflag=1;	//启动成功
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}

	return asrflag;	//启动失败
}

/***********************************************************
* 名    称：LD3320_GPIO_Cfg(void)
* 功    能：初始化需要用到的IO口
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
static void LD3320_GPIO_Cfg(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	//io口配置
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);
	//LD_CS/A0/RSET
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_2|GPIO_Pin_1;//;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//LD_RD
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_2);	/*A0默认拉高*/
	GPIO_SetBits(GPIOA,GPIO_Pin_0);	/*RD默认拉高*/
}
/***********************************************************
* 名    称：LD3320_Spi_cfg(void)
* 功    能：配置SPI功能和端口初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
static void LD3320_Spi_cfg(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    //spi端口配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3 | RCC_APB2Periph_GPIOB,ENABLE);	   //使能SPI3外设时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	//P0/P1/P2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//WR 、CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	LD_CS_H();
	
	//spi功能配置
	SPI_Cmd(SPI3, DISABLE);
	/* SPI3 配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   //时钟极性 空闲状态时，SCK保持低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //波特率控制 SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7
	SPI_Init(SPI3, &SPI_InitStructure);
	/* 使能SPI3 */
	SPI_Cmd(SPI3, ENABLE);

}
/***********************************************************
* 名    称： LD3320_EXTI_Cfg(void) 
* 功    能： 外部中断功能配置和相关端口配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
static void LD3320_EXTI_Cfg(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//中断引脚配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//外部中断线配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line6);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_6);	 //默认拉高中断引脚

	EXTI_ClearFlag(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line6);
	//中断嵌套配置
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/***********************************************************
* 名    称：  EXTI9_5_IRQHandler(void)
* 功    能： 外部中断函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6)!= RESET ) 
	{
 		ProcessInt0(); 	//	识别处理函数
		printf("进入中断\r\n");	
		EXTI_ClearFlag(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

/*******************************************************************************************************
** 函数: ASRSuccess_Handle，识别成功，执行相应操作
**------------------------------------------------------------------------------------------------------
** 参数: asr_code：识别码
** 返回: 无
** 作者: 2018.1.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 RunFlag = FALSE;
void ASRSuccess_Handle(uint8 asr_code)
{
	printf("\r\n识别码:%d\n",asr_code);		
	if(0 == asr_code)
	{
		printf("我在，需要我的帮助吗？\n");
		TTSPlay(0, "[t3][2]我在,[2]需要[2]我的[3]帮助吗");
		RunFlag = TRUE;
	}
	else if(RunFlag)
	{
		RunFlag = FALSE;
		/* 识别码0-10为搜索天气识别码 */
		if(asr_code>=0&&asr_code<=10)
		{
			switch(asr_code)		  
			{
				case CODE01:			
					printf("“福州”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索福州天气");
					memcpy(g_city,"fujianfuzhou",sizeof(g_place));
					break;
				case CODE02:	 
					printf("“上海”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索上海天气");			
					memcpy(g_city,"shanghai",sizeof(g_place));
					break;
				case CODE03:		
					printf("“深圳”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索深圳天气");	
					memcpy(g_city,"shenzhen",sizeof(g_place));
					break;
				case CODE04:		
					printf("“北京”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索北京天气");	
					memcpy(g_city,"beijing",sizeof(g_place));
					break;
				case CODE05:		
					printf("“广州”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索广州天气");	
					memcpy(g_city,"guangzhou",sizeof(g_place));
					break;
				case CODE06:		
					printf("“南宁”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索南宁天气");	
					memcpy(g_city,"nanning",sizeof(g_place));
					break;
				case CODE07:		
					printf("“厦门”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索厦门天气");	
					memcpy(g_city,"xiamen",sizeof(g_place));
					break;
				case CODE08:		
					printf("“泉州”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索泉州天气");	
					memcpy(g_city,"quanzhou",sizeof(g_place));
					break;
				case CODE09:		
					printf("“莆田”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索莆田天气");	
					memcpy(g_city,"putian",sizeof(g_place));
					break;
				case CODE10:	
					printf("“南平”命令识别成功\r\n");
					TTSPlay(0, "[t3][2]小天正在为您搜索南平天气");	
					memcpy(g_city,"nanping",sizeof(g_place));
					break;
			}
			memset(&weather_data, 0, sizeof(weather_data));
			GET_NowWeather();
			GET_DailyWeather();
			GetWeatherTimer = TIMER1_HOUR;	
			DisplayWeather(weather_data);
			DisplayWeatherIcon(weather_data);
		}
		else
		{
			switch(asr_code)	
			{
				case CODE11:
					printf("“语音播报天气”命令识别成功\r\n");
					printf("%s\n",g_WeatherText);
//					TTSPlay(0, (uint8_t*)g_WeatherText);
					break;
				case CODE12:
					printf("“今天的气温是多少”命令识别成功\r\n");
					break;
				default:
					TTSPlay(0, "语音识别失败，请对准麦克风说话！");
					break;
			}
		}
		
	}
}