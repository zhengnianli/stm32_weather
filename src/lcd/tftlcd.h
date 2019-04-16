#ifndef __TFTLCD_H
#define __TFTLCD_H

#include "config.h"

#define TFTLCD_R61509V3

//颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

typedef struct
{
	uint16 LCD_CMD;
	uint16 LCD_DATA;
}TFTLCD_TypeDef;

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A10作为数据命令区分线 
//注意设置16位总线时STM32内部会右移一位对齐!			    
#define TFTLCD_BASE        ((u32)(0x6C000000 | 0x000007FE))
#define TFTLCD             ((TFTLCD_TypeDef *) TFTLCD_BASE)

//TFTLCD重要参数集
typedef struct  
{										    
	uint16 width;			//LCD 宽度
	uint16 height;			//LCD 高度
	uint16 id;				//LCD ID
	uint8  dir;           	//LCD 方向
}TFTLCD_DATA_t;

/* 管理LCD重要参数 */
extern TFTLCD_DATA_t tftlcd_data;	
/* LCD的前端颜色和背景色	*/   
extern uint16  FRONT_COLOR;//前端颜色 默认红色    
extern uint16  BACK_COLOR; //背景颜色.默认为白色

void LCD_WriteCmd(uint16 cmd);  //写寄存器函数
void LCD_WriteData(uint16 data);//写数据函数
void LCD_WriteCmdData(uint16 cmd,uint16 data);//写寄存器、数据函数
void LCD_WriteData_Color(uint16 color);//往数据寄存器里写入颜色值
uint16 LCD_ReadData(void);	 //读数据
void LCD_Display_Dir(u8 dir);//设置LCD显示方向
void TFTLCD_Init(void);      //初始化
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height);//设置窗口
void LCD_Clear(uint16 Color);	 									  //清屏
void LCD_Fill(uint16 xState,uint16 yState,uint16 xEnd,uint16 yEnd,uint16 color); //填充矩形
void LCD_DrawPoint(uint16 x,uint16 y);							  //画点
void LCD_DrawFRONT_COLOR(uint16 x,uint16 y,uint16 color);		  //快速画点
uint16 LCD_ReadPoint(uint16 x,uint16 y);							  //读取屏幕上某个点的颜色值
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);    //画线
void LCD_DrawLine_Color(uint16 x1, uint16 y1, uint16 x2, uint16 y2,uint16 color);//画有颜色的线
void LCD_DrowSign(uint16 x, uint16 y, uint16 color);	//画十字标记
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2);		//画矩形
void LCD_Draw_Circle(uint16 x0,uint16 y0,uint8 r);					//画圆形
void LCD_ShowChar(uint16 x,uint16 y,uint8 num,uint8 size,uint8 mode);		//在指定位置显示一个字符
void LCD_ShowNum(uint16 x,uint16 y,u32 num,uint8 len,uint8 size);	 	//显示数字
void LCD_ShowxNum(uint16 x,uint16 y,u32 num,uint8 len,uint8 size,uint8 mode);//显示数字
void LCD_ShowString(uint16 x,uint16 y,uint16 width,uint16 height,uint8 size,uint8 *p);//显示字符串
void LCD_ShowFontHZ(uint16 x, uint16 y, uint8 *cn); 							//显示汉字			   						   																			
void LCD_ShowPicture(uint16 x, uint16 y, uint16 wide, uint16 high,uint8 *pic);		//显示图片
#endif
