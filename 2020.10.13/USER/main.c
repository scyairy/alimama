#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "pwm.h"
//#include "usmart.h"  
#include "usart2.h"  
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h"
#include "bsp_oled.h"
#include "usart3.h"
#include "beep.h"
#include "usart4.h"
//ALIENTEK ̽����STM32F407������ ʵ��35
//����ͷ ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
 
 
u8 ov2640_mode=0;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ


#define jpeg_buf_size 2  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־



//��ȡR,G,B�Ĳ���
#define RGB_R 0XF800
#define RGB_G 0X07E0
#define RGB_B 0X001F


#define ki 1750
#define kj 21
u16 R,G,B;
//u16 r_R,r_G,r_B;
u16 gav;
#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//����YUV���ݻ���yuv_buf�Ĵ�С(��λ���ֽ���������4����Ϊ����Ϊ32λ)(*4�ֽ�)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv���ݻ���buf��__align(4)Ϊ���ֽڶ��롿

void scan(void);
void trace(void);
//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	if(ov2640_mode)//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{
			DMA2_Stream1->CR&=~(1<<0);		//ֹͣ��ǰ����
			while(DMA2_Stream1->CR&0X01);	//�ȴ�DMA2_Stream1������   
			jpeg_data_len=jpeg_buf_size-DMA2_Stream1->NDTR;//�õ��˴����ݴ���ĳ���
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA2_Stream1->CR|=1<<0;				//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
	}
} 



u16 gray(u16 pixel)

{
    static u16 Gray;
	  /*******��ȡR,G,Bֵ*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
	  return Gray;
}	


u16 Binary(u16 pixel)
{
    static u16 Gray;
	  /*******��ȡR,G,Bֵ*******/
	R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	Gray = (u16)((R*634+G*613+B*232));
	
	if(R>15&&R-0.5*G>6&&R-B>6)//�����Yֵ���ڵ��ڷ�ֵ18  10  10 6m/16  6  6
	{
		 Gray=0xffff;	//��ֵ������Ϊ��    
	}
	else
	{
		Gray =0x0000;	//��ֵ������Ϊ��
	}
	
	return Gray;	//���ػҶ�ͼ��ֵ(RGB565��ʽ��ʾ)
}




u16 yuv422_to_Gray(u16 threshold,u16 RGB565)
{
	u16 Gray;	//���ڴ���Ҷ�ֵ����(RGB565��ʽ��ʾ)
//	u16 temp;	//���ڴ���yuv422��ʽ�����е�����ֵY��
	
//	temp = (u8)yuv422>>8;//��yuv422��ʽ�����е�����ֵY����ȡ������ֵ��temp������Yֵ�ڸ��ֽڣ�����OV2640�Ĵ������þ�����
	Gray =(((RGB565>>(8+3))<<11)|((RGB565>>(8+2))<<5)|((RGB565>>(8+3))<<0));//Y��תΪ�Ҷ�ֵ(RGB565��ʽ��ʾ)
  
	if(Gray >= threshold)//�����Yֵ���ڵ��ڷ�ֵ
	{
		 Gray=0xffff;	//��ֵ������Ϊ��
	}
	else
	{
		Gray =0x0000;	//��ֵ������Ϊ��
	}
	
	return Gray;	//���ػҶ�ͼ��ֵ(RGB565��ʽ��ʾ)
} 


//�������ܣ�YUV422��ʽY����ֵ��(RGB565��ʽ��ʾ)
//yuv422��yuv��ʽ����
//threshold����ֵ

u16 yuv422_y_to_bitmap(u8 threshold,u16 yuv422)
{
	u16 bitmap;	//��ֵ�����ݱ���(RGB565��ʽ��ʾ)
	u8 temp;	//���ڴ���yuv422��ʽ�����е�����ֵY��
	
	temp = (u8)(yuv422>>8);//��yuv422��ʽ�����е�����ֵY����ȡ������ֵ��temp������Yֵ�ڸ��ֽڣ�����OV2640�Ĵ������þ�����

	if(temp >= threshold)//�����Yֵ���ڵ��ڷ�ֵ
	{
		bitmap =0xffff;	//��ֵ������Ϊ��
	}
	else
	{
		bitmap =0x0000;	//��ֵ������Ϊ��
	}
	
	return bitmap;	//���ض�ֵ������ֵ
} 


//**************************************************//
//**  ��С���İ������stm32f4xx.h��HSE_VALUE 25	    **//
//**  ��С���İ������system_sym32f4xx.c��PLL_M 25	**//
//**************************************************//


