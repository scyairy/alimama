#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12) //PE2
#define KEY3 	  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_13)	//PA0


#define KEY0_PRES 	1
#define KEY1_PRES	  2
#define KEY2_PRES	  3
#define KEY3_PRES   4

void KEY_Init(void);	//IO初始化
void key_thread_entry(void* parameter);

#endif
