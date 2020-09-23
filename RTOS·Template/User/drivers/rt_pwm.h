#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "sys.h" 
#include <rtthread.h>

#define left_wheel PDout(4)
#define right_wheel PDout(5)

#define pwm_r TIM3->CCR3 
#define pwm_l TIM3->CCR4

void motor_stop(void);
void motor_init(void);

void PWM_Init(uint16_t arr,uint16_t psc);

void mot(int lift_speed,int right_speed);




#endif
