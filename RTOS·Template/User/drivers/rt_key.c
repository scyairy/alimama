#include "sys.h"   
#include "stdlib.h"
#include "rt_key.h"
#include <rtthread.h>
#include "usart.h"
#include "operation.h"
#include "rt_oled.h"
#include "rt_led.h"
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13; //KEY0 KEY1 KEY2对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);//

	
 
} 
rt_uint8_t sta;
 void key_thread_entry(void *paramete)
{	 
	while(1)
	{
		  
		static rt_uint8_t key_up=1;//按键按松开标志	  
		if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
		{
			rt_thread_delay(5);//去抖动	
			key_up=0;
			if(KEY0==0)
			{
				sta=KEY0_PRES;
			}
			else if(KEY1==0)
			{
				sta=KEY1_PRES;
			}
			else if(KEY2==0)
			{
				sta=KEY2_PRES;
			}
			else if(KEY3==0)
			{
				sta=KEY3_PRES;
			}
		
		
		}
		else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)
		{
			key_up=1; 
			sta=0;
		}   
		rt_mb_send(key_value_mb , sta);
		rt_thread_delay(10);
	}
	
}
