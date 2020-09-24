#include "rt_led.h"
#include "sys.h"

void LED_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
   
    
	GPIO_SetBits(GPIOG,GPIO_Pin_14);
}

void LED_Init1(void)
{    	 
	RCC->AHB1ENR|=1<<5;//使能PORTF时钟 
	GPIO_Set(GPIOF,PIN9|PIN10,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10设置
	LED0=1;//LED0关闭
//	LED1=1;//LED1关闭
}

