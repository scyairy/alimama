#include <rtthread.h>
#include <stm32f4xx.h>
#include "operation.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_oled.h"
#include "robot_ui.h"
#include "bsp_grayscale.h"
#include "bsp_flash.h"
#include "JY61.h"

#include "image.h"

/*  变量分配4字节对齐 */
ALIGN(RT_ALIGN_SIZE)

/*  静态线程的 线程堆栈*/
static rt_uint8_t operation_stack[1024];
static rt_uint8_t key_stack[512];
/* 静态线程的 线程控制块 */
struct rt_thread operation_thread;
struct rt_thread key_thread;

/*定义画面变量Frame(extern 到 robot_ui.c) ，并初始化为第一幅画面 ：RobotRun*/
rt_uint8_t Frame = RobotRun;
/*定义存放 从 邮箱中读取的数据 的变量keycode(extern 到 robot_ui.c)*/
rt_uint32_t keycode ;

rt_mailbox_t key_value_mb = RT_NULL;

uint8_t run_flag = 0;

    



/*线程operation_thread的入口函数*/
static void operation_thread_entry(void* paramete);




/****************************************************************************
* 功    能: 创建按键扫描线程和operation线程，以及两个线程的邮箱
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
rt_err_t operation(void)
{
    rt_err_t result;

	key_value_mb = rt_mb_create("keycode value mb",1,RT_IPC_FLAG_FIFO);
	if(key_value_mb == RT_NULL)
	{
		rt_kprintf("keycode value mailbox create failed!\r\n");
	}

	
    /* 创建operation线程 ： 优先级 16 ，时间片 5个系统滴答 */
    result = rt_thread_init(&operation_thread,
                            "operation",
                            operation_thread_entry, RT_NULL,
                            (rt_uint8_t*)&operation_stack[0], sizeof(operation_stack), 16, 5);

    if (result == RT_EOK)
    {
        rt_thread_startup(&operation_thread);
    }

    /* 创建key线程 ： 优先级 15 ，时间片 5个系统滴答 */
    result = rt_thread_init(&key_thread,
                            "keycode",
                            key_thread_entry, RT_NULL,
                            (rt_uint8_t*)&key_stack[0], sizeof(key_stack), 15, 5);

    if (result == RT_EOK)
    {
        rt_thread_startup(&key_thread);
    }
    return 0;
}

