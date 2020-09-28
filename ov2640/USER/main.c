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

u32 sum_x,sum_y,sum_1,sum_2;
u16 R,G,B;
//u16 r_R,r_G,r_B;
u16 gav;
#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//定义YUV数据缓存yuv_buf的大小(单位是字节总数除以4，因为数组为32位)(*4字节)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv数据缓存buf【__align(4)为四字节对齐】


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


int main(void)
{        
//	u8 key=0;
//	u16 threshold=80;	//二值化时用到的阀值变量   threshold=128   TEST
	
	u16 led0pwmval=0;    
	u8 dir=1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(115200);		//初始化串口波特率为115200 
	usart2_init(42,115200);		//初始化串口2波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化
 	KEY_Init();					//按键初始化
	TIM4_PWM_Init(1999,839);//辉
	// TIM14_PWM_Init(3000-1,200-1);	//楠

 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	 
	while(OV2640_Init())//初始化OV2640
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
		DCMI_DMA_Init((u32)&yuv_buf,yuv_buf_size,2,1);//DCMI DMA配置
	
		OV2640_OutSize_Set(220,280);//OV2640输出图像尺寸为：176X144
		PWM_down=50;
		PWM_up=248;
////////////////////////////////////////////////////////////////////////////////////////		
	// PWM_down=150;
	// PWM_up=248;//待测
	// while(1)
	// {
	// 		u16 i,temp_h,temp_l,temp,gray0,gray1;
	// 		// u16 Graymax=0;
	// 		// u16 Graymin=65535;
	// 		// delay_ms(10);

	// 		OV2640_RGB565_Mode();
	// 		DCMI_Start();//启动传输
	// 		DCMI_Stop(); //停止显示

	// 		LCD_Set_Window(0,0,220,280);	//设置窗口       -----屏幕显示二值化图--------
	// 		LCD_WriteRAM_Prepare();	//开始写入GRAM

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
	// 			PWM_up=248;//待测
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
				DCMI_Start(); 		//启动传输
				DCMI_Stop(); //停止显示
	
				LCD_Set_Window(0,0,220,280);	//设置窗口       -----屏幕显示二值化图--------
				LCD_WriteRAM_Prepare();	//开始写入GRAM
		
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
//舵机pwm测试	
// while(1)
// {
//		 TIM_SetCompare1(TIM14,50);	//修改比较值，修改占空比
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,100);	//修改比较值，修改占空比
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,150);	//修改比较值，修改占空比
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,200);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,250);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,200);
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,150);	//修改比较值，修改占空比
//		delay_ms(1000);
//		TIM_SetCompare1(TIM14,100);	//修改比较值，修改占空比
//		delay_ms(1000);
// }

}
