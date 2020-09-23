
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
#include "JY61.h"
#include "cw_adc.h"
#include "rt_oled.h"
#include "rt_led.h"
#include "operation.h"
#include "rt_pwm.h"



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
		mlx906_init();//红外测温
		LED_init();//LED初始化
		oled_all();//oled初始化
//		OLED_Clear();//oled清屏
//		motor_init();//舵机pwm
//		Adc_Init_cw();//贴片测温
//		ads1115_I2C_INIT();//贴片测温
//		Init_HX711pin();//压力传感器
//		USART3_Init(115200);//JY61串口初始化
}

static void robot_sw_init(void)
{
		detect();
		rt_map_init();
}


