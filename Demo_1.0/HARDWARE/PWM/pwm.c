#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器PWM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //GPIOF9复用为定时器14

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM4 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器

	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
 
										  
}  

void Adjust_PID(int16_t x,int16_t y,float *p)
{
	int16_t Integral_error_x=0,Integral_error_y=0,Last_error_x=0,Last_error_y=0;
	u8 i;
	float error_x=0,Differ_error_x=0;
	float error_y=0,Differ_error_y=0;
    int16_t last_x=105;
	int16_t last_y=155;

    float Kpx =0.8,Kpy=0.18;          //比例系数              600_0.1_0_12
	float Ki = 0.0;
    float Kd =0.04;		 //积分系数，微分系数


			if(x<last_x)
			{
				error_x=last_x-x;
				if(error_x> 50)	error_x= 50;
				if(error_x> 10)	
				{
					if(error_x<Last_error_x)
					PWM_down = (PWM_down+(error_x*Kpx+Integral_error_x*Ki-(Last_error_x-error_x)*Kd));
					else
					PWM_down = (PWM_down+(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));
					
				}
			}
			else
			{
				error_x=x-last_x;
				if(error_x> 50)	error_x= 50;
				if(error_x> 10)	
				{
					
					if(error_x<Last_error_x)
					PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki-(Last_error_x-error_x)*Kd));
					else
					PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));

				}
			}
			
			if(y<last_y)
			{
				error_y=last_y-y;
				if(error_y> 50)	error_y= 50;
				if(error_y> 10)
				{
					
					if(error_x<Last_error_x)
					PWM_up = (PWM_up-(error_y*Kpy+Integral_error_y*Ki-(Last_error_y-error_y)*Kd));
					else
					PWM_up = (PWM_up-(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
					
				}
			}
			else
			{
				error_y=y-last_y;
				if(error_y> 50)	error_y= 50;
				if(error_y> 10)
				{
					if(error_x<Last_error_x)
					PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki-(Last_error_y-error_y)*Kd));
					else
					PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
				}
			}

			// if(error_x> 50)
			// 		error_x= 50;
			// if(error_y> 50)
			// 		error_y= 50;	
			// Integral_error_x += error_x;
			// Integral_error_y += error_y;

			// if(error_x> 10)
			// {
			// 	PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));
			// }
			// if(error_y> 10)
			// {
			// 	PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
			// }	

			// last_x=x;
			// last_y=y; 		
			Last_error_x = error_x;
			Last_error_y = error_y;
			
	    *p=error_x;
			*(p+1)=error_y;
}

void Adjust_PID1(int16_t x,int16_t y)
{
	int16_t Integral_error_x=0,Integral_error_y=0,Last_error_x=0,Last_error_y=0;
	u8 i;
	float error_x=0,Differ_error_x=0;
	float error_y=0,Differ_error_y=0;
    int16_t last_x=105;
	int16_t last_y=155;

    float Kpx =0.8,Kpy=0.18;          //比例系数              600_0.1_0_12
	float Ki = 0.0;
    float Kd =0;		 //积分系数，微分系数


			if(x<last_x)
			{
				error_x=last_x-x;
				if(error_x> 50)	error_x= 50;
				if(error_x> 10)	
				{
					if(error_x<Last_error_x)
					PWM_down = (PWM_down+(error_x*Kpx+Integral_error_x*Ki-(Last_error_x-error_x)*Kd));
					else
					PWM_down = (PWM_down+(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));
					
				}
			}
			else
			{
				error_x=x-last_x;
				if(error_x> 50)	error_x= 50;
				if(error_x> 10)	
				{
					
					if(error_x<Last_error_x)
					PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki-(Last_error_x-error_x)*Kd));
					else
					PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));

				}
			}
			
			if(y<last_y)
			{
				error_y=last_y-y;
				if(error_y> 50)	error_y= 50;
				if(error_y> 10)
				{
					
					if(error_x<Last_error_x)
					PWM_up = (PWM_up-(error_y*Kpy+Integral_error_y*Ki-(Last_error_y-error_y)*Kd));
					else
					PWM_up = (PWM_up-(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
					
				}
			}
			else
			{
				error_y=y-last_y;
				if(error_y> 50)	error_y= 50;
				if(error_y> 10)
				{
					if(error_x<Last_error_x)
					PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki-(Last_error_y-error_y)*Kd));
					else
					PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
				}
			}

			// if(error_x> 50)
			// 		error_x= 50;
			// if(error_y> 50)
			// 		error_y= 50;	
			// Integral_error_x += error_x;
			// Integral_error_y += error_y;

			// if(error_x> 10)
			// {
			// 	PWM_down = (PWM_down-(error_x*Kpx+Integral_error_x*Ki+(error_x-Last_error_x)*Kd));
			// }
			// if(error_y> 10)
			// {
			// 	PWM_up = (PWM_up+(error_y*Kpy+Integral_error_y*Ki+(error_y-Last_error_y)*Kd));
			// }	

			// last_x=x;
			// last_y=y; 		
			Last_error_x = error_x;
			Last_error_y = error_y;
			
}

void PWM_TEST()
{
 	while(1)
 	{
		LED0=~LED0;
		delay_ms(500);
		PWM_up=380;
		delay_ms(500);	
			PWM_down=740;
		delay_ms(500);
			PWM_up=300;
		delay_ms(500);
			PWM_down=470;
		delay_ms(500);
			PWM_up=380;
		delay_ms(500);
			PWM_down=740;
		delay_ms(500);
			PWM_up=600;
		delay_ms(500);
			PWM_down=1000;
		delay_ms(500);
			PWM_down=470;
		delay_ms(2000);
			PWM_down=1000;
		delay_ms(2000); 
			if(PWM_down>=800) PWM_down=500;	 
	}
}

// PWM_down范围470~1000（最左） 其中740为中心,舵机2
// PWM_up范围（最上面）300~600（最下面） 其中380为中心，舵机1
void PWM_RESET()
{
		PWM_down=760;
		PWM_up=400;
		delay_ms(500);
}


