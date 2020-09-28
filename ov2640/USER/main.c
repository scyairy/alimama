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

u32 sum_x,sum_y,sum_1,sum_2;
u16 R,G,B;
//u16 r_R,r_G,r_B;
u16 gav;
#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//����YUV���ݻ���yuv_buf�Ĵ�С(��λ���ֽ���������4����Ϊ����Ϊ32λ)(*4�ֽ�)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv���ݻ���buf��__align(4)Ϊ���ֽڶ��롿


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


int main(void)
{        
//	u8 key=0;
//	u16 threshold=80;	//��ֵ��ʱ�õ��ķ�ֵ����   threshold=128   TEST
	
	u16 led0pwmval=0;    
	u8 dir=1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200 
	usart2_init(42,115200);		//��ʼ������2������Ϊ115200
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ��
 	KEY_Init();					//������ʼ��
	TIM4_PWM_Init(1999,839);//��
	// TIM14_PWM_Init(3000-1,200-1);	//�

 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	 
	while(OV2640_Init())//��ʼ��OV2640
	{
		LCD_ShowString(30,30,240,16,16,"OV2640 ERROR");
		delay_ms(200);
	    LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
	} 
	LCD_ShowString(30,30,200,16,16,"OV2640 OK");

	delay_ms(500);
	

		LCD_Clear(WHITE);
		POINT_COLOR=RED; 	
		MY_DMA_Init();			
		DCMI_DMA_Init((u32)&yuv_buf,yuv_buf_size,2,1);//DCMI DMA����
	
		OV2640_OutSize_Set(220,280);//OV2640���ͼ��ߴ�Ϊ��176X144
		PWM_down=50;
		PWM_up=248;
////////////////////////////////////////////////////////////////////////////////////////		
	// PWM_down=150;
	// PWM_up=248;//����
	// while(1)
	// {
	// 		u16 i,temp_h,temp_l,temp,gray0,gray1;
	// 		// u16 Graymax=0;
	// 		// u16 Graymin=65535;
	// 		// delay_ms(10);

	// 		OV2640_RGB565_Mode();
	// 		DCMI_Start();//��������
	// 		DCMI_Stop(); //ֹͣ��ʾ

	// 		LCD_Set_Window(0,0,220,280);	//���ô���       -----��Ļ��ʾ��ֵ��ͼ--------
	// 		LCD_WriteRAM_Prepare();	//��ʼд��GRAM

	// 		sum_x = 0;
	// 		sum_1 = 0;
			
	// 		for(i=0;i<200*280*2/4;i=i+1)
	// 		{
	// 			temp_l=(u16)(yuv_buf[i]);
	// 			temp_h=(u16)(yuv_buf[i]>>16);
	// 			temp_l=Binary(temp_l);
	// 			if(temp_l==0XFFFF)
	// 			{
	// 				sum_x += (i%220);
	// 				sum_1++;
	// 			}
	// 			LCD->LCD_RAM = temp_l;
	// 			temp_h=Binary(temp_h);
	// 			if(temp_h==0XFFFF)
	// 			{
	// 				sum_x += (i%220);
	// 				sum_1++;
	// 			}
	// 			LCD->LCD_RAM = temp_h;
	// 		}
	// 		// LCD_Draw_Circle(x,y,5);
	// 		// LCD_ShowString(10,200,48,16,16,"weizhi:");
	// 		LCD_ShowNum(59,200,(u32)sum_x/sum_1,6,16);	
	// 		if((sum_x/sum_1)>120)
	// 		{
	// 			PWM_down=PWM_down+2;
	// 			PWM_up=PWM_up+2;	
	// 			delay_ms(500);	 
	// 		}
	// 		else if((sum_x/sum_1)<90)
	// 		{
	// 			PWM_down=PWM_down-2;
	// 			PWM_up=PWM_up-2;	
	// 			delay_ms(500);
	// 		}
	// 		else
	// 		{
	// 			PWM_down=150;
	// 			PWM_up=248;//����
	// 		}
			
				
	// 		LCD_ShowNum(80,240,(u32)sum_x,8,16);
	// 		LCD_ShowNum(80,160,(u32)sum_1,6,16);
	// }

///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////		
	while(1)
	{
		
			u16 i,j,temp_l,temp_h,temp1,temp2,flag=0;
			u32 sumx=0,sumy=0,area=0;
			u16 xmin,xmax,ymin,ymax;
			u16 x,y,pwmr=0,pwmr2=0,sign=0,sign1=0;

				OV2640_RGB565_Mode();
				DCMI_Start(); 		//��������
				DCMI_Stop(); //ֹͣ��ʾ
	
				LCD_Set_Window(0,0,220,280);	//���ô���       -----��Ļ��ʾ��ֵ��ͼ--------
				LCD_WriteRAM_Prepare();	//��ʼд��GRAM
		
				xmin=xmax=ymin=ymax=0;
				for(i=0;i<220*280/2;i++)
				{	
					temp_l=(u16)(yuv_buf[i]);
					temp_h=(u16)(yuv_buf[i]>>16);
				  	
					temp_l=Binary(temp_l);
					
					if(temp_l==0XFFFF)
					{

						
						// sumx=sumx+(i%220);
						// sumy=sumy+(i/220);
						// area=area+1;
				
						if(flag==0)
						{
								xmin=xmax=(i*2)%220;
								ymin=ymax=(i*2)/220;
								flag=1;
						}
						else
						{
								temp1=(i*2)%220;
								temp2=(i*2)/220;
								if(temp1<xmin)
								{
									xmin=temp1;	
								}
								else if(temp1>xmax)
								{
									xmax=temp1;
								}
								if(temp2<ymin)
								{
									ymin=temp2;
									
								}
								else if(temp2>ymax)
								{
									ymax=temp2;
								
								}
									
						}
						
					}
					
					LCD->LCD_RAM = temp_l;
					
					temp_h=Binary(temp_h);
					
					if(temp_h==0XFFFF)
					{
//							sumx=sumx+(i*2)%220;
//						  sumy=sumy+(i/220);
//						  area=area+1;
		
						if(flag==0)
						{
								xmin=xmax=(i*2)%220+1;
								ymin=ymax=(i*2)/220;
								flag=1;
						}
						else
						{
								temp1=(i*2)%220+1;
								temp2=(i*2)/220;
								if(temp1<xmin)
								{
									xmin=temp1;	
								}
								else if(temp1>xmax)
								{
									xmax=temp1;
								}
								if(temp2<ymin)
								{
									ymin=temp2;
									
								}
								else if(temp2>ymax)
								{
									ymax=temp2;
								
								}
																			
					}					
				}
			 LCD->LCD_RAM = temp_h;		
			}	
			    x=(xmin+xmax)/2;
					y=(ymin+ymax)/2;
////				x=sumx/area;
////				y=sumy/area;
				LCD_ShowxNum(30+11*8,140,x,3,16,0);
				LCD_ShowxNum(30+11*8,160,y,3,16,0);
				if(x!=0 && y!=0)
				{
						LCD_Draw_Circle(x,y,5);
				}
					
					// if(x>0&&x<=220)		
					// {
					// 	dir=1;
					// 	led0pwmval=50+((110-x)*200)/110;
					// 	LCD_ShowxNum(30+11*8,140,110-x,3,16,0);
					// }
					// else if(x>220)
					// {
					// 	dir=0;
					// 	led0pwmval=250-(200*(x-110))/110;
					// 	LCD_ShowxNum(30+11*8,140,x-110,3,16,0);
					// }
					

					if(x==0)
					{
						
							
							// pwmr=pwmr+15;
							PWM_down=PWM_down+5;
							LED0=~LED0;
						
					}
					else if(x>0&&x<105)
					{
						PWM_down=PWM_down+1;						

					}
					else if(x<220&&x>115)
					{

						PWM_down=PWM_down-1;					
					}
					else
					{
						
						PWM_down=PWM_down;		
					}

										

					
					delay_ms(100);
	}
///////////////////////////////////////////////////////////////////////////////////////	
	
///////////////////////////////////////////////////////////////////////////////////////
//���pwm����	
// while(1)
// {
//		 TIM_SetCompare1(TIM14,50);	//�޸ıȽ�ֵ���޸�ռ�ձ�
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,100);	//�޸ıȽ�ֵ���޸�ռ�ձ�
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,150);	//�޸ıȽ�ֵ���޸�ռ�ձ�
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,200);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,250);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,200);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,150);	//�޸ıȽ�ֵ���޸�ռ�ձ�
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,100);	//�޸ıȽ�ֵ���޸�ռ�ձ�
//		delay_ms(1000);
// }

}
