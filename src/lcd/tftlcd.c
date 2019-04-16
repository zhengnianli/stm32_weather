#include "tftlcd.h"
#include "font.h"

/* LCD的画笔颜色和背景色 */	   
uint16 FRONT_COLOR = WHITE;	//画笔颜色
uint16 BACK_COLOR = BLACK;  //背景色 

TFTLCD_DATA_t tftlcd_data; 

/********************************************************************************************************
** 函数: TFTLCD_GPIO_Init,  TFTLCD_GPIO初始化,内部函数，供TFTLCD_Init函数调用
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void TFTLCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* 使能PORTD,E,G时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);
	
 	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);			  //PD0、1、4、5、8、9、10、14、15 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);			  //PE7、8、9、10、11、12、13、14、15    	    	 											 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);  		  //PG0、12
}

/********************************************************************************************************
** 函数: TFTLCD_FSMC_Init,  FSMC初始化,内部函数，供TFTLCD_Init函数调用
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void TFTLCD_FSMC_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadTimingInitStructure; 
	FSMC_NORSRAMTimingInitTypeDef  FSMC_WriteTimingInitStructure;
	/* 使能FSMC时钟 */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	
	
	/* 读时序配置 */ 
	FSMC_ReadTimingInitStructure.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  	FSMC_ReadTimingInitStructure.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  	FSMC_ReadTimingInitStructure.FSMC_DataSetupTime = 0x0f;		 //数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  	FSMC_ReadTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
	/* 写时序配置 */ 
	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x15;	 //地址建立时间（ADDSET）为16个HCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x15;	 //地址保持时间		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x05;	 //数据保存时间为6个HCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 	
 
	/* 配置访问外部存储器的时序参数 */
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;					 //这里我们使用NE4 ，也就对应BTCR[6],[7]。
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //不复用数据地址
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;			 //NOR/PSRAM，把TFTLCD当做SRAM来使用   
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;   //存储器数据宽度为16bit   
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;//FSMC_BurstAccessMode_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			  //存储器写使能
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 			  //读写使用不同的时序
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadTimingInitStructure; //读写时序
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_WriteTimingInitStructure;    //写时序
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  										  //初始化FSMC配置

	/* 使能存储块1第4区，起始地址为0X6C000000 */
 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

/********************************************************************************************************
** 函数: LCD_WriteCmd, 写寄存器函数
**------------------------------------------------------------------------------------------------------
** 参数: cmd
** 返回: 无											  
********************************************************************************************************/
void LCD_WriteCmd(uint16 cmd)
{
	TFTLCD->LCD_CMD=cmd;//写入要写的寄存器序号
}

/********************************************************************************************************
** 函数: LCD_WriteData, 写数据函数
**------------------------------------------------------------------------------------------------------
** 参数: data
** 返回: 无											  
********************************************************************************************************/
void LCD_WriteData(uint16 data)
{
	TFTLCD->LCD_DATA=data;//写入要写的寄存器的值
}

/********************************************************************************************************
** 函数: LCD_WriteCmdData, 写命令、数据函数
**------------------------------------------------------------------------------------------------------
** 参数: cmd, data 
** 返回: 无											  
********************************************************************************************************/
void LCD_WriteCmdData(uint16 cmd,uint16 data)
{
	LCD_WriteCmd(cmd);
	LCD_WriteData(data);
}

/********************************************************************************************************
** 函数: LCD_WriteData_Color, 写颜色函数
**------------------------------------------------------------------------------------------------------
** 参数: color
** 返回: 无											  
********************************************************************************************************/
void LCD_WriteData_Color(uint16 color)
{
	TFTLCD->LCD_DATA=color;
}

/********************************************************************************************************
** 函数: LCD_ReadData, 读数据函数
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 读到的数据											  
********************************************************************************************************/
uint16 LCD_ReadData(void)
{
	return TFTLCD->LCD_DATA;
}

