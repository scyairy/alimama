
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
		mlx906_init();//�������
		LED_init();//LED��ʼ��
		oled_all();//oled��ʼ��
//		OLED_Clear();//oled����
//		motor_init();//���pwm
//		Adc_Init_cw();//��Ƭ����
//		ads1115_I2C_INIT();//��Ƭ����
//		Init_HX711pin();//ѹ��������
//		USART3_Init(115200);//JY61���ڳ�ʼ��
}

static void robot_sw_init(void)
{
		detect();
		rt_map_init();
}


