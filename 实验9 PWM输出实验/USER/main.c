#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "key.h"  
#include "pwm.h" 
#include "usart2.h"
#include "usart3.h" 
#include "bsp_oled.h"


int main(void)
{        

	float pp[4];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init(168);			//延时初始化  
//	uart_init(115200);		//初始化串口波特率为115200 
//	usart2_init(42,115200);		//初始化串口2波特率为115200
		usart4_init(115200);	//串口初始化波特率为115200
	LED_Init();					//初始化LED 

	KEY_Init();					//按键初始化
	


 	TIM4_PWM_Init(7999,209);//辉
	PWM_RESET();
	oled_init();
	delay_ms(25);
//	LED0=~LED0;

	OLED_Clear();
	OLED_ShowNum(0,0,12,4,12);
	
	while(1)
	{
		
	
				OLED_ShowNum(0,6,USART_RX_BUF4[0],4,12);
				OLED_ShowNum(30,6,USART_RX_BUF4[1],4,12);
				delay_ms(25);
				Adjust_PID(USART_RX_BUF4[0],USART_RX_BUF4[1],pp);
	}

		//Adjust_PID(c_x,c_y,pp);
}



