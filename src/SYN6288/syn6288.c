/********************************************************************************************************
**
** SYN6288 中文语音合成模块
**
**
**
********************************************************************************************************/

#include "syn6288.h"


u8 UART4_RX_BUF[UART4_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 UART4_RX_STA=0;       //接收状态标记	


static void UART4_SendStr(uint8_t *data, uint8_t len);


/********************************************************************************************************
** 函数: SYN6288_UART4_Init,  串口屏初始化
**------------------------------------------------------------------------------------------------------
** 参数: bound:波特率 
** 说明: SYN6288模块默认波特率9600bit/s
** 返回: 无		
** 作者: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void SYN6288_UART4_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			   //串口输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOC,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//RX			 //串口输入
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOC,&GPIO_InitStructure); /* 初始化GPIO */
	

   //UART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口4
	
	USART_Cmd(UART4, ENABLE);  //使能串口4 
	
	USART_ClearFlag(UART4, USART_FLAG_TC);
		
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启相关中断

	//UART4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
}

/*******************************************************************************************************
** 函数: TTSPlay，文本合成语音函数
**------------------------------------------------------------------------------------------------------
** 参数: Music(背景音乐选择):0无背景音乐。1-15：相关背景音乐   *Text:文本指针变量 
** 返回: 无
** 说明：（1）SYN6288默认波特率为9600bit/s
		（2）数据包格式：5字节帧头+文本+1字节校验，文本字节数小于等于200字节
** 作者: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TTSPlay(uint8_t Music,uint8_t *Text)
{
	/****************需要发送的文本**********************************/ 
	uint8_t DataPacket[50];		//
	uint8_t Text_Len;  
	uint8_t ecc  = 0;  			//定义校验字节
	uint8_t i=0;  
	Text_Len =strlen((const char*)Text); 			//需要发送文本的长度

	/*****************帧固定配置信息**************************************/           
	DataPacket[0] = 0xFD ; 				//构造帧头FD
	DataPacket[1] = 0x00 ; 				//构造数据区长度的高字节
	DataPacket[2] = Text_Len + 3; 		//构造数据区长度的低字节
	DataPacket[3] = 0x01 ; 				//构造命令字：合成播放命令		 		 
	DataPacket[4] = 0x01 | Music<<4 ;   //构造命令参数：背景音乐设定

	/*******************校验码计算***************************************/		 
	for(i = 0; i<5; i++)   				//依次发送构造好的5个帧头字节
	{  
		ecc=ecc^(DataPacket[i]);		//对发送的字节进行异或校验	
	}

	for(i= 0; i<Text_Len; i++)   		//依次发送待合成的文本数据
	{  
		ecc=ecc^(Text[i]); 				//对发送的字节进行异或校验		
	}		 
	/*******************发送帧信息***************************************/		  
	memcpy(&DataPacket[5], Text, Text_Len);
	DataPacket[5+Text_Len]=ecc;
	UART4_SendStr((char*)DataPacket,5+Text_Len+1);
} 	 

/*******************************************************************************************************
** 函数: UART4_SendStr，UART4发送字符串函数
**------------------------------------------------------------------------------------------------------
** 参数: data：要发送的字符串   len：字符串长度
** 返回: 无
** 作者: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void UART4_SendStr(uint8_t *data, uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART_SendData(UART4, data[i]);         //向串口1发送数据
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//等待发送结束
	}
}

/*******************************************************************************************************
** 函数: USART3_IRQHandler，USART3中断函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void UART4_IRQHandler(void)                	//串口3中断服务程序
{
	u8 r;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		r =USART_ReceiveData(UART4);//(USART3->DR);	//读取接收到的数据
		if((UART4_RX_STA&0x8000)==0)//接收未完成
		{
			if(UART4_RX_STA&0x4000)//接收到了0x0d
			{
				if(r!=0x0a)UART4_RX_STA=0;//接收错误,重新开始
				else UART4_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(r==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=r ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_REC_LEN-1))UART4_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		
	} 
	
}
