#include "rtc.h"


//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31

Calendar_T calendar_t;	// 日历结构体

/* 月份x修正数据表 */											 
uint8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 
/* 平年的月份日期表 */
const uint8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
/* 星期数组 */
char *wday[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

/********************************************************************************************************
** 函数: RTC_NVIC_Config,  RTC中断配置，供RTC_Init函数调用
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;				//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);							
}

/********************************************************************************************************
** 函数: RTC_Init,  RTC初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 0（成功），其他（失败）											  
********************************************************************************************************/
uint8 RTC_Init(void)
{
	//检查是不是第一次配置时钟
	uint8 temp = 0;
	
	/* 使能PWR和BKP外设时钟  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
	
	/* 使能RTC和后备寄存器访问(取消备份区域写保护) */
	PWR_BackupAccessCmd(ENABLE);	
	
	/* 从指定的后备寄存器中读出数据:读出与写入的指定数据是否相等 */
	if (BKP_ReadBackupRegister(BKP_DR1) != 0XA0A0)		
	{	 			
		BKP_DeInit();				// 复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	// 设置外部低速晶振(LSE),使用外设低速晶振
		/* 检查指定的Rtc_Flag设置与否,等待低速晶振就绪 */
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=250)return 1;					 // 初始化时钟失败,晶振有问题	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	 // 设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);					 // 使能RTC时钟  
		RTC_WaitForLastTask();					 // 等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();					 // 等待RTC寄存器与RTC的APB时钟同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		 // 使能RTC秒中断
		RTC_WaitForLastTask();					 // 等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();  				 // 允许配置	
		RTC_SetPrescaler(32767);				 // 设置RTC预分频的值
		RTC_WaitForLastTask();					 // 等待最近一次对RTC寄存器的写操作完成
//		RTC_Set(2017,10,20,21,51,40);    		 // 设置时间
		RTC_Set(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec);    		 // 设置时间
		RTC_ExitConfigMode();   				 // 退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0XA0A0);// 向指定的后备寄存器中写入用户程序数据
	}
	/* 系统继续计时 */
	else
	{
		RTC_WaitForSynchro();					 // 等待RTC寄存器与RTC的APB时钟同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		 // 使能RTC秒中断
		RTC_WaitForLastTask();				     // 等待最近一次对RTC寄存器的写操作完成
	}
	RTC_NVIC_Config();							 // RTC中断分组设置		    				     
	RTC_Get();									 // 更新时间	
	PrintfToHMI("RTC Init" ,"page_Init", "InitRTCTxt", "txt");
	return 0; 									 // 初始化成功
}

/********************************************************************************************************
** 函数: IsLeapYear,  判断闰年、平年
**------------------------------------------------------------------------------------------------------
** 参数: year
** 返回: 0（平年），1（闰年）		
** 注意: 闰年判断条件:1.能被4整除，不能被100整除；2.能被4整除，又能被400整除
********************************************************************************************************/
uint8 IsLeapYear(uint16 year)
{
	if( year%4==0 )
	{
		if( year%100==0 )
		{
			if( year%400==0 )
			{
				return 1;//闰年
			}
			else
			{
				return 0;//平年
			}
		}
		else
		{
			return 1;	 //闰年
		}
	}
	else
	{
		return 0;        //平年
	}
}

/********************************************************************************************************
** 函数: RTC_Get,  得到当前的时间
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 0（成功），其他（错误）											  
********************************************************************************************************/
uint8 RTC_Get(void)
{
	static uint16 daycnt = 0;
	uint32 rtc_count = 0;    		// 用于接收RTC计数器值
	uint32 day_num = 0;		 		// 1970年1月1日至今过了多少天
	uint16 year = 0;				// 年数
	
    rtc_count = RTC_GetCounter();	// 获取RTC计数器的值
 	day_num = rtc_count/86400;      // 得到天数(秒钟数对应的)
	if( daycnt!=day_num )			// 超过一天了
	{	  
		daycnt = day_num;
		year = 1970;				// 从1970年开始
		while( day_num >= 365 )		// 天数超过了1年就记年份
		{				 
			if( IsLeapYear(year) )  // 1是闰年，0是平年
			{
				if( day_num >= 366 )
				{
					day_num -= 366; //总天数减去闰年366天之后年数加1
				}
				else 
				{
					year++;
					break;
				}  
			}
			else day_num -= 365;    // 总天数减去平年365天之后年数加1
			year++;  
		}   
		calendar_t.w_year = year;	// 得到年份
		year = 0;
		while(day_num>=28)			// 超过了一个月就记月份
		{
			if( IsLeapYear(calendar_t.w_year)&&year==1 )//当年是不是闰年2月份
			{
				if( day_num >= 29 )
					day_num -= 29;	//减去年份的天数之后的总天数减去闰年2月29天之后月份加1
				else 
					break; 
			}
			else 
			{
				if( day_num >= mon_table[year] )
					day_num -= mon_table[year];//平年
				else break;
			}
			year++;  
		}
		calendar_t.w_month = year+1;	//得到月份
		calendar_t.w_date  = day_num+1; //得到日期 
	}
	day_num=rtc_count%86400;     		//一整天之内的秒钟数   	   
	calendar_t.hour=day_num/3600;     	//小时
	calendar_t.min=(day_num%3600)/60; 	//分钟	
	calendar_t.sec=(day_num%3600)%60; 	//秒钟
	calendar_t.week=RTC_GetWeek(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date);//获取星期   
	return 0;
}

