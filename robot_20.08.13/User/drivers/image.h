#ifndef __image_H__
#define __image_H__

#include <rthw.h>
#include <rtthread.h>
extern rt_uint8_t image_refresh,image_result;

void image_thread_entry(void* parameter);
rt_uint8_t get_image(char platform);
void image_thread_creat(void);

#endif
