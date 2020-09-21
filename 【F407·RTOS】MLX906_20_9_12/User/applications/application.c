
#include <board.h>
#include <rtthread.h>

#include "delay.h"

#ifdef RT_USING_FINSH	
#include <shell.h>
#include <finsh.h>
#endif

#include "mlx906.h"
#include "rt_oled.h"
#include "rt_led.h"


/*************************************/

void rt_init()
{
	mlx906_init();
	LED_init();
	oled_all();
 	
}
void rt_main()
{
	mlx906();
}

void rt_init_thread_entry(void* parameter)
{
	#ifdef RT_USING_FINSH
		/* init finsh */
		finsh_system_init();
	#endif
	//开始

	rt_init();
	rt_main();

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


