#ifndef __COMPASS_H
#define __COMPASS_H

#include <rtthread.h>
#include "stm32f4xx.h"
#include "JY61.h"

void USART3_Init(void);
void show_angle(void);

#endif
