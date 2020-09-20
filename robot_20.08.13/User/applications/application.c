/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_oled.h"
#include "delay.h"
#include "operation.h"
#include "robot_ui.h"
#include "bsp_grayscale.h"
#include "bsp_flash.h"
#include "track.h"
#include "robot_run.h"
#include "bsp_motor.h"
#include "JY61.h"
#include "bsp_compass.h"
#include "image.h"


#ifdef RT_USING_FINSH	//Finsh组件,用户命令行组件
#include <shell.h>
#include <finsh.h>
#endif


static void robot_hw_init(void);	//硬件程序初始化
static void robot_sw_init(void);	//软件程序初始化


void rt_init_thread_entry(void* parameter)
{
	#ifdef RT_USING_FINSH
		/* init finsh */
		finsh_system_init();
	#endif
		robot_hw_init();		//硬件初始化
	    robot_sw_init();	//软件初始化
}

int rt_application_init()
{
    rt_thread_t tid;	//建立线程结构
	
    tid = rt_thread_create("init",		//建立动态线程
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);				//开始调度线程,将线程调度到就绪状态

    return 0;
}

static void robot_hw_init(void)		//硬件初始化
{
	oled_init();
	key_init();
	ILL_Init();
	OLED_Clear();
	grayscale_init();
	robot_ui_init();
	motor_init();
	USART3_Init();
}

static void robot_sw_init(void)		//软件初始化
{
	operation();
    robot_run();
	image_thread_creat();
	
}


