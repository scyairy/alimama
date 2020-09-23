/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-31     Bernard      first implementation
 * 2011-06-05     Bernard      modify for STM32F107 version
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f4xx.h"
#include "board.h"

/**
 * @addtogroup STM32
 */

/*@{*/

extern int  rt_application_init(void);

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define STM32_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define STM32_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define STM32_SRAM_BEGIN    (&__bss_end)
#endif

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1) ;
}

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */					//初始化平台：完成中断向量表设置,系统滴答时钟设置,
    rt_hw_board_init();

    /* show version */				//串口打印版本号
    rt_show_version();

    /* init tick */						//初始化系统滴答定时器并设置为0
    rt_system_tick_init();

    /* init kernel object */	//初始化系统管理对象
    rt_system_object_init();

    /* init timer system */		//初始化系统定时器
    rt_system_timer_init();

		/*初始化系统堆栈区*/
    rt_system_heap_init((void*)STM32_SRAM_BEGIN, (void*)STM32_SRAM_END);

    /* init scheduler system */	//初始化系统调度器
    rt_system_scheduler_init();

    /* init application */			//用户应用初始化
    rt_application_init();

    /* init timer thread */			//系统定时器功能
    rt_system_timer_thread_init();

    /* init idle thread */			//初始化空闲线程
    rt_thread_idle_init();

    /* start scheduler */				//启动调度器
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}

int main(void)
{
		/*
			; * 表示本程序里面用到的变量提供给其他模块调用
			; * 将状态寄存器的内容传送至通用寄存器
			; * 关闭中断
			; * 等同于mov pc,Lr 即跳转到Lr中存放的地址处
		*/
    /* disable interrupt first */	//禁用一切中断,初始化在context_rvds.S
    rt_hw_interrupt_disable();

    /* startup RT-Thread RTOS */	//启动RT-Thread程序
    rtthread_startup();

    return 0;
}

/*@}*/
