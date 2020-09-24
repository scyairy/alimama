
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
static void robot_hw_init(void);	//Ӳ�������ʼ��
static void robot_sw_init(void);	//��������ʼ��

void rt_init_thread_entry(void* parameter)
{
	#ifdef RT_USING_FINSH
		/* init finsh */
		finsh_system_init();
	#endif
	//��ʼ
	robot_hw_init();		//Ӳ����ʼ��
	robot_sw_init();	//�����ʼ��

}

int rt_application_init()//��ʼ�߳�
{
    rt_thread_t init_thread;	//�����߳̽ṹ
	
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 16, 20);

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}
static void robot_hw_init(void)		//Ӳ����ʼ��
{
		Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
		mlx906_init();//�������
		LED_init();//LED��ʼ��
		KEY_Init();//������ʼ��
		oled_all();//oled��ʼ��
		OLED_Clear();//oled����
		motor_init();//���pwm
		Adc_Init_cw();//��Ƭ����
		ads1115_I2C_INIT();//��Ƭ����
		Init_HX711pin();//ѹ��������
		delay_init(168);			//��ʱ��ʼ��
		uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200 
		usart2_init(42,115200);		//��ʼ������2������Ϊ115200
		TIM14_PWM_Init(3000-1,200-1);	//1Mhz�ļ���Ƶ��,2Khz��PWM.  
}

static void robot_sw_init(void)
{
		detect();
//		ov2640();
		rt_map_init();
}


