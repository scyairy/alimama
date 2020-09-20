#ifndef __soft_timer_H
#define __soft_timer_H	

#include <rtthread.h>
#include <stm32f4xx.h>
#include <finsh.h>

rt_err_t demo_start(void);
void timeout_callbak(void* parameter);
void timer_conrol(void);
void show_run_time(void);
void refresh_time(void);
#endif
