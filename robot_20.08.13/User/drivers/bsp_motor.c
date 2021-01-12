#include "bsp_motor.h"
#include "JY61.h"
#include "bsp_compass.h"

//电机方向初始化 
//PF1 PF3
static void Dir_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         //使能GPIOF时钟

   //初始化设置
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //普通输出模式
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
   GPIO_Init(GPIOD, &GPIO_InitStructure);                         //初始化
 	
}

//电机PWM初始化 
//PF6 PF7
static void PWM_Init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	      //TIM10时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 	      //使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);         //GPIOF6复用为定时器10
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);       //GPIOF7复用为定时器11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;           //GPIOF6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                   //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	           //速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);                          //初始化PF6
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                       //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;      //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;                          //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                //初始化定时器3
	
	//初始化TIM3 Channel3\4 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;       //输出极性:TIM输出比较极性
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM1 4OC1
	
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM1 4OC1	
	
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);              //使能TIM10在CCR1上的预装载寄存器
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);              //使能TIM10在CCR1上的预装载寄存器
    TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPE使能 
	TIM_Cmd(TIM3,ENABLE);   
}
	

/*
	右轮函数
	dir=1:右轮向后
	dir=0:右轮向前
*/
void motor_L(uint16_t dir,uint16_t speed)
{
	Motor_Left_DIR=!dir;
	if(dir==1)Motor_Left_PWM=1054-speed;
	if(dir==0)Motor_Left_PWM=speed;
}
/*
	左轮函数
	dir=1:左轮向后
	dir=0:左轮向前
*/
void motor_R(uint16_t dir,uint16_t speed)
{

		Motor_Right_DIR=dir;
	if(dir==0)Motor_Right_PWM=1054-speed;
	if(dir==1)Motor_Right_PWM=speed;
}
static void motor(uint8_t dir1,uint16_t L_Speed,uint8_t dir2,uint16_t R_Speed)
{
//	float times;//300=0.985//350=0.989//400=0.992//450=0.996//500=1
//	if(L_Speed<=300){times=0.996;}
//	else if(L_Speed<=350){times=0.985;}
//	else if(L_Speed<=400){times=0.992;}
//	else if(L_Speed<=450){times=0.996;}
//	else {times=1;}
//	
//	if(L_Speed==400||R_Speed==400) times=1;
//	if(L_Speed==500||R_Speed==500) times=1.02;
//	if(L_Speed==550||R_Speed==550) times=0.983;
//	if(L_Speed==600||R_Speed==600) times=0.984;*times
	motor_L(dir1,L_Speed*0.98);
	motor_R(dir2,R_Speed);
}

/*
//dir:0向前  1向后
//RL_Tire：0右轮 1左轮
static void motor(uint8_t dir,uint8_t RL_Tire,int16_t speed)
{   
	if(dir==0)
	{
		if(RL_Tire==0)
		{
			Motor_Right_DIR=1;
			Motor_Right_PWM=speed;

		}
		else if(RL_Tire==1)
		{
			
			Motor_Left_DIR=0;
			Motor_Left_PWM=750-speed;
		
		}
	}
	else if(dir==1)
	{
		if(RL_Tire==0)
		{
		    Motor_Right_DIR=0;
			Motor_Right_PWM=750-speed;
		}
		else if(RL_Tire==1)
		{
			Motor_Left_DIR=1;
			Motor_Left_PWM=speed;
		}
	}
}
*/

void motor_stop(void)
{
	Motor_Right_DIR=1;
	Motor_Right_PWM=1;
	Motor_Left_DIR=1;
	Motor_Left_PWM=1;
}

/*
	运动函数
	Speed_Left:左轮速度
	Speed_Right:右轮速度
*/
void mot(int16_t Speed_Left,int16_t Speed_Right)
{
	if(Speed_Left>=0&&Speed_Right>=0)motor(1,Speed_Left,1,Speed_Right);
	if(Speed_Left<0&&Speed_Right>0)motor(0,-Speed_Left,1,Speed_Right);
	if(Speed_Left<0&&Speed_Right<0)motor(0,-Speed_Left,0,-Speed_Right);
	if(Speed_Left>0&&Speed_Right<0)motor(1,Speed_Left,0,-Speed_Right);
}

void turn_angle(int ang,int speed)
{
    int angle0,angle1,angle2;//angle0为初始角，angle1为目标角度，angle2为首次转角（为了使转角更快更稳，采用两段式转发）
//    use_compass=1;
    rt_thread_delay(25);
    angle0=Angle_z;
    angle1=(angle0+ang+360)%360;
    angle2=(angle0+ang/2+360)%360;
    if(ang>0)
    {
        while(!(angle2==Angle_z||angle2==Angle_z-1||angle2==Angle_z+1||angle2==Angle_z+2))//先以设定速度转一半
        {
            mot(-speed,speed);
            rt_thread_delay(5);
        }
        motor_stop();
        rt_thread_delay(10);
        while(!(angle1==Angle_z||angle1==Angle_z-1||angle1==Angle_z+1))//以100的速度转到指定角度
        {
            mot(-170,170);
            rt_thread_delay(5);
        }
    }
    else
    {
        while(!(angle2==Angle_z||angle2==Angle_z-1||angle2==Angle_z-2||angle2==Angle_z+1))
        {
            mot(speed,-speed);
            rt_thread_delay(5);
        }
        motor_stop();
        rt_thread_delay(10);
        while(!(angle1==Angle_z||angle1==Angle_z-1||angle1==Angle_z-2||angle1==Angle_z+1))
        {
            mot(150,-150);
            rt_thread_delay(5);
        }
    }
    motor_stop();
//    use_compass=0;
		rt_thread_delay(100);
}


void motorr(int left_speed,int right_speed)
{
	mot(- left_speed,- right_speed);
}
/*
void mot(int16_t speed_left,int16_t speed_right)
{
	if(speed_left>=0&&speed_right>=0)
	{
		motor(0,1,speed_left);      //左轮向前
		motor(0,0,speed_right);      //右轮向前
	}
	else
	{
		if(speed_left<0&&speed_right<0)
		{
			motor(1,1,-speed_left); //左轮向后
			motor(1,0,-speed_right); //右轮向后
		}
		else if(speed_left<0&&speed_right>0)
		{
			motor(1,1,-speed_left); //左轮向后
			motor(0,0,speed_right);  //右轮向前
		}
		else
		{
			motor(0,1,speed_left);  //左轮向前
			motor(1,0,-speed_right); //右轮向后
		}
	}
}
*/

void motor_init(void)
{
    Dir_Init();
	PWM_Init(1054-1,7-1);
	motor_stop();
}