/********************************************************************************************************
** 函数: RTC_GetWeek,  获得现在是星期几
**------------------------------------------------------------------------------------------------------
** 参数: uint16 year,uint8 month,uint8 day
** 返回: 星期号											  
********************************************************************************************************/
uint8 RTC_GetWeek(uint16 year,uint8 month,uint8 day)
{
	uint16 week;
	uint8 yearH,yearL;
	
	yearH = year/100;	
	yearL = year%100; 
	// 如果为21世纪,年份数加100  
	if ( yearH>19 )
		yearL += 100;
	// 所过闰年数只算1900年之后的  
	week = yearL+yearL/4;
	week = week%7; 
	week = week+day+table_week[month-1];
	if( yearL%4==0&&month<3 )
		week--;
	return (week%7);
}

/********************************************************************************************************
** 函数: RTC_Set,  RTC设置日期时间函数（以1970年1月1日为基准，把输入的时钟转换为秒钟）
**------------------------------------------------------------------------------------------------------
** 参数: uint16 year,uint8 mon,uint8 day,uint8 hour,uint8 min,uint8 sec
** 返回: 0（成功），1（失败）											  
********************************************************************************************************/
uint8 RTC_Set(uint16 year,uint8 mon,uint8 day,uint8 hour,uint8 min,uint8 sec)
{
	uint16 t;
	uint32 seccount=0;
	
	/* 求1970至今的秒钟数，用于设置年份 */
	if( year<1970||year>2099 )	
		return 1;	 
	for(t=1970;t<year;t++)	//把所有年份的秒钟相加
	{
		if(IsLeapYear(t))
		{
			seccount += 31622400;    //闰年的秒钟数
		}
		else 
		{
			seccount += 31536000;  //平年的秒钟数
		}
	}
	
	/* 求某年1月至当月的秒钟数，用于设置月份 */
	mon -= 1; //mon_table数组的索引
	for( t=0; t<mon; t++)	   //把前面月份的秒钟数相加
	{
		seccount += (u32)mon_table[t]*86400;
		if(IsLeapYear(year)&&t==1)
		{
			seccount += 86400;//闰年2月份增加一天的秒钟数	
		}			
	}
	
	/* 求某月1号至今天的秒钟数，用于设置号数 */
	seccount += (u32)(day-1)*86400;
	
	/* 求某天0时至当前小时的秒钟数，用于设置小时 */
	seccount += (u32)hour*3600;
	
	/* 求某小时0分至当前分钟的秒钟数，用于设置分钟 */
    seccount += (u32)min*60;	 
	
	/* 求某分钟0秒至当前秒的秒钟数，用于设置秒钟 */
	seccount += sec;
	
	/* 使能PWR和BKP外设时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	 
	PWR_BackupAccessCmd(ENABLE);// 使能RTC和后备寄存器访问 
	RTC_SetCounter(seccount);	// 设置RTC计数器的值
	RTC_WaitForLastTask();	    // 等待最近一次对RTC寄存器的写操作完成  	
	
	return 0;	    
}

/********************************************************************************************************
** 函数: DisplayTime,  液晶屏显示时间
**------------------------------------------------------------------------------------------------------
** 参数: time:日历结构体变量
** 返回: 无		
** 作者: 2017.12.13 by Lizn
********************************************************************************************************/
char time_buf[32] = {0};
void DisplayTime(Calendar_T time)
{
	//------------------------------------------------------------------------------------------
	//打印年月日时分秒星期至串口屏
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskYearTxt.txt=\"%.4d\"", time.w_year);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskMonthTxt.txt=\"%.2d\"", time.w_month);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskDayTxt.txt=\"%.2d\"", time.w_date);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskHourTxt.txt=\"%.2d\"", time.hour);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskMinTxt.txt=\"%.2d\"", time.min);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskSecTxt.txt=\"%.2d\"", time.sec);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskWeekTxt.txt=\"%s\"", wday[time.week]);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
}

u8 g_time_flag = 0;       //秒中断标志位
u8 WeatherIconFlag = TRUE;
/********************************************************************************************************
** 函数: RTC_IRQHandler,  RTC中断处理函数
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无								  
********************************************************************************************************/
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
	{	
		RTC_Get();		 //更新时间  
		//------------------------------------------------------------------------------------------
		//串口显示
		printf("RTC Time:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%s\r\n",
		calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,
		calendar_t.hour,calendar_t.min,calendar_t.sec,wday[calendar_t.week]);
		//------------------------------------------------------------------------------------------
		//液晶显示
		DisplayTime(calendar_t);
 	}			  								 
	RTC_ClearITPendingBit(RTC_IT_SEC);// 清秒中断
	RTC_WaitForLastTask();	  	    			// 等待最近一次对RTC寄存器的写操作完成  				 	   	 
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
