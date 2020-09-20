#include <board.h>
#include <rtthread.h>
#include "bsp_oled.h"
#include "robot_ui.h"
#include "bsp_grayscale.h"
#include "bsp_key.h"
#include "operation.h"

void robot_ui_init(void)
{
	if(KEY_OK)
	{
	OLED_Clear();	
	OLED_ShowString(0,0,"->RobotRun",12);
	OLED_ShowString(0,2,"  GrayScale",12);
	OLED_ShowString(0,4,"  Laser",12);
	OLED_ShowString(0,6,"  Compass",12);
		while(!KEY_OK)
		{
			rt_thread_delay(10);
		}
		rt_thread_delay(500);
	}
	else
		RobotRun_Operation();
}

void RobotRun_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->RobotRun",12);
	OLED_ShowString(0,2,"  GrayScale",12);
	OLED_ShowString(0,4,"  Laser",12);
	OLED_ShowString(0,6,"  Compass",12);
}
void GrayScale_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->GrayScale",12);
	OLED_ShowString(0,2,"  Laser",12);
	OLED_ShowString(0,4,"  Compass",12);
	OLED_ShowString(0,6,"  Motor",12);
}
void Laser_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Laser",12);
	OLED_ShowString(0,2,"  Compass",12);
	OLED_ShowString(0,4,"  Motor",12);
	OLED_ShowString(0,6,"  ShotDown",12);
}
void Compass_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Compass",12);
	OLED_ShowString(0,2,"  Motor",12);
	OLED_ShowString(0,4,"  ShotDown",12);
	OLED_ShowString(0,6,"  Image",12);
}
void Motor_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Motor",12);
	OLED_ShowString(0,2,"  ShotDown",12);
	OLED_ShowString(0,4,"  Image",12);
	OLED_ShowString(0,6,"  RobotRun",12);
}
void ShotDown_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->ShotDown",12);
	OLED_ShowString(0,2,"  Image",12);
	OLED_ShowString(0,4,"  RobotRun",12);
	OLED_ShowString(0,6,"  GrayScale",12);
}
void Image_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Image",12);
	OLED_ShowString(0,2,"  RobotRun",12);
	OLED_ShowString(0,4,"  GrayScale",12);
	OLED_ShowString(0,6,"  Laser",12);
}

void Gray_Front_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Front",12);
//	OLED_ShowString(0,2,"  Mid",12);
	OLED_ShowString(0,2,"  Back",12);
	OLED_ShowString(0,4,"  Bool",12);
}
void Gray_Back_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"  Front",12);
//	OLED_ShowString(0,2,"->Mid",12);
	OLED_ShowString(0,2,"->Back",12);
	OLED_ShowString(0,4,"  Bool",12);
}
void GrayBool_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"  Front",12);
//	OLED_ShowString(0,2,"  Mid",12);
	OLED_ShowString(0,2,"  Back",12);
	OLED_ShowString(0,4,"->Bool",12);	
//	OLED_ShowString(0,4,"->H_SETPOINT",12);
}

void Second_Platform_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Second",12);
	OLED_ShowString(0,2,"  Third",12);
	OLED_ShowString(0,4,"  Forth",12);
	OLED_ShowString(0,6,"  Fifth",12);
}
void Third_Platform_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Third",12);
	OLED_ShowString(0,2,"  Forth",12);
	OLED_ShowString(0,4,"  Fifth",12);
	OLED_ShowString(0,6,"  Sixth",12);
}
void Fourth_Platform_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Forth",12);
	OLED_ShowString(0,2,"  Fifth",12);
	OLED_ShowString(0,4,"  Sixth",12);
	OLED_ShowString(0,6,"  Second",12);
}
void Fifth_Platform_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Fifth",12);
	OLED_ShowString(0,2,"  Sixth",12);
	OLED_ShowString(0,4,"  Second",12);
	OLED_ShowString(0,6,"  Third",12);
}
void Sixth_Platform_Frame(void)
{
	OLED_Clear();
	OLED_ShowString(0,0,"->Sixth",12);
	OLED_ShowString(0,2,"  Second",12);
	OLED_ShowString(0,4,"  Third",12);
	OLED_ShowString(0,6,"  Fourth",12);
}
void Show_FrontBool(void)
{
	OLED_ShowNum(20,3,f[0],1,12);   
	OLED_ShowNum(32,3,f[1],1,12);   
	OLED_ShowNum(44,3,f[2],1,12);    
	OLED_ShowNum(56,3,f[3],1,12);    
	OLED_ShowNum(68,3,f[4],1,12);   
	OLED_ShowNum(80,3,f[5],1,12);    
	OLED_ShowNum(92,3,f[6],1,12);    
	OLED_ShowNum(104,3,f[7],1,12);	
}

void Show_BackBool(void)
{
	OLED_ShowNum(20,5,b[0],1,12);   
	OLED_ShowNum(32,5,b[1],1,12);   
	OLED_ShowNum(44,5,b[2],1,12);    
	OLED_ShowNum(56,5,b[3],1,12);    
	OLED_ShowNum(68,5,b[4],1,12);   
	OLED_ShowNum(80,5,b[5],1,12);    
	OLED_ShowNum(92,5,b[6],1,12);    
	OLED_ShowNum(104,5,b[7],1,12);	
	
}
void Show_Grayscale(int16_t* grayscale)
{
	OLED_ShowNum(0,2,grayscale[0],4,12);    
	OLED_ShowNum(32,2,grayscale[1],4,12);   
	OLED_ShowNum(64,2,grayscale[2],4,12);    
	OLED_ShowNum(96,2,grayscale[3],4,12);    
	OLED_ShowNum(0,4,grayscale[4],4,12);    
	OLED_ShowNum(32,4,grayscale[5],4,12);    
	OLED_ShowNum(64,4,grayscale[6],4,12);    
	OLED_ShowNum(96,4,grayscale[7],4,12);    
}





