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

#define DEBOUNCE_SHORT_TIME     2   		// 轻触按键消抖时间5（单位：50毫秒）
#define DEBOUNCE_LONG_TIME      3//10 // 长按键时间DEBOUNCE_COUT_FIRST+DEBOUNCE_COUT_INTERVAL*DEBOUNCE_LONG_TIME（单位：50毫秒）
#define DEBOUNCE_COUT_FIRST     20 	// 连按键间隔时间100（单位：50毫秒）
#define DEBOUNCE_COUT_INTERVAL  3   // 连按键间隔时间20（单位：50毫秒）

void KEY_Init(void);	//IO初始化
void key_thread_entry(void* parameter);
void rt_key_scan(void);

#endif