int main(void)
{  
	float pp[4];
	float dis=0,wide=0;
	u8 key=0;
//	u16 threshold=80;	//��ֵ��ʱ�õ��ķ�ֵ����   threshold=128   TEST
	u16 led0pwmval=0;    
	u8 dir=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200 
	usart2_init(42,115200);		//��ʼ������2������Ϊ115200
	oled_all();
	oled.show_number(0,1,1,4,12);
	usart4_init(19200);
	usart3_init(19200);
	LED_Init();					//��ʼ��LED 
	BEEP_Init();

	KEY_Init();					//������ʼ��
	TIM4_PWM_Init(7999,209);//��
	

		PWM_RESET();
		oled.show_number(0,2,2,4,12);
		oled.clear();
	while(1)
	{
				
//				while(1)
//				{
//						URSART3_Recieve();
//				}
				key=KEY_Scan(0);
				switch(key)
				{
					case KEY0_PRES:USART_SendData(UART4, 4);         
												while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
												break;
					case KEY1_PRES:
												GPIO_ResetBits(GPIOB,GPIO_Pin_9);
												USART_SendData(UART4, 2); 
											
												while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
												break;		
					case KEY2_PRES:
												GPIO_ResetBits(GPIOB,GPIO_Pin_9);
												USART_SendData(UART4, 3);         
												
												while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
												break;	
					case WKUP_PRES:
												GPIO_ResetBits(GPIOB,GPIO_Pin_9);
												USART_SendData(UART4, 1);         
												while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);	
												break;
					default: delay_ms(200);				
				}
				
				if (ucRxBuffer[1]==0x53)	//��״����ɫ�����롢�߳�
				{

					oled.show_string(0,0,"color:",16);
					oled.show_string(0,2,"object:",16);
					oled.show_string(0,4,"dis:",16);
					oled.show_string(0,6,"wide:",16);
				URSART3_Recieve();
				oled.show_number(40,4,z,1,16);
//				oled.show_string(48,6,".",16);
				oled.show_number(48,4,s1,1,16);
				oled.show_number(56,4,s2,1,16);
				oled.show_number(64,4,s3,1,16);
					if(stcAngle1[0]==5) oled.show_string(60,0,"red",16);
					else if(stcAngle1[0]==6) oled.show_string(60,0,"green",16);	
					else if (stcAngle1[0]==7) oled.show_string(60,0,"blue",16);
					else oled.show_string(60,0,"NONE",16);
					
					if(stcAngle1[1]==8) oled.show_string(64,2,"squre",16);
					else if(stcAngle1[1]==9) oled.show_string(64,2,"circle",16);
					else if (stcAngle1[1]==10) oled.show_string(64,2,"triangle",16);
					else oled.show_string(64,2,"NONE",16);
					
					if(stcAngle1[5]==9)	oled.show_string(70,6,"over",16);
					
					if(stcAngle1[2]*2.11<=38 && stcAngle1[2]*2.11>=31)
					{
							oled.show_number(56,6,stcAngle1[2]*2.11*100,4,16);
					}
					else oled.show_number(56,6,3546,4,16);
//					dis= ki*100/stcAngle1[2]*2/100;
//					oled.show_number(40,4,dis,6,16);
//					wide= kj*100/stcAngle1[2]*2/10;
//					oled.show_number(40,6,wide,6,16);
					delay_ms(2000);
					oled.clear();
					
					if(stcAngle1[4]==1)
					{
						LED0=0;
						GPIO_SetBits(GPIOB,GPIO_Pin_9);
						delay_ms(500);
						LED0=1;
						GPIO_ResetBits(GPIOB,GPIO_Pin_9);
					}
				}
				else if (ucRxBuffer[1]==0x52)//��ɫ������ʾ
				{
					Adjust_PID(stcGyro1[0],stcGyro1[1],pp);
					oled.show_string(0,0,"center_x:",16);
					oled.show_string(0,2,"center_y:",16);
					oled.show_number(70,0,stcGyro1[0],2,16);
					oled.show_number(70,2,stcGyro1[1],2,16);
////					if(stcGyro1[2]!=0) 
////					{
////						LED0=0;
////						GPIO_SetBits(GPIOB,GPIO_Pin_9);
////						delay_ms(500);
////						LED0=1;
////						GPIO_ResetBits(GPIOB,GPIO_Pin_9);
////					}
					
				}
				else if (ucRxBuffer[1]==0x51)//����ʶ��
				{
					delay_ms(200);
					oled.clear();
					URSART3_Recieve();
					Adjust_PID(ucStcAcc1[0],ucStcAcc1[1],pp);
					oled.show_string(0,0,"center_x:",16);
					oled.show_string(0,2,"center_y:",16);
					oled.show_string(0,4,"ball:",16);
					oled.show_string(0,6,"dis:",16);
					oled.show_number(40,6,z,1,16);
//				oled.show_string(48,6,".",16);
					oled.show_number(48,6,s1,1,16);
					oled.show_number(56,6,s2,1,16);
					oled.show_number(64,6,s3,1,16);
					oled.show_number(70,0,ucStcAcc1[0],2,16);
					oled.show_number(70,2,ucStcAcc1[1],2,16);
					if(ucStcAcc1[2]==3) oled.show_string(50,4,"basketball",16);
					else if(ucStcAcc1[2]==1) oled.show_string(50,4,"volleyball",16);
					else if(ucStcAcc1[2]==2) oled.show_string(50,4,"football",16);
					else oled.show_string(50,4,"football",16);
					
					if(ucStcAcc1[3]==1)//���ⱨ��
				
					{
							LED0=0;
							GPIO_SetBits(GPIOB,GPIO_Pin_9);
							delay_ms(500);
							LED0=1;
							GPIO_ResetBits(GPIOB,GPIO_Pin_9);							
					}
				}
					
				
				
				
				

//			URSART3_Recieve();
//				oled.show_number(20,6,z,1,16);
////				oled.show_string(48,6,".",16);
//				oled.show_number(28,6,s1,1,16);
//				oled.show_number(36,6,s2,1,16);
//				oled.show_number(44,6,s3,1,16);

	}
		
	
	

}

