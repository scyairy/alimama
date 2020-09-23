#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12) //PE2
#define KEY3 	  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_13)	//PA0


#define KEY0_PRES 	1
#define KEY1_PRES	  2
#define KEY2_PRES	  3
#define KEY3_PRES   4

#define DEBOUNCE_SHORT_TIME     2   		// �ᴥ��������ʱ��5����λ��50���룩
#define DEBOUNCE_LONG_TIME      3//10 // ������ʱ��DEBOUNCE_COUT_FIRST+DEBOUNCE_COUT_INTERVAL*DEBOUNCE_LONG_TIME����λ��50���룩
#define DEBOUNCE_COUT_FIRST     20 	// ���������ʱ��100����λ��50���룩
#define DEBOUNCE_COUT_INTERVAL  3   // ���������ʱ��20����λ��50���룩

void KEY_Init(void);	//IO��ʼ��
void key_thread_entry(void* parameter);
void rt_key_scan(void);

#endif
