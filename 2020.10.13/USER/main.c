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
//ALIENTEK 探索者STM32F407开发板 实验35
//摄像头 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
 
u8 ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式


#define jpeg_buf_size 2  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志



//提取R,G,B的参数
#define RGB_R 0XF800
#define RGB_G 0X07E0
#define RGB_B 0X001F


#define ki 1750
#define kj 21
u16 R,G,B;
//u16 r_R,r_G,r_B;
u16 gav;
#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//定义YUV数据缓存yuv_buf的大小(单位是字节总数除以4，因为数组为32位)(*4字节)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv数据缓存buf【__align(4)为四字节对齐】

void scan(void);
void trace(void);
//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	if(ov2640_mode)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{
			DMA2_Stream1->CR&=~(1<<0);		//停止当前传输
			while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置   
			jpeg_data_len=jpeg_buf_size-DMA2_Stream1->NDTR;//得到此次数据传输的长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	//传输长度为jpeg_buf_size*4字节
			DMA2_Stream1->CR|=1<<0;				//重新传输
			jpeg_data_ok=0;						//标记数据未采集
		}
	}
} 



u16 gray(u16 pixel)

{
    static u16 Gray;
	  /*******提取R,G,B值*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
	  return Gray;
}	


u16 Binary(u16 pixel)
{
    static u16 Gray;
	  /*******提取R,G,B值*******/
	R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	Gray = (u16)((R*634+G*613+B*232));
	
	if(R>15&&R-0.5*G>6&&R-B>6)//如果，Y值大于等于阀值18  10  10 6m/16  6  6
	{
		 Gray=0xffff;	//二值化像素为白    
	}
	else
	{
		Gray =0x0000;	//二值化像素为黑
	}
	
	return Gray;	//返回灰度图像值(RGB565格式显示)
}




u16 yuv422_to_Gray(u16 threshold,u16 RGB565)
{
	u16 Gray;	//用于储存灰度值变量(RGB565格式显示)
//	u16 temp;	//用于储存yuv422格式数据中的亮度值Y量
	
//	temp = (u8)yuv422>>8;//把yuv422格式数据中的亮度值Y量提取出来赋值给temp变量（Y值在高字节，根据OV2640寄存器设置决定）
	Gray =(((RGB565>>(8+3))<<11)|((RGB565>>(8+2))<<5)|((RGB565>>(8+3))<<0));//Y量转为灰度值(RGB565格式显示)
  
	if(Gray >= threshold)//如果，Y值大于等于阀值
	{
		 Gray=0xffff;	//二值化像素为白
	}
	else
	{
		Gray =0x0000;	//二值化像素为黑
	}
	
	return Gray;	//返回灰度图像值(RGB565格式显示)
} 


//函数功能：YUV422格式Y量二值化(RGB565格式显示)
//yuv422：yuv格式数据
//threshold：阀值

u16 yuv422_y_to_bitmap(u8 threshold,u16 yuv422)
{
	u16 bitmap;	//二值化数据变量(RGB565格式显示)
	u8 temp;	//用于储存yuv422格式数据中的亮度值Y量
	
	temp = (u8)(yuv422>>8);//把yuv422格式数据中的亮度值Y量提取出来赋值给temp变量（Y值在高字节，根据OV2640寄存器设置决定）

	if(temp >= threshold)//如果，Y值大于等于阀值
	{
		bitmap =0xffff;	//二值化像素为白
	}
	else
	{
		bitmap =0x0000;	//二值化像素为黑
	}
	
	return bitmap;	//返回二值化像素值
} 


//**************************************************//
//**  最小核心板需更改stm32f4xx.h中HSE_VALUE 25	    **//
//**  最小核心板需更改system_sym32f4xx.c中PLL_M 25	**//
//**************************************************//


int main(void)
{  
	float pp[4];
	float dis=0,wide=0;
	u8 key=0;
//	u16 threshold=80;	//二值化时用到的阀值变量   threshold=128   TEST
	u16 led0pwmval=0;    
	u8 dir=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(115200);		//初始化串口波特率为115200 
	usart2_init(42,115200);		//初始化串口2波特率为115200
	oled_all();
	oled.show_number(0,1,1,4,12);
	usart4_init(19200);
	usart3_init(19200);
	LED_Init();					//初始化LED 
	BEEP_Init();

	KEY_Init();					//按键初始化
	TIM4_PWM_Init(7999,209);//辉
	

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
				
				if (ucRxBuffer[1]==0x53)	//形状、颜色、距离、边长
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
				else if (ucRxBuffer[1]==0x52)//颜色跟踪显示
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
				else if (ucRxBuffer[1]==0x51)//球体识别
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
					
					if(ucStcAcc1[3]==1)//声光报警
				
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

