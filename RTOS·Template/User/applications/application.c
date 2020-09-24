
#include <board.h>
#include <rtthread.h>
#include "delay.h"
#include "map.h"
#include "bsp_ads1115.h"
#ifdef RT_USING_FINSH	
#include <shell.h>
#include <finsh.h>
#endif

#include "mlx906.h"
#include "hx711.h"
#include "cw_adc.h"
#include "rt_oled.h"
#include "rt_led.h"
#include "operation.h"
#include "rt_pwm.h"
#include "usart.h"
#include "timer.h"
#include "rt_key.h"
#include "ov2640.h"



/*************************************/
static void robot_hw_init(void);	//硬件程序初始化
static void robot_sw_init(void);	//软件程序初始化

void rt_init_thread_entry(void* parameter)
{
	#ifdef RT_USING_FINSH
		/* init finsh */
		finsh_system_init();
	#endif
	//开始
	robot_hw_init();		//硬件初始化
	robot_sw_init();	//软件初始化

}

int rt_application_init()//起始线程
{
    rt_thread_t init_thread;	//建立线程结构
	
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 16, 20);

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}
static void robot_hw_init(void)		//硬件初始化
{
		Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
		mlx906_init();//红外测温
		LED_init();//LED初始化
		KEY_Init();//按键初始化
		oled_all();//oled初始化
		OLED_Clear();//oled清屏
		motor_init();//舵机pwm
		Adc_Init_cw();//贴片测温
		ads1115_I2C_INIT();//贴片测温
		Init_HX711pin();//压力传感器
		delay_init(168);			//延时初始化
		uart_init(84,115200);		//初始化串口波特率为115200 
		usart2_init(42,115200);		//初始化串口2波特率为115200
		TIM14_PWM_Init(3000-1,200-1);	//1Mhz的计数频率,2Khz的PWM.  
}

static void robot_sw_init(void)
{
		detect();
//		ov2640();
		rt_map_init();
}


