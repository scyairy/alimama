#include <rtthread.h>
#include <stm32f4xx.h>
#include "soft_timer.h"
#include "bsp_oled.h"
/* ��ʱ���Ŀ��ƿ� */
static rt_timer_t timer_test;
static rt_tick_t timeout_value=100;

rt_err_t demo_start(void)
{
    /*������̬��ʱ��*/
    timer_test = rt_timer_create("timer1",          /* ��ʱ�������� timer1 */
                                 timeout_callbak,   /* ��ʱʱ�ص��Ĵ����� */
                                 RT_NULL,           /* ��ʱ��������ڲ��� */
                                 timeout_value,     /* ��ʱ���ȣ���OS TickΪ��λ����timeout_value��OS Tick */
                                 RT_TIMER_FLAG_PERIODIC);   /* �����Զ�ʱ�� */

    /* ������ʱ�� */
    if (timer_test != RT_NULL)
        rt_timer_start(timer_test);

    rt_kprintf("timer started !\n");
    return 0;
}

rt_uint32_t flag=0;

/* ��ʱʱ�ص��Ĵ����� */
void timeout_callbak(void* parameter)
{
    flag++;
//        if((flag%10)==0)
//        {
//            OLED_ShowString(60,7,"           ",14);
//            OLED_ShowNum(60,7,(int)(flag/10),3,14);
//        }
}

void show_run_time(void)
{
    rt_kprintf("\nRobot run tims:%d.%d s\n\r",flag/10,flag%10);
    OLED_ShowNum(40,4,(int)(flag/10),3,16);
    OLED_ShowNum(80,5,(int)(flag%10),1,14);
}

void refresh_time(void)
{
    flag=0;
}
