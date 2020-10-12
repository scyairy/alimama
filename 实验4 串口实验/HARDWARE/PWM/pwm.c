#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ��PWM ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //GPIOF9����Ϊ��ʱ��14
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //GPIOF9����Ϊ��ʱ��14

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM4 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���

	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
 
										  
}  

void Adjust_PID(int16_t x,int16_t y,float *p)
{
	int16_t Integral_error_x=0,Integral_error_y=0,Last_error_x=0,Last_error_y=0;
	u8 i;
	float error_x=0,Differ_error_x=0;
	float error_y=0,Differ_error_y=0;
    int16_t last_x=105;
	int16_t last_y=155;

    float Kpx =0.8,Kpy=0.18;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =0.04;		 //����ϵ����΢��ϵ��


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

    float Kpx =0.8,Kpy=0.18;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =0;		 //����ϵ����΢��ϵ��


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

// PWM_down��Χ470~1000������ ����740Ϊ����,���2
// PWM_up��Χ�������棩300~600�������棩 ����380Ϊ���ģ����1
void PWM_RESET()
{
		PWM_down=760;
		PWM_up=400;
		delay_ms(500);
}