/********************************************************************************************************
** 函数: LCD_Display_Dir, 设置LCD的显示方向
**------------------------------------------------------------------------------------------------------
** 参数: 0（竖屏），1（横屏）
** 返回: 无										  
********************************************************************************************************/
void LCD_Display_Dir(uint8 dir)
{
	if(dir==0)  //默认竖屏方向
	{				
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0100);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1030);
		tftlcd_data.height=400;
		tftlcd_data.width=240;
		tftlcd_data.dir=0;
		
	}
	else
	{		
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0000);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1038);
		tftlcd_data.height=240;
		tftlcd_data.width=400;
		tftlcd_data.dir=1;
	}	
}

/********************************************************************************************************
** 函数: LCD_Set_Window, 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy),窗体大小:width*height. 
**------------------------------------------------------------------------------------------------------
** 参数: sx,sy:窗口起始坐标(左上角) width,height:窗口宽度和高度,必须大于0!!
** 返回: 无										  
********************************************************************************************************/
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height)
{    
	if(tftlcd_data.dir==0)
	{
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0211);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0213);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sy);	
	}   	
	else
	{
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0213);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0211);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sy);	
	}
	LCD_WriteCmd(0x0202);
}

/********************************************************************************************************
** 函数: LCD_Clear, 清屏函数
**------------------------------------------------------------------------------------------------------
** 参数: color:要清屏的填充色
** 返回: 无										  
********************************************************************************************************/
void LCD_Clear(uint16 color)
{
	uint16_t i, j ;

	LCD_Set_Window(0, 0, tftlcd_data.width-1, tftlcd_data.height-1);	 //作用区域
  	for(i=0; i<tftlcd_data.width; i++)
	{
		for (j=0; j<tftlcd_data.height; j++)
		{
			LCD_WriteData_Color(color);
		}
	} 
}

/********************************************************************************************************
** 函数: TFTLCD_Init, TFTLCD初始化函数
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无										  
********************************************************************************************************/
void TFTLCD_Init(void)
{
	TFTLCD_GPIO_Init();
	TFTLCD_FSMC_Init();	
	delay_ms(50); 

	LCD_WriteCmd(0X0000);				   	
	tftlcd_data.id=LCD_ReadData();    
 	printf(" LCD ID:%x\r\n",tftlcd_data.id); //打印LCD ID
	/* --R61509V_CPT3.0 --- */

	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	
	LCD_WriteCmd(0x0400); LCD_WriteData(0x6200);
	LCD_WriteCmd(0x0008); LCD_WriteData(0x0808);
	
	LCD_WriteCmd(0x0300); LCD_WriteData(0x0005);//gamma
	LCD_WriteCmd(0x0301); LCD_WriteData(0x4C06);
	LCD_WriteCmd(0x0302); LCD_WriteData(0x0602);
	LCD_WriteCmd(0x0303); LCD_WriteData(0x050C);
	LCD_WriteCmd(0x0304); LCD_WriteData(0x3300);
	LCD_WriteCmd(0x0305); LCD_WriteData(0x0C05);
	LCD_WriteCmd(0x0306); LCD_WriteData(0x4206);
	LCD_WriteCmd(0x0307); LCD_WriteData(0x060C);
	LCD_WriteCmd(0x0308); LCD_WriteData(0x0500);
	LCD_WriteCmd(0x0309); LCD_WriteData(0x0033);
	
	LCD_WriteCmd(0x0010); LCD_WriteData(0x0014);//
	LCD_WriteCmd(0x0011); LCD_WriteData(0x0101);//
	LCD_WriteCmd(0x0012); LCD_WriteData(0x0000);//
	LCD_WriteCmd(0x0013); LCD_WriteData(0x0001);//
	
	LCD_WriteCmd(0x0100); LCD_WriteData(0x0330);//BT AP
	LCD_WriteCmd(0x0101); LCD_WriteData(0x0247);//DC0,DC1,VC
	LCD_WriteCmd(0x0103); LCD_WriteData(0x1000);//VDV
	LCD_WriteCmd(0x0280); LCD_WriteData(0xDE00);//VCM
	LCD_WriteCmd(0x0102); LCD_WriteData(0xD1B0);//VR,VCMR,PSON,PON
	delay_ms(10);
	
	LCD_WriteCmd(0x0001); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0002); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0003); LCD_WriteData(0x1030);
	LCD_WriteCmd(0x0009); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x000C); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0090); LCD_WriteData(0x8000);
	LCD_WriteCmd(0x000F); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0210); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0211); LCD_WriteData(0x00EF);
	LCD_WriteCmd(0x0212); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0213); LCD_WriteData(0x018F);//432=1AF, 400=18F
	LCD_WriteCmd(0x0500); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0501); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0502); LCD_WriteData(0x005F);
	LCD_WriteCmd(0x0401); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x0404); LCD_WriteData(0x0000);
	delay_ms(10);
							 
	LCD_WriteCmd(0x0007); LCD_WriteData(0x0100);//BASEE
	delay_ms(10);
	
	LCD_WriteCmd(0x0200); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0201); LCD_WriteData(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0202);

	LCD_Display_Dir(0);		//0：竖屏  1：横屏  默认竖屏
	LCD_Clear(WHITE);
}

