#include "rt_pwm.h"

//电机初始化
void motor_init(void)
{
	  PWM_Init(1054-1,7-1);    //使用1054是为了当pwm为1000时，pwm占空比大于95%
	  motor_stop();
}


//电机PWM初始化 
//PB4 PB5 TIM3 ch1,ch2
void PWM_Init(uint16_t arr,uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); //GPIOB?′
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //GPIOB?

    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 | GPIO_Pin_1;  //GPIOB
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOB,&GPIO_InitStructure);               //
     
    TIM_TimeBaseStructure.TIM_Prescaler=psc;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseStructure.TIM_Period=arr;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                  

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC3Init(TIM3, &TIM_OCInitStructure); 
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_OC4Init(TIM3, &TIM_OCInitStructure); 
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); 
    
    TIM_ARRPreloadConfig(TIM3,ENABLE);

    TIM_Cmd(TIM3, ENABLE);
}

//dir:0向前  1向后
//RL_Tire：0右轮 1左轮
void mot(int left_speed,int right_speed)
{
    if (left_speed > 0&&right_speed > 0)
    {
			 rt_thread_delay(1);
        pwm_l =1053-left_speed ;
        pwm_r =1053-right_speed;
//        left_wheel = 1 ;
//        right_wheel = 1 ;
    }
//    else if (left_speed <= 0&&right_speed <= 0)
//    {
//        pwm_l =-left_speed;
//        pwm_r =-right_speed;
//        left_wheel = 0 ;
//        right_wheel = 0 ;
//    }
//    else if (left_speed > 0&&right_speed<= 0)
//    {
//        pwm_l =left_speed;
//        pwm_r =1053+right_speed;
//        left_wheel = 1 ;
//        right_wheel = 0 ;
//    }
//    else if (left_speed <= 0&&right_speed > 0)
//    {
//        pwm_l =1053+left_speed;
//        pwm_r =right_speed;
//        left_wheel = 0 ;
//        right_wheel = 1 ;
//    }
}

void motor_stop(void)
{
    mot(1000,0);
}



