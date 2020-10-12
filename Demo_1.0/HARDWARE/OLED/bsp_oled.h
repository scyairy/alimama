#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#include "sys.h"
#include "stdlib.h"


#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	   
//-----------------OLED IIC端口定义----------------  					   

//IO方向设置
//#define SDA_IN()  {GPIOD->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	
//#define SDA_OUT() {GPIOD->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 
//IO操作函数
#define IIC_SCL    PDout(9) //SCL
#define IIC_SDA    PDout(8) //SDA	 
#define READ_SDA   PDin(8)  //输入SDA 

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

struct OLED_DEVICE
{
    void    (*init)         (void);
    void    (*clear)        (void);
    void    (*show_number)  (u8 x,u8 y,u32 num,u8 len,u8 size);
    void    (*show_char)     (u8 x,u8 y,u8 chr,u8 Char_Size);
    void    (*show_string)   (u8 x,u8 y, u8 *p,u8 Char_Size);
    void    (*show_chinese)  (u8 x,u8 y,u8 no);
};

extern struct OLED_DEVICE oled;

void oled_all(void);

void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

void oled_show(void);

#endif

