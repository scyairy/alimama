#ifndef __USART4_H
#define __USART4_H	 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART4_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART4_RX_STA;         		//����״̬���	
extern u8 t;
extern u8 len;
extern 	u8 z,s1,s2,s3,s4;

 

void URSART3_Recieve(void);
void usart3_init(u32 bound);

#endif









