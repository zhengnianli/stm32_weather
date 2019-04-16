#include "esp8266_drive.h"


//串口接收缓存区 	
uint8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 			//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
uint8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节
uint16 USART2_RX_STA=0; 

//内部函数声明
static void USART2_Init(uint32 bound);
static uint8 *ESP8266_CheckAck(uint8 *ack);


/********************************************************************************************************
** 函数: ESP8266_Init,  ESP8266初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
void ESP8266_USART2_Init(uint32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(ESP8266_RST_Pin_Periph_Clock|ESP8266_CH_PD_Pin_Periph_Clock, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_Pin;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
	GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

   	GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;			   
	GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);
	
	USART2_Init(bound);			//串口2初始化
	
	ESP8266_RST_Pin_SetH;
	ESP8266_CH_PD_Pin_SetL;	    //使能引脚，初始化为低电平，配置AT时再置为高
	
	ESP8266_Config();			//AT指令配置
	PrintfToHMI("WiFi Init..." ,"page_Init", "InitWiFiTxt", "txt");
}

/********************************************************************************************************
** 函数: USART2_Init,  串口3初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void USART2_Init(uint32 bound)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2  TXD
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
   

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3  RXD
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA3  
  
	//Usart2 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

   	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  	USART_Init(USART2, &USART_InitStructure); //初始化串口2

  	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//开启串口接受和总线空闲中断
  	
	USART_Cmd(USART2, ENABLE);                    //使能串口2 	
	
	TIM2_Init(1000-1, 72-1);
	USART2_RX_STA=0;		//清零
}

/********************************************************************************************************
** 函数: ESP8266_CheckAck, ESP8266发送命令后,检测接收到的应答
**------------------------------------------------------------------------------------------------------
** 参数: ack：期待的应答结果
**		  
** 返回: 0：失败 其他：	期待应答结果的位置(ack的位置)										  
********************************************************************************************************/
static uint8 *ESP8266_CheckAck(uint8 *ack)
{
	char *res_ack = NULL;
	
	if(USART2_RX_STA&0x8000)  //接收到回车表明接收数据结束
	{
		USART2_RX_BUF[USART2_RX_STA&0x7fff] = 0;
		res_ack = strstr((const char*)USART2_RX_BUF, (const char*)ack);
	}
	
	return (uint8*)res_ack;   //成功
}

/********************************************************************************************************
** 函数: ESP8266_Printf, 采用打印方式发送数据
**------------------------------------------------------------------------------------------------------
** 参数: fmt, ...
**		  
** 返回: void 								  
********************************************************************************************************/
void ESP8266_Printf(char *fmt,...)
{
	uint16 i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF, fmt, ap);
	va_end(ap);
	len = strlen((const char*)USART2_TX_BUF);//此次发送数据的长度
	for(i=0;i<len;i++)						 //循环发送数据
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2, USART2_TX_BUF[i]);
	}
}

/********************************************************************************************************
** 函数: ESP8266_SendATCmd, 对ESP8266模块发送AT指令
**------------------------------------------------------------------------------------------------------
** 参数: cmd：待发送的指令 ack：期待的响应，为NULL表不需响应
**		  wait_time：等待响应的时间（单位10ms）
** 返回: 0：发送成功 1：失败											  
********************************************************************************************************/
uint8 ESP8266_SendATCmd(uint8 *cmd, uint8 *ack, uint16 wait_time)
{
	uint8 res = 0;
	USART2_RX_STA = 0;
	
	ESP8266_Printf("%s\r\n", cmd);
	if(ack&&wait_time)
	{
		while(--wait_time)
		{
			delay_ms(10);
			if(USART2_RX_STA&0x8000) //接收到回车表明接收数据结束
			{
				if(ESP8266_CheckAck(ack))
				{
					printf("%s ack=%s\n",cmd, ack);
					USART2_RX_STA = 0;
					break; //得到有效数据，退出while循环
				}
				USART2_RX_STA = 0;
			}
		}
		if(wait_time==0)  //超时
		{
			res = 1;	  //失败
		}
	}
	
	return res;
}

/********************************************************************************************************
** 函数: ESP8266_ExitTran,  退出透传模式
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 0:退出透传成功, 1:失败											  
********************************************************************************************************/
uint8 ESP8266_ExitTran(void)
{
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(500);					//等待500ms
	return ESP8266_SendATCmd("AT","OK",20);//退出透传判断.
}

/********************************************************************************************************
** 函数: ESP8266_Config,  ESP8266模块的常用配置
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无										  
********************************************************************************************************/
void ESP8266_Config(void)
{
	printf("ESP8266 WIFI Init...\n"); 
	ESP8266_CH_PD_Pin_SetH;   //使能引脚，高电平有效
	while(ESP8266_SendATCmd("AT","OK",20))	//检查WIFI模块是否在线
	{
		ESP8266_ExitTran();//退出透传
		ESP8266_SendATCmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		delay_ms(800);
	} 
	printf("ESP8266 WIFI Linking...\n"); 
	while(ESP8266_SendATCmd("ATE0","OK",20));	//关闭回显
	ESP8266_SendATCmd("AT+CWMODE=3","OK",50);	//设置WIFI 1为STA模式,2为AP模式,3为STA与AP模式共存
	ESP8266_SendATCmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	ESP8266_SendATCmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
}

/********************************************************************************************************
** 函数: ESP8266_ConnectAP,  ESP8266模块连接路由
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 0:连接, 1:未连接									  
********************************************************************************************************/
uint8 ESP8266_ConnectAP(char *ssid, char *password)
{
	char cmd[100];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
	return ESP8266_SendATCmd((char*)cmd, "WIFI GOT IP", 300);
}

/********************************************************************************************************
** 函数: ESP8266_LinkServer, 连接远程服务器
**------------------------------------------------------------------------------------------------------
** 参数: enumN:传输协议 ip:服务器的ip   port:端口
** 返回: 0：连接成功 1：连接失败						  
********************************************************************************************************/
uint8 ESP8266_LinkServer(ENUM_NetPro_TypeDef enumN, uint8 *ip, uint8 *port)
{
	uint8 ATcmd[120];
	switch( enumN )
	{
		case enumTCP:
			sprintf((char*)ATcmd, "AT+CIPSTART=\"%s\",\"%s\",%s", "TCP", ip, port);
			
			break;
		case enumUDP:
			sprintf((char*)ATcmd, "AT+CIPSTART=\"%s\",\"%s\",%s", "UDP", ip, port);
			break;
	}
	return ESP8266_SendATCmd(ATcmd, "OK", 200);
}

/********************************************************************************************************
** 函数: USART2_IRQHandler,  串口2中断服务函数
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
void USART2_IRQHandler( void )
{	
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	
		Uart2CharRecTimer = 10;
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
			{
				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值	 
			}
		}
	}  				 						
}

