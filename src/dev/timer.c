#include "timer.h"

uint16 Uart2CharRecTimer =0;

/********************************************************************************************************
** 函数: TIM1_Init, 通用定时器1初始化
**------------------------------------------------------------------------------------------------------
** 参数: arr：自动重装值 psc：时钟预分频数
** 说明: 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft
** 返回: void	
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM1_Init(uint16 arr,uint16 psc)
{
	int i;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //	/* 自动重装载寄存器周期的值(计数值) */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* 对外部时钟进行采样的时钟分频,这里没有用到 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //
 	TIM_ClearFlag(TIM1,TIM_FLAG_Update );
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //
 
	//中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel =  TIM1_UP_IRQn;;//指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道使能
	NVIC_Init(&NVIC_InitStructure);  //初始化寄存器
	TIM_Cmd(TIM1, ENABLE);  //使能TIMX		
    
	for(i=0;i< MAX_TIMER ;i++)
	 g_Timer1[i] = 0;   //全局定时器初始化
}


/********************************************************************************************************
** 函数: TIM2_Init, 通用定时器2初始化
**------------------------------------------------------------------------------------------------------
** 参数: arr：自动重装值 psc：时钟预分频数
** 说明: 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft
** 返回: void	
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM2_Init(uint16 arr,uint16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM2时钟使能    
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	TIM_Cmd(TIM2,ENABLE);	//开启定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

/********************************************************************************************************
** 函数: TIM3_Init, 通用定时器3初始化
**------------------------------------------------------------------------------------------------------
** 参数: arr：自动重装值 psc：时钟预分频数
** 说明: 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft
** 返回: void	
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM3_Init(uint16 arr,uint16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //	/* 自动重装载寄存器周期的值(计数值) */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* 对外部时钟进行采样的时钟分频,这里没有用到 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //
 	TIM_ClearFlag(TIM3,TIM_FLAG_Update );
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //
 
	//中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel =  TIM3_IRQn;;//指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道使能
	NVIC_Init(&NVIC_InitStructure);  //初始化寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIMX			 
}

/********************************************************************************************************
** 函数: TIM1_IRQHandler,  定时器1中断服务程序
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无	
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	uint8 i;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM1更新中断发生与否
	{
		//-------------------------------------------------------------------------------
		// 各种定时间器计时
		for (i = 0; i < MAX_TIMER; i++)					// 定时时间递减							
		if( g_Timer1[i] ) g_Timer1[i]-- ;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );   //清除TIMx更新中断标志 
	}
} 

/********************************************************************************************************
** 函数: TIM2_IRQHandler,  定时器2中断服务程序
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无		
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/	    
void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		if(Uart2CharRecTimer) Uart2CharRecTimer --;
		else if((USART2_RX_STA>0)&& (( USART2_RX_STA & (1<<15))==0))  
		{
			USART2_RX_STA|=1<<15;				//强制标记接收完成
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIM2更新中断标志    
	}	    
}

/********************************************************************************************************
** 函数: TIM3_IRQHandler,  定时器3中断服务程序
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无	
** 作者: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM1更新中断发生与否
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
	}
} 
