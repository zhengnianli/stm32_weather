#ifndef __SYN6288_H
#define __SYN6288_H

#include "config.h"

#define UART4_REC_LEN  		200  	  //定义最大接收字节数 200
	  	
extern u8  UART4_RX_BUF[UART4_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART4_RX_STA;         	  //接收状态标记	

void SYN6288_UART4_Init(u32 bound);
void TTSPlay(uint8_t Music,uint8_t *Text);

#endif

