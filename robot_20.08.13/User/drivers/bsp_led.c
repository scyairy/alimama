#include "bsp_led.h"
#include <rtthread.h>

#include "operation.h"
#include "bsp_key.h"
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;

void led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(led0_rcc, ENABLE);


  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = led0_pin ;
	GPIO_Init(led0_gpio, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = led1_pin ;
	GPIO_Init(led1_gpio, &GPIO_InitStructure);
	
	GPIO_SetBits(led0_gpio,led0_pin);
	GPIO_SetBits(led1_gpio,led1_pin);
}


//static void led_thread_entry(void* parameter)
//{


////		while (1)
////		{
////			LED1 = 0;
////		
////			rt_thread_delay( RT_TICK_PER_SECOND/2 );
////		
////			LED1 = 1;
////		
////			rt_thread_delay( RT_TICK_PER_SECOND/2 );

////	}
//	while(1)
//	{
//		LED1=!LED1;
//		rt_thread_delay(500);
//	}


//	
//}


//int rt_led_init(void)
//{
//    rt_thread_t init_thread;

//    rt_err_t result;

//    /* init led thread */
//    result = rt_thread_init(&led_thread,
//                            "led",
//                            led_thread_entry,
//                            RT_NULL,
//                            (rt_uint8_t*)&led_stack[0],
//                            sizeof(led_stack),
//                            20,
//                            5);
//    if (result == RT_EOK)
//    {
//        rt_thread_startup(&led_thread);
//    }
//	
//	return 0;
//}

void Infrared_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void Laser_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	


}

void ILL_Init(void)
{
	Infrared_Init();
	led_init();
	Laser_Init();
}

void twinkling(void)	//иак╦
{
	LED1=!LED1;
	rt_thread_delay(1000);
	LED1=!LED1;
	rt_thread_delay(800);
	LED1=!LED1;
	rt_thread_delay(1000);
	LED1=!LED1;
}
