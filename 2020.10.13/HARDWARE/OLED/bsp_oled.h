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
//-----------------OLED IIC�˿ڶ���----------------  					   

//IO��������
//#define SDA_IN()  {GPIOD->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	
//#define SDA_OUT() {GPIOD->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 
//IO��������
#define IIC_SCL    PDout(9) //SCL
#define IIC_SDA    PDout(8) //SDA	 
#define READ_SDA   PDin(8)  //����SDA 

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

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

void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

void oled_show(void);

#endif

