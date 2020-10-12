#ifndef __USART3_H
#define __USART3_H	 
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 



extern unsigned char ucStctime1[8],ucStcAcc1[8],stcGyro1[8],stcAngle1[8];
extern unsigned char stcMag1[8],stcDStatus1[8],stcPress1[8],stcLonLat1[8],stcGPSV1[8];

extern u8 ucRxBuffer[10];

void CopeSerial2Data1(void);
void usart4_init(u32 bound);
#endif









