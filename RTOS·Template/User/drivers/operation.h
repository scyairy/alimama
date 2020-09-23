#ifndef __DETECT_H
#define __DETECT_H 

#include "sys.h"   
#include <rtthread.h>

extern rt_uint16_t key_flag;

extern rt_mailbox_t key_value_mb ;
extern rt_uint32_t key_value;
//extern rt_uint16_t encoder;



extern struct rt_thread key_thread;
extern struct rt_thread opreation_thread;
rt_err_t detect(void);
void opreation_thread_entry(void* paramete);


#endif

