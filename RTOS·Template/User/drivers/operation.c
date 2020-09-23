#include "operation.h"
#include <rtthread.h>
#include "rt_key.h"
#include "rt_led.h"
#include "rt_oled.h"
#include <string.h>


/*  变量分配4字节对齐 */
ALIGN(RT_ALIGN_SIZE)
//静态线程的线程控制块
struct rt_thread key_thread;
struct rt_thread opreation_thread;
 rt_mailbox_t key_value_mb = RT_NULL;
//静态线程的线程堆栈
static rt_uint8_t key_stack[0x200];
static rt_uint8_t test_stack[0x200];
 rt_uint32_t key_value;
#define MB_LEN 1

#define THREAD_PRIORITY      16
#define THREAD_TIMESLICE     5

rt_err_t detect(void)
{
	
	 rt_err_t result;

    /* 初始化一个mailbox */
    key_value_mb = rt_mb_create("mba", MB_LEN, RT_IPC_FLAG_FIFO);          /* 采用FIFO方式进行线程等待 */
    if (key_value_mb == RT_NULL)
    {
        rt_kprintf("init mailbox failed.\n");
    }
		
		//创建按键发邮件线程
    result=rt_thread_init(&key_thread,
                   "key",
                   key_thread_entry,
                   RT_NULL,
                   (rt_uint8_t*)&key_stack[0],
                   sizeof(key_stack),
                   THREAD_PRIORITY-1, THREAD_TIMESLICE);//优先级15,先
									 
    if(result==RT_EOK)
		{
			rt_thread_startup(&key_thread);
		}
		
		//创建接收邮件进行检测线程							 
    result = rt_thread_init(&opreation_thread,
                   "opreation",
                   opreation_thread_entry,
                   RT_NULL,
                   (rt_uint8_t*)&test_stack[0],
                   sizeof(test_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);//优先级16

		if(result==RT_EOK)
		{
			rt_thread_startup(&opreation_thread);
		}							 
									 
    return 0;

}
rt_uint16_t key_flag=4;

void opreation_thread_entry(void* paramete)
{

	while(1)
	{

		
    
		if (rt_mb_recv(key_value_mb , &key_value , RT_WAITING_FOREVER) == RT_EOK)

//					mot(1,1);
//          TIM_SetCounter(TIM4,0);

//				}
//				else if(key_value==KEY1_PRES)
//				{
//					
//					huhu_pid_30(340,200);
////					mot(653,653);
//				}
//				else if(key_value==KEY2_PRES)
//				{
//					huhu_pid_60(688,715);
////					mot(955,955);
//				}
//				else if(key_value==KEY3_PRES)
//				{
//					huhu_pid_30_60(1,1);
////					huhu_mot_30_60(3);
//				}
					
				rt_thread_delay(10);
			
								}
							
	}
	
