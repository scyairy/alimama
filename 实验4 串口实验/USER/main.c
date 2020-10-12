#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "key.h"  
#include "pwm.h" 
#include "usart2.h"
#include "usart3.h" 
#include "timer.h" 
#include "bsp_oled.h"

u8 c_x=0;
u8 c_y=0;
int main(void)
{        

	float pp[4];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200 
	usart2_init(42,115200);		//��ʼ������2������Ϊ115200
	LED_Init();					//��ʼ��LED 

	KEY_Init();					//������ʼ��
	TIM4_PWM_Init(7999,209);//��

	usart4_init(115200);	//���ڳ�ʼ��������Ϊ115200
  
	oled_init();
	
	while(1)
	{
		
				c_x=USART_RX_BUF[0];
				c_y=USART_RX_BUF[1];
				OLED_ShowNum(0,6,c_x,4,12);
				OLED_ShowNum(30,6,c_y,4,12);
				
			
	}

		Adjust_PID(c_x,c_y,pp);
}



