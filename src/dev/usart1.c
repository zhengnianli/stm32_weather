#include "usart1.h"
	 
/********************************************************************************************************
** 函数: fput,  重定向,在使用printf函数时自动调用
**------------------------------------------------------------------------------------------------------
** 参数: int ch, FILE *p
** 返回: ch											  
********************************************************************************************************/
int fputc(int ch, FILE *p)
{
	USART_SendData(USART1,(uint8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/********************************************************************************************************
** 函数: USART1_Init,  串口1初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
void USART1_Init(uint32 bound)
{
	/* 声明相关结构体变量 */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 相关时钟使能 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;		       // 串口输出PA9(TX)
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;    // IO口频率
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	   // 复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  			   // 初始化
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;		   // 串口输入PA10(RX)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;// 浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); 			   // 初始化 
	
	/* USART1 初始化设置 */
	USART_InitStructure.USART_BaudRate   = bound;				// 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据长度 
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    // 1位停止位
	USART_InitStructure.USART_Parity	 = USART_Parity_No;		// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;//收发模式
	USART_Init(USART1, &USART_InitStructure);// 初始化
	
	/* 使能串口1 */
	USART_Cmd(USART1, ENABLE);
	
	/* 清除发送标志位 */
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	/* 开启接收中断使能 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* NVIC配置 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	   // 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;	   // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		   // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);						   // 初始化
	PrintfToHMI("USART1 Init..." ,"page_Init", "InitUsart1Txt", "txt");
}

/********************************************************************************************************
** 函数: USART1_IRQHandler,  串口1中断服务函数
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
void USART1_IRQHandler(void)
{
	uint8 rec;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		rec = USART_ReceiveData(USART1);
		USART_SendData(USART1, rec);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
	}
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