/****************************************************************************
* 功    能: 通过按键 显示 和 切换操作
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无
****************************************************************************/
void operation_thread_entry(void* paramete)
{ 

    /* 无限循环*/
    while (1)
    {
		switch(Frame)							//界面显示选择功能
		{
			case RobotRun:
						   RobotRun_Frame();
					       break;
			case GrayScale:	  
					       GrayScale_Frame();
						   break;
		    case Laser:   
						   Laser_Frame();
						   break;
			case Compass: 
						   Compass_Frame();
					       break;
			case Motor:    Motor_Frame();
					       break;
			case ShotDown:	ShotDown_Frame();
								 break;
			case Image:			Image_Frame();
								break;
			case Second_Platform:Second_Platform_Frame();
								break;
			case Third_Platform: 	Third_Platform_Frame();
								break;
			case Fourth_Platform:	Fourth_Platform_Frame();
								break;
			case Fifth_Platform:	Fifth_Platform_Frame();
								break;
			case Sixth_Platform:		Sixth_Platform_Frame();
								break;
			case Gray_Front:
						   Gray_Front_Frame();
					       break;
			case Gray_Back:
						   Gray_Back_Frame();
						   break;
			case Gray_Bool:
						   GrayBool_Frame();
						   break;
			default:				
						   break;
		}
        if (rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
        {
            /*判断邮件内容*/
			
			if(keycode == KEY_VALUE_ENSURE)   			//相应功能操作实现
			{

				switch(Frame)
				{
					case RobotRun:
						          RobotRun_Operation();			//读取各个灰度值
							      break;
					case Laser:	  
					              Laser_Operation();			//读取激光传感器的值
							      break;
				    case Compass:   
								  Compass_Operation();					//读取角度传感器的值
								  break;
					case Motor: 
						          Motor_Operation();				//读取小车速度(待完成)
					              break;
					case ShotDown:
											ShotDown_Operation();
											break;
					case Second_Platform:
											Second_image();
											break;
					case Third_Platform:
											Third_image();
											break;
					case Fourth_Platform:
											Fourth_image();
											break;
					case Fifth_Platform:
											Fifth_image();
											break;
					case Sixth_Platform:
											Sixth_image();
											break;
					case Gray_Front:
           						  GrayFront_Operation();	//读取前灰度的值
					              break;
					case Gray_Back:
								  GrayBack_Operation();					//读取后灰度的值
						          break;
					case Gray_Bool:
						          Bool_Operation();					//读取bool值
					              break;
					case GrayScale:
								  Frame = Gray_Front;
								  break;
					case Image:
									Frame = Second_Platform;
									break;
					default:				
								  break;				
				}
			
			}					   
		   else if(keycode == KEY_VALUE_DOWN)		//按键选择
		   {
				if(Frame == Gray_Front)
				{
					Frame = Gray_Back;
				}
				else if(Frame == Gray_Back)
				{
					Frame = Gray_Bool;
				}
				else if(Frame == Gray_Bool)
				{
					Frame = Gray_Front ;
				}
				else if(Frame==Second_Platform)
				{
					Frame=Third_Platform;
				}
				else if(Frame==Third_Platform)
				{
					Frame=Fourth_Platform;
				}
				else if(Frame==Fourth_Platform)
				{
					Frame=Fifth_Platform;
				}
				else if(Frame==Fifth_Platform)
				{
					Frame=Sixth_Platform;
				}
				else if(Frame==Sixth_Platform)
				{
					Frame=Second_Platform;
				}
				else
				{
					if(++Frame > Image)
					{
						Frame = RobotRun;
					}
				}
			   
		   }
		   else if(keycode == KEY_VALUE_UP)		//按键选择
		   {
				if(Frame == Gray_Bool)
				{
					Frame = Gray_Back;
				}
				else if(Frame == Gray_Back)
				{
					Frame = Gray_Front;
				}
				else if(Frame==Gray_Front)
				{
					Frame = Gray_Bool;
				}
				else if(Frame==Second_Platform)
				{
					Frame=Sixth_Platform;
				}
				else if(Frame==Third_Platform)
				{
					Frame=Second_Platform;
				}
				else if(Frame==Fourth_Platform)
				{
					Frame=Third_Platform;
				}
				else if(Frame==Fifth_Platform)
				{
					Frame=Fourth_Platform;
				}
				else if(Frame==Sixth_Platform)
				{
					Frame=Fifth_Platform;
				}
				else
				{
					if(--Frame < RobotRun)
					{
						Frame = Image;
					}
				}		
		   }
		   else if(keycode == KEY_VALUE_CANCEL)			//确认键
		   {
			   if(Frame == Gray_Front||Frame == Gray_Back||Frame == Gray_Bool)
			   {
				   Frame = GrayScale;
			   }
				 if(Frame == Second_Platform||Frame == Third_Platform||Frame == Fourth_Platform||Frame == Fifth_Platform||Frame == Sixth_Platform)
				 {
					Frame = Image;
				 }
		   }
          		   

        }	  
     }
}

void RobotRun_Operation(void)	//读取各个灰度值
{
	Flash_Read16BitDatas(FrontGrayscaleAddr,8,F_white_green);
	Flash_Read16BitDatas(FrontGrayscaleAddr+2*8,8,F_white_black);
	Flash_Read16BitDatas(FrontGrayscaleAddr+2*16,8,F_green_black);
	Flash_Read16BitDatas(BackGrayscaleAddr,8,B_white_green);
	Flash_Read16BitDatas(BackGrayscaleAddr+2*8,8,B_white_black);
	Flash_Read16BitDatas(BackGrayscaleAddr+2*16,8,B_green_black);
//	Flash_Read16BitDatas(MidGrayscaleAddr,8,MidGrayscale);
	

	run_flag = 1;
	
	
}

//void GrayFront_Operation(void)		//前灰度操作改ing
//{
//	static uint8_t flag = 0;
//    static int16_t* color = F_White;
//	rt_int16_t H_SETPOINT=0;
//	OLED_Clear();
//	OLED_ShowString(0,0,"white:",12);
//	while(1)
//	{
//		Get_FGrayscale(color);
//		Show_Grayscale(color);
//		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
//		{
//			if(flag == 0)
//			{
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//				    OLED_ShowString(0,0,"green:",12);
//					flag = 1 ;
//					color = F_Green;
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					Frame = Gray_Front ;
//					break;
//				}
//			}
//			else if(flag == 1)
//			{
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//					OLED_ShowString(0,0,"black:",12);		
//					color = F_Black;
//					flag = 2;
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					Frame = Gray_Front ;
//					break;
//				}
//			}
//			else if(flag == 2)
//			{				 		
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//          /*flash 储存 average of white-green*/
//          /*flash 储存 average of white-black*/
//					/*flash 储存 average of green-black*/
//					OLED_ShowString(0,0,"MidGraycale:",12);	
//					SaveSystemParam(F_Grayscale);
//					

//					flag = 3;
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					Frame = Gray_Front ;
//					break;
//				}
//			}
//			else if(flag == 3)
//			{

//				if(keycode == KEY_VALUE_ENSURE)
//				{

//					color = Mid_Grayscle;
//					SaveSystemParam(M_Grayscale);	
//		OLED_ShowString(0,6,"H_SETPOINT:",12);
//		H_SETPOINT=Get_H_SETPOINT();
//		OLED_ShowNum(0,7,H_SETPOINT,4,12);					
////					OLED_ShowString(0,6,"save completed:",12);					
//				}					
//				if (rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
//					{
//						if(keycode == KEY_VALUE_CANCEL)
//						{
//							flag = 0;
//							color = F_White;
//							Frame = Gray_Front ;
//							break;
//						}
//						break;
//					}
//					
//				
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					flag = 0;
//					color = F_White;					
//					Frame = Gray_Front ;
//					break;
//				}
//			}
//		}
//	}
	
//}

void GrayFront_Operation(void)		//前灰度操作
{
	static uint8_t flag = 0;
    static int16_t* color = F_White;
	
	OLED_Clear();
	OLED_ShowString(0,0,"white:",12);
	while(1)
	{
		Get_FGrayscale(color);
		Show_Grayscale(color);
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(flag == 0)
			{
				if(keycode == KEY_VALUE_ENSURE)
				{
				    OLED_ShowString(0,0,"green:",12);
					flag = 1 ;
					color = F_Green;
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Front ;
					break;
				}
			}
			else if(flag == 1)
			{
				if(keycode == KEY_VALUE_ENSURE)
				{
					OLED_ShowString(0,0,"black:",12);		
					color = F_Black;
					flag = 2;
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Front ;
					break;
				}
			}
			else if(flag == 2)
			{				 		
				if(keycode == KEY_VALUE_ENSURE)
				{
           /*flash 储存 average of white-green*/
          /*flash 储存 average of white-black*/
					/*flash 储存 average of green-black*/
					
					SaveSystemParam(F_Grayscale);
					OLED_ShowString(0,6,"save completed:",12);	
					if (rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
					{
						if(keycode == KEY_VALUE_CANCEL)
						{
							flag = 0;
							color = F_White;
							Frame = Gray_Front ;
							break;
						}
					}
					
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					flag = 0;
					color = F_White;					
					Frame = Gray_Front ;
					break;
				}
			}
		}
	}

}


//void GrayBack_Operation(void)			//后灰度操作改ing
//{
//	static uint8_t flag = 0; 
//	static int16_t* color = F_White;
//	rt_int16_t H_SETPOINT=0;
//	OLED_Clear();
//	OLED_ShowString(0,0,"one:",12);
//	while(1)
//	{
//		Get_FGrayscale(color);
//		Show_Grayscale(color);
//		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
//		{
//			if(flag == 0)
//			{
//				 	
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//					OLED_ShowString(0,0,"two:",12);
//					flag = 1 ;
//					color = F_Green;
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					Frame = Gray_Front;
//					break;
//				}
//			}
//			else if(flag == 1)
//			{
//	
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//					OLED_ShowString(0,0,"three:",12);
//					color = F_Black;
//					flag = 2;
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					Frame = Gray_Front;
//					break;
//				}
//			}
//			else if(flag == 2)
//			{
//				if(keycode == KEY_VALUE_ENSURE)
//				{
//          /*flash 储存 average of white-green*/
//          /*flash 储存 average of white-black*/
//					/*flash 储存 average of green-black*/
//					SaveSystemParam(B_Grayscale);
//		OLED_ShowString(0,6,"H_SETPOINT:",12);
//		H_SETPOINT=Get_H_SETPOINT();
//		OLED_ShowNum(0,7,H_SETPOINT,4,12);						
//				//	OLED_ShowString(0,2,"storage completed:",12);	
//					if (rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
//					{
//						if(keycode == KEY_VALUE_CANCEL)
//						{
//							flag = 0;
//							color = F_White;
//							Frame = Gray_Front;
//							break;
//						}
//					}
//				}
//				else if(keycode == KEY_VALUE_CANCEL)
//				{
//					flag = 0;
//					color = F_White;
//					Frame = Gray_Front;
//					break;
//				}
//			}
//		}
//	}
//}

void GrayBack_Operation(void)			//后灰度操作
{
	static uint8_t flag = 0; 
	static int16_t* color = B_White;\
	rt_int16_t H_SETPOINT=0;
	OLED_Clear();
	OLED_ShowString(0,0,"white:",12);
	while(1)
	{
		Get_BGrayscale(color);
		Show_Grayscale(color);
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(flag == 0)
			{
				 	
				if(keycode == KEY_VALUE_ENSURE)
				{
					OLED_ShowString(0,0,"green:",12);
					flag = 1 ;
					color = B_Green;
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Back;
					break;
				}
			}
			else if(flag == 1)
			{
	
				if(keycode == KEY_VALUE_ENSURE)
				{
					OLED_ShowString(0,0,"black:",12);
					color = B_Black;
					flag = 2;
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Back;
					break;
				}
			}
			else if(flag == 2)
			{
				if(keycode == KEY_VALUE_ENSURE)
				{
          /*flash 储存 average of white-green*/
          /*flash 储存 average of white-black*/
					/*flash 储存 average of green-black*/
					SaveSystemParam(B_Grayscale);					
					OLED_ShowString(0,2,"storage completed:",12);	
					if (rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
					{
						if(keycode == KEY_VALUE_CANCEL)
						{
							flag = 0;
							color = B_White;
							Frame = Gray_Back;
							break;
						}
					}
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					flag = 0;
					color = B_White;
					Frame = Gray_Back;
					break;
				}
			}
		}
	}
}	

void Laser_Operation(void)			//激光操作
{
	/*left :   right :  获取激光的值并显示*/
	OLED_Clear();
	while(1)
	{
	    OLED_ShowString(0,0,"L:",12);
	    OLED_ShowString(50,0,"R:",12);
			OLED_ShowNum(10,0,Infrared1,3,12);
			OLED_ShowNum(60,0,Infrared2,3,12);
			OLED_ShowNum(10,1,laser_l,3,12);
			OLED_ShowNum(60,2,laser_r,3,12);
		if(rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL) break;
		}
	}

}

