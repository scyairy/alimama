#include <rtthread.h>
#include "robot_run.h"
#include "operation.h"
#include "bsp_led.h"
#include "bsp_grayscale.h"
#include "map.h"
#include "track.h"
#include "bsp_motor.h"
#include "bsp_oled.h"
#include "JY61.h"
#include "route_selection.h"
#include "image.h"
#include "soft_timer.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t map_stack[10240];
static struct rt_thread map_thread;


static void map_thread_entry(void* parameter)
{
	int mms=4000;
while(1)
	{
		demo_start();
	  OLED_Clear();
	  OLED_ShowChar(75,5,'1',12);
//	rt_kprintf("start.....");
//  while(!Infrared1);
//	rt_thread_delay(1500);
//	Road_To_platform(500);
//	route_1_To_2();
//	all_route();
//	route_6_To_8();
		
		
//		PID_to_Angle(350);
// 	 	Tiyi_Laser_R_1point(385);
// 		motor_stop();
// 		rt_thread_delay(200);
// 		motor_time(1000,200,200);//µ¹ÍËÒ»¶Î
// 		rt_thread_delay(100);
// 		turn_angle(-28,350);//30		
// 		while(1)
// 		{		
// //			mot(-350,-350);
// 			motor_stop();
// 		}
		
		
//	Road_to_Angle(350);
//	motor_time(110,380,300);
//	Xiepo_Laser8_time(550,310);
//	rt_thread_delay(100);
//	turn_angle_qqb(90,-300,200);
//	turn_right_angle(300,0);
//	motor_time(300,250,250);//ÏÂÐ±ÆÂ	
//liuleliule_8();
		
//liuleliule_7();		
//PID_to_time(400,3000);
//Road_to_time(350,9000);		


route_1_To_2();
route_2_To_4();
route_4_To_5();		
route_5_To_8();
liuleliule_8();	
//PID_to_Angle(350);
//PID_Laser_R_1point(350);
//route_5_To_7();
//	Road_to_Angle(400);	
//Tiyi_5_To_8();
//turn_angle(-180,300);		
//route_8_To_7();	
//turn_angle(-180,300);
//liuleliule_8();
//turn_angle(180,300);
//route_7_To_8();	
//Road_to_Angle(600);//¼ì²âÉÏ°å	while(1)

		rt_thread_delay(10);
	  show_run_time();
		rt_thread_delay(2000);
//		refresh_time();
		while(1);

	}

}

int rt_map_init(void)
{
    rt_thread_t init_thread;

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


