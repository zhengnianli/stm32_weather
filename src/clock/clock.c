#include "clock.h"

/********************************************************************************************************
** 函数: 画圆
**------------------------------------------------------------------------------------------------------
** 参数: x,y:坐标  color:颜色
** 返回: 无											  
********************************************************************************************************/
static void GetCircle(int x,int y,int r,int color)
{
	int xc=0;
	int yc,p;
	yc=r;
	p=3-(r<<1);	
	while(xc <= yc)
	{
		LCD_DrawFRONT_COLOR(x+xc,y+yc,color);
		LCD_DrawFRONT_COLOR(x+xc,y-yc,color);	
		LCD_DrawFRONT_COLOR(x-xc,y+yc,color);
		LCD_DrawFRONT_COLOR(x-xc,y-yc,color);
		LCD_DrawFRONT_COLOR(x+yc,y+xc,color);	
		LCD_DrawFRONT_COLOR(x+yc,y-xc,color);
		LCD_DrawFRONT_COLOR(x-yc,y+xc,color);
		LCD_DrawFRONT_COLOR(x-yc,y-xc,color);
		if(p<0)
		{
			p += (xc++ << 2) + 6;	
		}
		else
			p += ((xc++ - yc--)<<2) + 10;
	}
}

/********************************************************************************************************
** 函数: DrawCircle, 画指针时钟的边界圆及中心
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void DrawCircle(void)	 
{
	GetCircle(120,250,100,YELLOW);
	GetCircle(120,250,99,YELLOW);
	GetCircle(120,250,98,YELLOW);
	GetCircle(120,250,97,YELLOW);
	GetCircle(120,250,5,YELLOW);			
}

/********************************************************************************************************
** 函数: DrawGrid, 画指针时钟的格点
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
static void DrawGrid(void) 
{
	u8 i;
	u8 rome[][3]={"12","1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" } ; //表盘数字 
	int x1,y1,x2,y2,x3,y3;
	for(i=0;i<60;i++)
	{
		x1 = (int)(120 + (sin(i * PI / 30) * 92)); 
		y1 = (int)(120 - (cos(i * PI / 30) * 92)); 
		x2 = (int)(120 + (sin(i * PI / 30) * 97)); 
		y2 = (int)(120 - (cos(i * PI / 30) * 97)); 
		FRONT_COLOR=RED;
		LCD_DrawLine(x1,y1+130,x2,y2+130);

		if(i%5==0)
		{
			x1 = (int)(120 + (sin(i * PI / 30) * 85)); 
			y1 = (int)(120 - (cos(i * PI / 30) * 85)); 
			x2 = (int)(120 + (sin(i * PI / 30) * 97)); 
			y2 = (int)(120 - (cos(i * PI / 30) * 97)); 
			LCD_DrawLine(x1,y1+130,x2,y2+130);	

			x3 = (int)(92 + (sin((i ) * PI / 30) * 80)); 
			y3 = (int)(92 - (cos((i ) * PI / 30) * 80));
			FRONT_COLOR=YELLOW;
			LCD_ShowString(x3+20,y3+150,tftlcd_data.width,tftlcd_data.height,16,rome[i/5]);
		}	
	}		
}

/********************************************************************************************************
** 函数: DrawGrid, 画指针时钟的时针、分针、秒针
**------------------------------------------------------------------------------------------------------
** 参数: hhour时，mmin分，秒ssec
** 返回: 无											  
********************************************************************************************************/
static void DrawPointer(int hhour,int mmin,int ssec)  
{
	int xhour, yhour, xminute, yminute, xsecond, ysecond; //表心坐标系指针坐标
	xhour = (int)(60 * sin( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	yhour = (int)(60 * cos( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	xminute = (int)(90 * sin( mmin * PI / 30 + ssec * PI / 1800)); 
	yminute = (int)(90 * cos( mmin * PI / 30 + ssec * PI / 1800)); 
	xsecond = (int)(100 * sin( ssec * PI / 30)); 
	ysecond = (int)(100 * cos( ssec * PI / 30)); 

	FRONT_COLOR=RED;
	LCD_DrawLine(120 + xhour, 250 - yhour, 120 -xhour / 6, 250 + yhour / 6);
	FRONT_COLOR=BLUE;
	LCD_DrawLine(120 + xminute, 250 - yminute, 120 -xminute / 4, 250 + yminute / 4);
	FRONT_COLOR=GREEN;
	LCD_DrawLine(120 + xsecond, 250 - ysecond, 120 -xsecond / 3, 250 + ysecond / 3);
	
}

/********************************************************************************************************
** 函数: PointerWipe, 擦除指针（令指针的颜色与底色相同即达到擦除指针的效果）
**------------------------------------------------------------------------------------------------------
** 参数: hhour时，mmin分，秒ssec
** 返回: 无											  
********************************************************************************************************/
static void PointerWipe(int hhour,int mmin,int ssec)  
{
	int xhour, yhour, xminute, yminute, xsecond, ysecond; //表心坐标系指针坐标
	xhour = (int)(60 * sin( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	yhour = (int)(60 * cos( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	xminute = (int)(90 * sin( mmin * PI / 30 + ssec * PI / 1800)); 
	yminute = (int)(90 * cos( mmin * PI / 30 + ssec * PI / 1800)); 
	xsecond = (int)(100 * sin( ssec * PI / 30)); 
	ysecond = (int)(100 * cos( ssec * PI / 30)); 

	FRONT_COLOR=BLACK;
	LCD_DrawLine(120 + xhour, 250 - yhour, 120 -xhour / 6, 250 + yhour / 6);
	LCD_DrawLine(120 + xminute, 250 - yminute, 120 -xminute / 4, 250 + yminute / 4);
	LCD_DrawLine(120 + xsecond, 250 - ysecond, 120 -xsecond / 3, 250 + ysecond / 3);
}

/********************************************************************************************************
** 函数: ShowClock, 显示时间
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无											  
********************************************************************************************************/
void ShowClock(void)
{
	char date_buf[20],time_buf[10];
	
	DrawCircle(); 	//表盘
	DrawGrid();		//画指针
	if( g_time_flag )
	{
		PointerWipe(calendar_t.hour,calendar_t.min,calendar_t.sec);
		g_time_flag = 0;
		RTC_Get();
		//------------------------------------------------------------------------------------------
		//串口显示
		printf("RTC Time:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%s\n",
		calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,
		calendar_t.hour,calendar_t.min,calendar_t.sec,wday[calendar_t.week]);
		//------------------------------------------------------------------------------------------
		//液晶显示	
		sprintf(date_buf,"%.4d/%.2d/%.2d   %s",calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,wday[calendar_t.week]);
		sprintf(time_buf,"%.2d:%.2d:%.2d",calendar_t.hour,calendar_t.min,calendar_t.sec);
		DrawPointer(calendar_t.hour, calendar_t.min, calendar_t.sec);	//画指针
	}
	DrawCircle();
	DrawGrid();
	LCD_ShowString(24,80,tftlcd_data.width,tftlcd_data.height,24,(uint8 *)date_buf);
	LCD_ShowString(77,110,tftlcd_data.width,tftlcd_data.height,24,(uint8 *)time_buf);
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/