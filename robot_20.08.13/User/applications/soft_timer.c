#include <rtthread.h>
#include <stm32f4xx.h>
#include "soft_timer.h"
#include "bsp_oled.h"
/* 定时器的控制块 */
static rt_timer_t timer_test;
static rt_tick_t timeout_value=100;

rt_err_t demo_start(void)
{
    /*创建动态定时器*/
    timer_test = rt_timer_create("timer1",          /* 定时器名字是 timer1 */
                                 timeout_callbak,   /* 超时时回调的处理函数 */
                                 RT_NULL,           /* 超时函数的入口参数 */
                                 timeout_value,     /* 定时长度，以OS Tick为单位，即timeout_value个OS Tick */
                                 RT_TIMER_FLAG_PERIODIC);   /* 周期性定时器 */

    /* 启动定时器 */
    if (timer_test != RT_NULL)
        rt_timer_start(timer_test);

    rt_kprintf("timer started !\n");
    return 0;
}

rt_uint32_t flag=0;

/* 超时时回调的处理函数 */
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
