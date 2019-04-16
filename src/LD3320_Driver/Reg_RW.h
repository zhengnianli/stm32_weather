// Reg_RW.h  读写LD3320芯片的寄存器，共有四种方法。
#ifndef REG_RW_H
#define REG_RW_H

#include "config.h"


//#define LD_RST_H() GPIO_SetBits(GPIOB, GPIO_Pin_6)
//#define LD_RST_L() GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define LD_RST_H() GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define LD_RST_L() GPIO_ResetBits(GPIOB, GPIO_Pin_1)

#define LD_CS_H()	GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define LD_CS_L()	GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#define LD_SPIS_H()  GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define LD_SPIS_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_9)



/****************************************************************
函数名： LD_WriteReg
功能：写LD3320芯片的寄存器
参数：  address, 8位无符号整数，地址
		dataout，8位无符号整数，要写入的数据
返回值：无
****************************************************************/ 
void LD_WriteReg( unsigned char address, unsigned char dataout );

/****************************************************************
函数名： LD_ReadReg
功能：读LD3320芯片的寄存器
参数：  address, 8位无符号整数，地址
返回值：8位无符号整数，读取的结果
****************************************************************/ 
unsigned char LD_ReadReg( unsigned char address );



#endif