void Compass_Operation(void)		//角度传感器操作[有GY26和JY61区别]
{
	OLED_Clear();
	while(1)
	{
		show_angle();	//显示角度值
		if(rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL) break;
		}
	}
	
}
void Motor_Operation(void)
{
	OLED_Clear();
	while(1)
	{
		OLED_ShowString(0,0,"L_Speed:",12);
		OLED_ShowString(0,2,"R_Speed:",12);
		if(rt_mb_recv(key_value_mb, &keycode, RT_WAITING_FOREVER) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL) break;
		}
	}	

}

void Bool_Operation(void)		//bool量操作
{
	static uint8_t flag = 0; 
	static int16_t* F_color = F_white_green;
	static int16_t* B_color = B_white_green;
	

	
	Flash_Read16BitDatas(FrontGrayscaleAddr,8,F_white_green);
	Flash_Read16BitDatas(FrontGrayscaleAddr+2*8,8,F_white_black);
	Flash_Read16BitDatas(FrontGrayscaleAddr+2*16,8,F_green_black);
	
	
	Flash_Read16BitDatas(BackGrayscaleAddr,8,B_white_green);
	Flash_Read16BitDatas(BackGrayscaleAddr+2*8,8,B_white_black);
	Flash_Read16BitDatas(BackGrayscaleAddr+2*16,8,B_green_black);
	
	OLED_Clear();
	OLED_ShowString(0,0,"white-green:",12);
	OLED_ShowString(0,2,"Front:",12);
	OLED_ShowString(0,4,"Back:",12);
	while(1)
	{
		F_Compare(F_color);
		B_Compare(B_color);
		Show_BackBool();
		Show_FrontBool();
		
		
		if (rt_mb_recv(key_value_mb, &keycode, 2) == RT_EOK)
		{
			if(flag == 0)
			{
				if(keycode == KEY_VALUE_ENSURE)
				{
				    OLED_ShowString(0,0,"white-black:",12);
					flag = 1 ;
					F_color = F_white_black;
					B_color = B_white_black;				
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Bool ;
					break;
				}
			}
			else if(flag == 1)
			{
				if(keycode == KEY_VALUE_ENSURE)
				{
					OLED_ShowString(0,0,"green-black:",12);		
					F_color = F_green_black;
					B_color = B_green_black;
					flag = 2;
				}
				else if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Gray_Bool ;
					break;
				}
			}
			else if(flag == 2)
			{				 		

			    if(keycode == KEY_VALUE_CANCEL)
				{
					flag = 0;
					F_color = F_white_green;
					B_color = B_white_green;					
					Frame = Gray_Bool ;
					break;
				}
			}
		}
	}
}

