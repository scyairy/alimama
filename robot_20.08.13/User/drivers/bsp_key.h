#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include <rtthread.h>
#include "stm32f4xx.h"

#define DEBOUNCE_SHORT_TIME     2   		// 轻触按键消抖时间5（单位：50毫秒）
#define DEBOUNCE_LONG_TIME      3//10 // 长按键时间DEBOUNCE_COUT_FIRST+DEBOUNCE_COUT_INTERVAL*DEBOUNCE_LONG_TIME（单位：50毫秒）
#define DEBOUNCE_COUT_FIRST     20 	// 连按键间隔时间100（单位：50毫秒）
#define DEBOUNCE_COUT_INTERVAL  3   // 连按键间隔时间20（单位：50毫秒）

#define KEY_OK PGin(13)

enum KEY_VALUE
{
	KEY_VALUE_ERR = 0,
	KEY_VALUE_UP,
	KEY_VALUE_DOWN,
	KEY_VALUE_ENSURE,
	KEY_VALUE_CANCEL
};


void key_thread_entry(void* parameter);
void key_init(void);

#endif /* __KEY_H */
