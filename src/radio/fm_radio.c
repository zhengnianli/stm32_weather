/*
*********************************************************************************************************
*
*	模块名称 : FM收音机主程序模块。
*	文件名称 : fm_radio.c
*	版    本 : V2.0
*	说    明 : 本模块实现调频收音机功能。
*
*		安富莱开发板配置的FM收音机芯片为TEA5767，该芯片挂在I2C总线上。
*		I2C总线通过模拟IO方式实现。TEA5767的音频输出连接到CODEC芯片WM8978的LINE输入端，因此
*		需要同时驱动WM8978才能从耳机或者板载扬声器输出音频信号。
*
*		操作提示：
*			[TAMPER]键     = 扬声器/耳机切换
*			[WAKEUP]键     = 扫描电台
*			[USER]键       = 静音打开/静音关闭
*			摇杆上/下键    = 调节音量
*			摇杆左/右键    = 向下搜台/向上搜台
*			摇杆OK键       = 自动搜台/手动搜台
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-08-27 armfly  ST固件库V3.5.0版本。
*		v2.0    2011-10-16 armfly  优化工程结构。取消TEA5767的硬件自动搜索功能，采用软件搜索判断。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "fm_radio.h"

#define LINE_CAP	18		/* 定义行间距 */

#define STR_Title	"FM调频收音机(TEA5767 + WM8978)"
#define STR_Help1	"[TAMPER]键     = 扬声器/耳机"
#define STR_Help3	"[USER]键       = 静音打开/静音关闭"
#define STR_Help4	"摇杆上/下键    = 调节音量"
#define STR_Help5	"摇杆左/右键    = 向下搜台/向上搜台"
#define STR_Help6	"摇杆OK键       = 自动搜台/手动搜台"

#define STR_HelpScan "请插上耳机, 然后按WAKEUP键自动搜索电台"

RADIO_T g_tRadio;
TEA5767_T g_tTEA;

static void DispStatus(void);
static void AutoScan(void);
static void DispSaveFreq(void);

/*
*********************************************************************************************************
*	函 数 名: RadioInit
*	功能说明: 收音机初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RadioInit(void)
{
	if (i2c_CheckDevice(TEA5767_SLAVE_ADDRESS) != 0)
	{
		printf("Don't Find TEA5767\n");
	}

	/* 初始化全局变量 */
	g_tRadio.ucSpkEn = 0;		/* 扬声器输出 */
	g_tRadio.ucMuteOn = 0;		/* 静音关闭 */
	g_tRadio.ucAutoMode = 1;	/* 自动搜台模式使能 */
	g_tRadio.ucVolume = 30;		/* 缺省音量 */
	g_tRadio.ulFreq = 100100000;	/* 美国/欧洲(87.5-108M), 日本(76-91MHz) */
	g_tRadio.ucAdcLevelSet = ADC_LEVEL_7;
	tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, ADC_LEVEL_7);
	PrintfToHMI("Radio Init..." ,"page_Init", "InitRadioTxt", "txt");
}

