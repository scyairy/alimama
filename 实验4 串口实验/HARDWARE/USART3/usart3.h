#ifndef __USART3_H
#define __USART3_H	 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define USART_REC_LEN  			2  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
extern  u8 Res;	  	
extern u8  USART_RX_BUF4[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA4; 
void usart4_init(u32 bound);




#endif














