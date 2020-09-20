#ifndef __BSP_GRAYSCALE_H
#define __BSP_GRAYSCALE_H	
#include <rtthread.h>
#include "sys.h"


//CD4051端口定义
#define A PFout(4)	
#define B PFout(5)		 
#define C PFout(6)
#define D PFout(7)
#define E PFout(8)
#define F PFout(9)

#define hd1 HD(1,1)
#define hd2 HD(1,2)
#define hd3 HD(1,3)
#define hd4 HD(1,4)
#define hd5 HD(1,5)
#define hd6 HD(1,6)
#define hd7 HD(1,7)
#define hd8 HD(1,8)

#define hhd1 HD(2,1)
#define hhd2 HD(2,2)
#define hhd3 HD(2,3)
#define hhd4 HD(2,4)
#define hhd5 HD(2,5)
#define hhd6 HD(2,6)
#define hhd7 HD(2,7)
#define hhd8 HD(2,8)


//外部声明 前灰度用于储存灰度值的数组
extern int16_t F_White[8];                    
extern int16_t F_Green[8];                    
extern int16_t F_Black[8];                    
//外部声明 后灰度用于储存灰度值的数组
extern int16_t B_White[8];                   
extern int16_t B_Green[8];                  
extern int16_t B_Black[8];  
//外部声明 前灰度用于储存中心线灰度值的数组
extern int16_t Mid_Grayscle[8];
//外部声明 
extern uint8_t f[8];//前灰度布尔量
extern uint8_t b[8];//后灰度布尔量

//外部申明 存储在Flash中和从Flash中读取后的存储的数组
extern int16_t  FrontGrayscale[24];              
extern int16_t  BackGrayscale[24];
extern int16_t  MidGrayscale[24];

//定义存储在Flash中和从Flash中读取后的存储的数组
extern int16_t  F_white_green[8];              //用于储存白色和绿色的平均值
extern int16_t  F_white_black[8];               //用于储存白色和灰色的平均值
extern int16_t  F_green_black[8];               //用于储存灰色和绿色的平均值

extern int16_t B_white_green[8];              //用于储存白色和绿色的平均值
extern int16_t B_white_black[8];               //用于储存白色和灰色的平均值
extern int16_t B_green_black[8];               //用于储存灰色和绿色的平均值

extern rt_mutex_t grayscale_mutex ;
extern uint8_t Color ;

enum Color_Type
{
	Front_WhiteGreen = 1,
	Front_WhiteBlack,
	Front_GreenBlack,
	Back_WhiteGreen,
	Back_WhiteBlack,
	Back_GreenBlack,
	Front_Back_WhiteGreen,
	Front_Back_WhiteBlack,
	Front_Back_GreenBlack
};

void grayscale_init(void); 
uint16_t  Get_Adc(uint8_t ch);
int16_t Get_Adc_Average(int8_t ch,int8_t times);


void Get_FGrayscale(int16_t* grayscale);//采集灰度值
void Get_BGrayscale(int16_t* grayscale);


void F_Compare(int16_t* aver);

void B_Compare(int16_t* aver);


int GetGrayscale(void);
int16_t pre_calculate(void);
int16_t Get_H_SETPOINT(void);


#endif 
