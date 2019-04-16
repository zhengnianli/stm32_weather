#include "serial_screen.h"

//串口接收缓存区 
u8 USART3_RX_BUF[USART3_REC_LEN];     //接收缓冲,最大USART3_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	

//定义一些全局变量，用于接收串口屏下发的数据
char g_place[32] = {0};
char g_place_utf8[32] = {0};
char g_city[32] = {0};
int utf8_len = 0;
char g_wifi_ssid[32] = "fjnu";
char g_wifi_password[32] = "happy110th";
char g_freq_int[32] = {0};
char g_freq_deci[32] = {0};
char g_serch_method[32] = "English";
char g_high_suggestion[64] = "天气较热，请大家注意多饮水，注意防暑。";
char g_low_suggestion[64] = "天气较凉，请大家注意增添衣物，防寒保暖。";

static void HMISendStart(void);

/********************************************************************************************************
** 函数: HMI_USART3_init,  串口屏初始化
**------------------------------------------------------------------------------------------------------
** 参数: bound:波特率 
** 说明:  串口屏默认波特率9600
** 返回: 无											  
********************************************************************************************************/
void HMI_USART3_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB 11  
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// 选择第3组的中断优先级
	//Usart3 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

   	//USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  	USART_Init(USART3, &USART_InitStructure); //初始化串口3

  	USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//开启串口接受和总线空闲中断
  	
	USART_Cmd(USART3, ENABLE);                    //使能串口3 	
	
	HMISendStart();		//确保串口HMI正常通信
	memset(Init_buf, 0, sizeof(Init_buf));
	HMISendStr("page page_Init");
	HMISendByte(0xff);
	PrintfToHMI("HMI Init..." ,"page_Init", "InitHMITxt", "txt");
}

/*******************************************************************************************************
** 函数: HMISendByte，往串口发送一个3个字节数据（供串口屏使用）
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void HMISendByte(u8 byte)
{
	u8 i;
	
	for(i=0;i<3;i++)
	{
		if(byte!=0)
		{
			USART_SendData(USART3, byte);                                 //发送数据
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){} //等待发送结束   
		}
		else
			return;
	}
}

/*******************************************************************************************************
** 函数: HMISendStart，为确保串口HMI正常通信
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void HMISendStart(void)
{
	delay_ms(200);
	HMISendByte(0xff);
	delay_ms(200);
}

/*******************************************************************************************************
** 函数: HMISendStr，向串口屏发送字符串 
**------------------------------------------------------------------------------------------------------
** 参数: buf
** 返回: 无
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void HMISendStr(char *buf)
{
	u8 i=0;
	
	while(1)
	{
		if(buf[i]!=0)
		{	
			USART_SendData(USART3, buf[i]);   
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){} 
			i++;
		}
		else
			return;
	}
}

/*******************************************************************************************************
** 函数: PrintfToHMI，打印初始化提示信息至串口屏
**------------------------------------------------------------------------------------------------------
** 参数: str:字符串  page：页面  objname：控件  property：属性
** 返回: 无
** 作者: 2018.1.11 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void PrintfToHMI(char *str, char *page, char *objname, char *property)
{
	memset(Init_buf, 0, sizeof(Init_buf));
	sprintf(Init_buf, "%s.%s.%s=\"%s\"", page, objname, property, str);	
	HMISendStr(Init_buf);
	HMISendByte(0xff);
}

/*******************************************************************************************************
** 函数: USART3_IRQHandler，USART3中断函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 作者: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void USART3_IRQHandler(void)                	
{
	u8 rec;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		rec = USART_ReceiveData(USART3);//(USART3->DR)，读取接收到的数据
		if((USART3_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
			{
				if(rec!=0x0a)USART3_RX_STA = 0;  //接收错误,重新开始
				else USART3_RX_STA |= 0x8000;	 //接收完成了 
			}
			else //还没收到0X0d
			{	
				if(rec==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF] = rec;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		
	} 
} 	 