//画点
//x,y:坐标
//FRONT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_Set_Window(x, y, x, y);  //设置点的位置
	LCD_WriteData_Color(FRONT_COLOR);	
}


/********************************************************************************************************
** 函数: LCD_DrawFRONT_COLOR, 快速画点
**------------------------------------------------------------------------------------------------------
** 参数: x,y:坐标 color:颜色
** 返回: 无										  
********************************************************************************************************/
void LCD_DrawFRONT_COLOR(uint16 x,uint16 y,uint16 color)
{	   
	LCD_Set_Window(x, y, x, y);
	LCD_WriteData_Color(color);	
} 

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/********************************************************************************************************
** 函数: LCD_ShowChar, 在指定位置显示一个字符
**------------------------------------------------------------------------------------------------------
** 参数: x,y:起始坐标 color:颜色 num:要显示的字符:" "--->"~" size:字体大小 12/16/24 
**        mode:叠加方式(1)还是非叠加方式(0)
** 返回: 无										  
********************************************************************************************************/
void LCD_ShowChar(uint16 x,uint16 y,uint8 num,uint8 size,uint8 mode)
{  							  
    uint8 temp,t1,t;
	uint16 y0=y;
	uint8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=ascii_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=ascii_1608[num][t];	//调用1608字体
		else if(size==24)temp=ascii_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
			else if(mode==0)LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=tftlcd_data.height)return;	//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=tftlcd_data.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   

/********************************************************************************************************
** 函数: LCD_Pow, m^n函数
**------------------------------------------------------------------------------------------------------
** 参数: m:底数 n:指数
** 返回: m^n次方.										  
********************************************************************************************************/
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

/********************************************************************************************************
** 函数: LCD_ShowNum, 显示数字
**------------------------------------------------------------------------------------------------------
** 参数: x,y:起点坐标 len :数字的位数 size:字体大小 num:数值(0~4294967295);
** 返回: 无										  
********************************************************************************************************/	
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 

/********************************************************************************************************
** 函数: LCD_ShowString, 显示字符串
**------------------------------------------------------------------------------------------------------
** 参数: x,y:起点坐标 color:颜色 width,height:区域大小 size:字体大小 *p:字符串起始地址
** 返回: 无										  
********************************************************************************************************/	
void LCD_ShowString(uint16 x,uint16 y,uint16 width,uint16 height,uint8 size,uint8 *p)
{
	uint8 x0 = x;
	width += x;
	height += y;
    while( (*p<='~')&&(*p>=' ') )//判断是不是非法字符!
    {       
        if( x>=width )
		{
			x = x0;
			y += size;
		}
        if( y>=height )break;//退出
        LCD_ShowChar(x, y, *p, size, 0);
        x += size/2;
        p++;
    }  
}

/********************************************************************************************************
** 函数: LCD_ShowFontHZ, 显示汉字
**------------------------------------------------------------------------------------------------------
** 参数: x,y:起点坐标 cn:汉字字符串
** 返回: 无										  
********************************************************************************************************/	
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNum扫描字库的字数
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK的位数
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
						} 						
						else
						{
							LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							if((y-y0)==29)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)结束
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)结束 	
		cn += 2;
		x += 32;
		x0=x;
	}
}	
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

