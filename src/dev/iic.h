#ifndef _iic_H
#define _iic_H

#include "config.h"

/* 定义I2C总线连接的GPIO端口, 只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GPIO_PORT_I2C	GPIOB					// GPIO端口 
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	// GPIO端口时钟 
#define I2C_SCL_PIN		GPIO_Pin_6				// 连接到SCL时钟线的GPIO 
#define I2C_SDA_PIN		GPIO_Pin_7				// 连接到SDA数据线的GPIO 
#define I2C_SCL_1()     GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		// SCL = 1 
#define I2C_SCL_0()     GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		// SCL = 0 

#define I2C_SDA_1()     GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		// SDA = 1 
#define I2C_SDA_0()     GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		// SDA = 0 

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	// 读SDA口线状态 

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

//IIC所有操作函数			
void i2c_Start(void);				//发送IIC开始信号
void i2c_Stop(void);				//发送IIC停止信号
void i2c_SendByte(uint8_t _ucByte); //IIC发送一个字节
uint8_t i2c_ReadByte(void);			//IIC读取一个字节
uint8_t i2c_WaitAck(void);			//IIC等待ACK信号
void i2c_Ack(void);					//IIC发送ACK信号
void i2c_NAck(void);				//IIC不发送ACK信号
uint8_t i2c_CheckDevice(uint8_t _Address);
#endif
