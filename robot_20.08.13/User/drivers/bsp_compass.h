#ifndef __COMPASS_H
#define __COMPASS_H

#include <rtthread.h>
#include "stm32f4xx.h"

void USART3_Init(void);
void show_angle(void);

#endif


// #ifndef __bsp_compass_H
// #define __bsp_compass_H

// #include "sys.h"
// /*
// 如果使用GY26则注释掉  #define RT_USE_GY61
// 如果使用JY61则注释掉  #define RT_USE_GY26
// */
// //#define RT_USE_GY26
// #define RT_USE_JY61

// extern int Angle_x,Angle_y,Angle_z;
// extern u8 angle_test;
// #ifdef RT_USE_GY26

// void compass_thread_creat(void);
// void compass_thread_entry(void* parameter);
// void refresh_angle(void);

// #endif

// #ifdef RT_USE_JY61

// void compass_thread_creat(void);
// void compass_thread_entry(void* parameter);

// //static void show_compass();

// #endif




// #endif /* __GY26_H */