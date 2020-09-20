#include <rtthread.h>
#include "robot_run.h"
#include "operation.h"
#include "bsp_led.h"
#include "bsp_grayscale.h"
#include "track.h"
#include "map.h"


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t run_stack[512];
static struct rt_thread run_thread;


static void run_thread_entry(void* parameter)
{


		while (1)
		{
			if(run_flag == 1)
	       {
			  
			    rt_thread_detach(&operation_thread);	//挂起操作功能
					rt_thread_detach(&key_thread);				//挂起按键功能			   
			    GetGrayscale();							//获取当前灰度值
					 rt_map_init();							//跑地图
//			    run_flag = 0; 			
					break;	
			}
		   rt_thread_delay(10);
		}
		 rt_thread_suspend(&run_thread);


	
}

int robot_run(void)
{
    rt_err_t result;

    /* init run_thread thread */
    result = rt_thread_init(&run_thread,
                            "run",
                            run_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&run_stack[0],
                            sizeof(run_stack),
                            16,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&run_thread);
    }
	
	return 0;

}





