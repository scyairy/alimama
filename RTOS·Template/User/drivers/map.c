#include <rtthread.h>
#include "operation.h"
#include "rt_led.h"
#include "map.h"
#include "rt_oled.h"
#include "JY61.h"
#include "soft_timer.h"
#include "mlx906.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t map_stack[10240];
static struct rt_thread map_thread;


static void map_thread_entry(void* parameter)
{
	while(1)
	{
        mlx906();
	}

}

int rt_map_init(void)
{
//    rt_thread_t init_thread;

    rt_err_t result;

	
    /* init led thread */
    result = rt_thread_init(&map_thread,
                            "map",
                            map_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&map_stack[0],
                            sizeof(map_stack),
                            16,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&map_thread);
    }
	
	return 0;

}