void ShotDown_Operation(void)
{
	get_image('s');
}

void Second_image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=get_image('a');

	OLED_ShowNum(0,0,USART_RX_BUF,4,12);
	while(1)
	{
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL)
			{
				Frame = Second_Platform ;
				break;
			}
		}
	}
}
void Third_image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=get_image('b');
	
	OLED_Clear();
	OLED_ShowNum(0,0,USART_RX_BUF,4,12);
	while(1)
	{
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL)
			{
				Frame = Third_Platform ;
				break;
			}
		}
	}
}
void Fourth_image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=get_image('c');
	OLED_Clear();
	OLED_ShowNum(0,0,USART_RX_BUF,4,12);
	while(1)
	{
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL)
			{
				Frame = Fourth_Platform ;
				break;
			}
		}
	}
}
void Fifth_image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=get_image('d');
	OLED_Clear();
	OLED_ShowNum(0,0,USART_RX_BUF,4,12);
	while(1)
	{
		if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
		{
			if(keycode == KEY_VALUE_CANCEL)
			{
				Frame = Fifth_Platform ;
				break;
			}
		}
	}
}
void Sixth_image(void)
{
		uint16_t USART_RX_BUF;
		USART_RX_BUF=get_image('e');
		OLED_Clear();
	OLED_ShowNum(0,0,USART_RX_BUF,4,12);
		while(1)
		{
			if (rt_mb_recv(key_value_mb, &keycode, 5) == RT_EOK)
			{
				if(keycode == KEY_VALUE_CANCEL)
				{
					Frame = Sixth_Platform ;
					break;
				}
			}
		}
}




