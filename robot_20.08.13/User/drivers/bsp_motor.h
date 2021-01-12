#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__
#include "sys.h" 
#include <rtthread.h>

#define Motor_Right_DIR PDout(5)
#define Motor_Left_DIR PDout(4)
#define Motor_Right_PWM TIM3->CCR4
#define Motor_Left_PWM TIM3->CCR3 

void motor_stop(void);
void motor_init(void);

//void mot(int16_t speed_left,int16_t speed_right);
//static void motor(uint8_t dir,uint8_t RL_Tire,int16_t speed);
void mot(int16_t Speed_Left,int16_t Speed_Right);
static void motor(uint8_t dir1,uint16_t L_Speed,uint8_t dir2,uint16_t R_Speed);
void motorr(int left_speed,int right_speed);
void motor_R(uint16_t dir,uint16_t speed);
void motor_L(uint16_t dir,uint16_t speed);
void turn_angle(int ang,int speed);
#endif