/*
*********************************************************************************************************
*	函 数 名: AutoScan
*	功能说明: 自动扫描电台，收到的电台保存到全局变量
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
/*
	武汉市中心程序可搜索的电台： 15个
	89.6	90.7	91.2	92.7	93.6	95.6	96.0	97.8
	99.8	101.8	102.6	103.8	104.6	105.8	107.8
*/
static void AutoScan(void)
{
	uint8_t ucLevelBuf[FREQ_COUNT];	/* 存储每个频点的ADC值，每隔20ms扫描一次 */
	uint8_t ucPos = 0;
	uint16_t i;
	uint8_t ucFirst, ucSecond;
	uint8_t k;
	char buf[100];

	memset(buf, ' ', 400 / 8);
	buf[400 / 8] = 0;

	printf("正在搜索电台...            \n");

   	g_tRadio.ulFreq	=  FM_FREQ_MIN;

	for (i = 0; i < FREQ_COUNT; i++)
	{
		tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, g_tRadio.ucAdcLevelSet);

		delay_ms(30);
		for (k = 0; k < 3; k++)
		{
			delay_ms(5);
			tea5767_ReadStatus(&g_tTEA);	/* 读取TEA5767的状态 */
			ucFirst = g_tTEA.ucAdcLevel; 

			if (ucFirst < 6)
			{
				break;
			}

			delay_ms(5);
			ucSecond = g_tTEA.ucAdcLevel;
			
			if (ucFirst == ucSecond)
			{
				break;
			} 
		}
		ucLevelBuf[i] = ucSecond;
		
		g_tRadio.ulFreq += FREQ_STEP;
	}

	/* 最多保存32个电台 */
	for (i = 0; i < FREQ_COUNT; i++)
	{
		if ((i == 0) || (i == FREQ_COUNT - 1))
		{
			/* 2头2个频点丢弃*/;			
		}
		else
		{
			if ((ucLevelBuf[i] > 5) &&
				(ucLevelBuf[i] > ucLevelBuf[i - 1]) && (ucLevelBuf[i] > ucLevelBuf[i + 1]))
			{
				g_tRadio.ulSaveFreq[ucPos++] = FM_FREQ_MIN + i * FREQ_STEP;
			}
		}
		if (ucPos >= FM_SAVE_COUNT)
		{
			break;
		}
	}
	g_tRadio.ucSaveCount = ucPos; 	/* 电台个数 */

   	/* 缺省播放第一个电台 */
	g_tRadio.ucSavePos = 0;
	g_tRadio.ulFreq = g_tRadio.ulSaveFreq[g_tRadio.ucSavePos]; 	
	tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, g_tRadio.ucAdcLevelSet);
}

/*
*********************************************************************************************************
*	函 数 名: DispSaveFreq
*	功能说明: 显示自动搜索到的电台频率
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/							
static void DispSaveFreq(void)
{
	char buf[100];
	uint16_t pos;
	uint8_t i;

	memset(buf, ' ', 400 / 8);
	buf[400 / 8] = 0;

	/* 显示搜索到的电台个数 */
	sprintf(buf, "已搜索到的电台个数: %d  ",g_tRadio.ucSaveCount);

	pos = 0;
	memset(buf, 0, sizeof(buf));
   	for (i = 0; i < 8; i++)
	{
		sprintf(&buf[pos], "%3d.%d ",
			g_tRadio.ulSaveFreq[i] / 1000000, (g_tRadio.ulSaveFreq[i] % 1000000) / 100000);

		pos = strlen(buf);
	}

	pos = 0;
	memset(buf, 0, sizeof(buf));
   	for (i = 8; i < 16; i++)
	{
		if (i == g_tRadio.ucSaveCount)
		{
			break;
		}
		sprintf(&buf[pos], "%d.%d ",
			g_tRadio.ulSaveFreq[i] / 1000000, (g_tRadio.ulSaveFreq[i] % 1000000) / 100000);

		pos = strlen(buf);
	}
}

/*
*********************************************************************************************************
*	函 数 名: DispStatus
*	功能说明: 显示当前状态，频率，音量等
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispStatus(void)
{
	uint16_t y, x1, x2;
	char buf[80];
	

	x1 = 20;	/* 状态栏第1列X坐标 */
	x2 = 200;	/* 状态栏第2列X坐标 */
	y = LINE_CAP * 11;

	if (g_tRadio.ucSearch == 1)
	{
		if (g_tRadio.ucDirection == SEARCH_UP)
		{
			sprintf((char *)buf, "频率 = >>>>>>>>    ");
		}
		else
		{
			sprintf((char *)buf, "频率 = <<<<<<<<    ");
		}
	}
	else
	{
		#if 1
		sprintf((char *)buf, "频率 = %d.%d兆赫  ",
			g_tRadio.ulFreq / 1000000, (g_tRadio.ulFreq % 1000000) / 100000);
		#else
		sprintf((char *)buf, "频率 = %dHz    ",	g_tRadio.ulFreq);
		#endif
	}

	sprintf((char *)buf, "音量 = %d ", g_tRadio.ucVolume);

	y += LINE_CAP;

	if (g_tRadio.ucAutoMode == 1)
	{
		printf("搜台 = 自动\n");
	}
	else
	{
		printf("搜台 = 手动\n");
	}

	sprintf((char *)buf, "信号强度  = %d ",g_tTEA.ucAdcLevel);
}
