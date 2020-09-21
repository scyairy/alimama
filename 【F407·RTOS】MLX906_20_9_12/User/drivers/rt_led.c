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
