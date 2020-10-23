#ifndef __USART4_H
#define __USART4_H	 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART4_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART4_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART4_RX_STA;         		//接收状态标记	
extern u8 t;
extern u8 len;
extern 	u8 z,s1,s2,s3,s4;

 

void URSART3_Recieve(void);
void usart3_init(u32 bound);

#endif









