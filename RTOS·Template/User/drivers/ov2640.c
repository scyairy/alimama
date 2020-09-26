#include "sys.h"
#include "ov2640.h"
#include "ov2640cfg.h"
#include "soft_timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	 
#include "timer.h"
#include "rt_led.h"
#include "ili93xx.h"
#include "dcmi.h"
#include "rt_oled.h"
#include "rtthread.h"

ALIGN(RT_ALIGN_SIZE)

static struct rt_thread fan_thread;
static struct rt_thread detach_thread;

static rt_uint8_t fan_stack[1024];
static rt_uint8_t detach_stack[1024];
static rt_thread_t tid1 = RT_NULL;

#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

#define RGB_R 0XF800
#define RGB_G 0X07E0
#define RGB_B 0X001F
u16 R,G,B;

u32 sum_x,sum_y,sum_1,sum_2;

#define yuv_buf_size 220*280*2/4
__align(4) u32 yuv_buf[yuv_buf_size];	


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//OV2640 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
  
//��ʼ��OV2640 
//�������Ժ�,Ĭ�������1600*1200�ߴ��ͼƬ!! 
//����ֵ:0,�ɹ�
//    ����,�������

u8 ov2640_mode=0;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ


#define jpeg_buf_size 2  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־


//��ȡR,G,B�Ĳ���
#define RGB_R 0XF800
#define RGB_G 0X07E0
#define RGB_B 0X001F
u16 R,G,B;

u32 sum_x,sum_y,sum_1,sum_2;

#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//����YUV���ݻ���yuv_buf�Ĵ�С(��λ���ֽ���������4����Ϊ����Ϊ32λ)(*4�ֽ�)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv���ݻ���buf��__align(4)Ϊ���ֽڶ��롿

/////////////////////////////////////////////
u8 OV2640_Init(void)
{ 
	u16 i=0;
	u16 reg;
	//����IO     	
  	RCC->AHB1ENR|=1<<6;		//ʹ������PORTGʱ��    
 	GPIO_Set(GPIOG,PIN9|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PG9,15�������
 	OV2640_PWDN=0;	//POWER ON
	delay_ms(10);
	OV2640_RST=0;	//��λOV2640
	delay_ms(10);
	OV2640_RST=1;	//������λ 
  	SCCB_Init();        		//��ʼ��SCCB ��IO��	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//����sensor�Ĵ���
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//��λOV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//��ȡ����ID �Ͱ�λ
	if(reg!=OV2640_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//��ȡ����ID �Ͱ�λ
	if(reg!=OV2640_PID)
	{
		printf("HID:%d\r\n",reg);
		//return 2;
	}   
 	//��ʼ�� OV2640,����SXGA�ֱ���(1600*1200)  
	for(i=0;i<sizeof(ov2640_uxga_init_reg_tbl)/2;i++)
	{
	   	SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0],ov2640_uxga_init_reg_tbl[i][1]);
 	} 
  	return 0x00; 	//ok
} 
//OV2640�л�ΪJPEGģʽ
void OV2640_JPEG_Mode(void) 
{
	u16 i=0;
	//����:YUV422��ʽ
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	//����:���JPEG����
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}



//OV2640�л�ΪYUV422ģʽ
void OV2640_YUV422_Mode(void) 
{
	//����:YUV422��ʽ
	SCCB_WR_Reg(0xFF, 0x00);//����:YUV422���
	SCCB_WR_Reg(0xDA, 0x01);//YUV422.YVYU	
}





//OV2640�л�ΪRGB565ģʽ
void OV2640_RGB565_Mode(void) 
{
	u16 i=0;
	//����:RGB565���
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
} 
//�Զ��ع����ò�����,֧��5���ȼ�
const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,	
		0x24,0x3e,	
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,	
		0x24,0x58,	
		0x25,0x50,	
		0x26,0x92,	
	},
}; 
//OV2640�Զ��ع�ȼ�����
//level:0~4
void OV2640_Auto_Exposure(u8 level)
{  
	u8 i;
	u8 *p=(u8*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		SCCB_WR_Reg(p[i*2],p[i*2+1]); 
	} 
}  
//��ƽ������
//0:�Զ�
//1:̫��sunny
//2,����cloudy
//3,�칫��office
//4,����home
void OV2640_Light_Mode(u8 mode)
{
	u8 regccval=0X5E;//Sunny 
	u8 regcdval=0X41;
	u8 regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			SCCB_WR_Reg(0XFF,0X00);	 
			SCCB_WR_Reg(0XC7,0X00);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	SCCB_WR_Reg(0XFF,0X00);	 
	SCCB_WR_Reg(0XC7,0X40);	//AWB OFF 
	SCCB_WR_Reg(0XCC,regccval); 
	SCCB_WR_Reg(0XCD,regcdval); 
	SCCB_WR_Reg(0XCE,regceval);  
}
//ɫ������
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(u8 sat)
{ 
	u8 reg7dval=((sat+2)<<4)|0X08;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0X7C,0X00);		
	SCCB_WR_Reg(0X7D,0X02);				
	SCCB_WR_Reg(0X7C,0X03);			
	SCCB_WR_Reg(0X7D,reg7dval);			
	SCCB_WR_Reg(0X7D,reg7dval); 		
}
//��������
//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(u8 bright)
{
  SCCB_WR_Reg(0xff, 0x00);
  SCCB_WR_Reg(0x7c, 0x00);
  SCCB_WR_Reg(0x7d, 0x04);
  SCCB_WR_Reg(0x7c, 0x09);
  SCCB_WR_Reg(0x7d, bright<<4); 
  SCCB_WR_Reg(0x7d, 0x00); 
}
//�Աȶ�����
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(u8 contrast)
{
	u8 reg7d0val=0X20;//Ĭ��Ϊ��ͨģʽ
	u8 reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,0x04);
	SCCB_WR_Reg(0x7c,0x07);
	SCCB_WR_Reg(0x7d,0x20);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,0x06);
}
//��Ч����
//0:��ͨģʽ    
//1,��Ƭ
//2,�ڰ�   
//3,ƫ��ɫ
//4,ƫ��ɫ
//5,ƫ��ɫ
//6,����	    
void OV2640_Special_Effects(u8 eft)
{
	u8 reg7d0val=0X00;//Ĭ��Ϊ��ͨģʽ
	u8 reg7d1val=0X80;
	u8 reg7d2val=0X80; 
	switch(eft)
	{
		case 1://��Ƭ
			reg7d0val=0X40; 
			break;	
		case 2://�ڰ�
			reg7d0val=0X18; 
			break;	 
		case 3://ƫ��ɫ
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://ƫ��ɫ
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://ƫ��ɫ
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://����
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7c,0x05);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,reg7d2val); 
}
//��������
//sw:0,�رղ���
//   1,��������(ע��OV2640�Ĳ����ǵ�����ͼ�������)
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//���ô������������ 
//sx,sy,��ʼ��ַ
//width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical)
void OV2640_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//��ȡVref֮ǰ��ֵ
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//����Vref��start��end�����2λ
	SCCB_WR_Reg(0X19,sy>>2);			//����Vref��start��8λ
	SCCB_WR_Reg(0X1A,endy>>2);			//����Vref��end�ĸ�8λ
	
	temp=SCCB_RD_Reg(0X32);				//��ȡHref֮ǰ��ֵ
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//����Href��start��end�����3λ
	SCCB_WR_Reg(0X17,sx>>3);			//����Href��start��8λ
	SCCB_WR_Reg(0X18,endx>>3);			//����Href��end�ĸ�8λ
}
//����ͼ�������С
//OV2640���ͼ��Ĵ�С(�ֱ���),��ȫ�ɸú���ȷ��
//width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 OV2640_OutSize_Set(u16 width,u16 height)
{
	u16 outh;
	u16 outw;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0X5A,outw&0XFF);		//����OUTW�ĵͰ�λ
	SCCB_WR_Reg(0X5B,outh&0XFF);		//����OUTH�ĵͰ�λ
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//����OUTH/OUTW�ĸ�λ 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//����ͼ�񿪴���С
//��:OV2640_ImageSize_Setȷ������������ֱ��ʴӴ�С.
//�ú������������Χ������п���,����OV2640_OutSize_Set�����
//ע��:�������Ŀ�Ⱥ͸߶�,������ڵ���OV2640_OutSize_Set�����Ŀ�Ⱥ͸߶�
//     OV2640_OutSize_Set���õĿ�Ⱥ͸߶�,���ݱ��������õĿ�Ⱥ͸߶�,��DSP
//     �Զ��������ű���,������ⲿ�豸.
//width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 OV2640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height)
{
	u16 hsize;
	u16 vsize;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);					
	SCCB_WR_Reg(0X51,hsize&0XFF);		//����H_SIZE�ĵͰ�λ
	SCCB_WR_Reg(0X52,vsize&0XFF);		//����V_SIZE�ĵͰ�λ
	SCCB_WR_Reg(0X53,offx&0XFF);		//����offx�ĵͰ�λ
	SCCB_WR_Reg(0X54,offy&0XFF);		//����offy�ĵͰ�λ
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//�ú�������ͼ��ߴ��С,Ҳ������ѡ��ʽ������ֱ���
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:ͼ���Ⱥ�ͼ��߶�
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 OV2640_ImageSize_Set(u16 width,u16 height)
{ 
	u8 temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//����HSIZE��10:3λ
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//����VSIZE��10:3λ
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

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


u16 Binary(u16 threshold,u16 pixel)
{
    static u16 Gray;
	  /*******��ȡR,G,Bֵ*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
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

u16 Redball(u16 threshold,u16 pixel)
{
    static u16 Gray;
	  /*******��ȡR,G,Bֵ*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
	if(R>14&&R-0.5*G>6&&R-B>6)//�����Yֵ���ڵ��ڷ�ֵ
	{
		 Gray=1;	//��ֵ������Ϊ��
	}
	else
	{
		Gray =0;	//��ֵ������Ϊ��
	}
	
	return Gray;	//���ػҶ�ͼ��ֵ(RGB565��ʽ��ʾ)
}

u16 Redball_x_y(u16 pixel)
{
    u16 x_y;
	  /*******��ȡR,G,Bֵ*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	if(R>14&&R-0.5*G>6&&R-B>6)//�����Yֵ���ڵ��ڷ�ֵ
	{
		 x_y=1;	//��ֵ������Ϊ��
	}
	else
	{
	   x_y=0;	//��ֵ������Ϊ��
	}
	
	return x_y;	//���ػҶ�ͼ��ֵ(RGB565��ʽ��ʾ)
}


////�������ܣ�YUV422��ʽת�Ҷ�ͼ��(RGB565��ʽ��ʾ)
////yuv422��yuv��ʽ����
//u16 yuv422_to_Gray(u16 yuv422)
//{
//	u16 Gray;	//���ڴ���Ҷ�ֵ����(RGB565��ʽ��ʾ)
////	u8 temp;	//���ڴ���yuv422��ʽ�����е�����ֵY��
//	
////	temp = (u8)yuv422>>8;//��yuv422��ʽ�����е�����ֵY����ȡ������ֵ��temp������Yֵ�ڸ��ֽڣ�����OV2640�Ĵ������þ�����

//	Gray =(((yuv422>>(8+3))<<11)|((yuv422>>(8+2))<<5)|((yuv422>>(8+3))<<0));//Y��תΪ�Ҷ�ֵ(RGB565��ʽ��ʾ)

//	
//	return Gray;	//���ػҶ�ͼ��ֵ(RGB565��ʽ��ʾ)
//} 

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

static void ov2640_entry(void *parameter)
{
	u8 key=0;
	u8 x,y,flag;
	u16 threshold=80;	//��ֵ��ʱ�õ��ķ�ֵ����   threshold=128   TEST
		oled.show_string(0,0, "no ok:",16);
	while(OV2640_Init())//��ʼ��OV2640
	{
		oled.show_string(0,0, "no ok..:",20);
		delay_ms(200);
	} 
//	LCD_ShowString(30,30,200,16,16,"OV2640 OK");
//	LCD_ShowString(30,50,240,16,16,"KEY_UP: fazhi++");	//�����ϣ���ֵ����
//	LCD_ShowString(30,70,240,16,16,"  KEY1: fazhi--");	//����1����ֵ����

	delay_ms(5000);
	
	
//	while(1)
//	{
////		LED0_PWM_VAL11=300;	
////		delay_ms(2000);	 
//		OLED_DrawPoint(15,60,1);
//		
//		OLED_DrawPoint(100,60,1);
////		LED0_PWM_VAL=2475;	
////		delay_ms(2000);	 
//	}
 	while(1)
	{	//RGB565����
		//RGB����ֱ����ʾ��LCD����

//		LCD_Clear(WHITE);
//		POINT_COLOR=RED; 	
		DCMI_Init1();			//DCMI����
//		DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,1,0);//DCMI DMA����
		////////////////////////////////////////////////////////////////////////
		//OV2640����DCMI_DMA��ͼ�����ݴ��䵽���������yuv_buf��
		//(u32)&yuv_buf: ���洢����ַ��ȡ�Զ���������ַ��ת��Ϊ32λ
		//yuv_buf_size�� ���洢�����ȡ���λ���ֽ���������4����Ϊ����Ϊ32λ
		//2��            ���洢��λ�� 0,8λ,   1,16λ,   2,32λ
		//1��            ���洢��������ʽ�� 0,������;    1,����
		DCMI_DMA_Init((u32)&yuv_buf,yuv_buf_size,2,1);//DCMI DMA����
		////////////////////////////////////////////////////////////////////////
		OV2640_OutSize_Set(220,280);//OV2640���ͼ��ߴ�Ϊ��176X144
//		OV2640_OutSize_Set(1600,1200);//OV2640���ͼ��ߴ�Ϊ��176X144
		////////////////////////////////////////////////////////////////////////	
		LED0_PWM_VAL=2475;
		while(1)
		{ 
			u16 i,temp_h,temp_l,temp,gray0,gray1;
			u16 Graymax=0;
			u16 Graymin=65535;
			delay_ms(10);

			OV2640_RGB565_Mode();
			DCMI_Start(); 		//��������
			DCMI_Stop(); //ֹͣ��ʾ
////////////////////////////////////////////////////////////////////////
//			LCD_Set_Window(0,0,220,280);	//���ô���       -----��Ļ��ʾ��ֵ��ͼ--------
//			LCD_WriteRAM_Prepare();	//��ʼд��GRAM
//			
			sum_x = 0;
			sum_1 = 0;
			for(i=0;i<220*280/2;i=i+2) 				//��͸�
			{				
				temp_l=(u16)(yuv_buf[i]);
//				LCD->LCD_RAM = Redball(threshold,temp_l);
				OLED_DrawPoint((i%220)/2,i/220/2,Redball(threshold,temp_l));
				
				if(Redball(threshold,temp_l) == 0xffff)
				{
					sum_x += (i%220);
					sum_1++;
				}

//				temp_h=(u16)(yuv_buf[i]>>16);
////				LCD->LCD_RAM = Redball(threshold,temp_h);
//				OLED_DrawPoint((i%220)/2,i/220/2,Redball(threshold,temp_h));
				
				if(Redball(threshold,temp_h) == 0xffff)
				{
					sum_x += (i%220);
					sum_1++;
				}
				
			}
			
//			LCD_ShowString(10,200,48,16,16,"weizhi:");
//			LCD_ShowNum(59,200,(u32)sum_x/sum_1,6,16);	
//			if((sum_x/sum_1)>120)
//			{
//				LED0_PWM_VAL=LED0_PWM_VAL+1;	
//				delay_ms(300);	 
//			}
//			else if((sum_x/sum_1)<90)
//			{
//				LED0_PWM_VAL=LED0_PWM_VAL-1;	
//				delay_ms(300);
//			}
//			else
//			{
//				LED0_PWM_VAL=2475;
//			}
//			
				
//			LCD_ShowNum(80,240,(u32)sum_x,8,16);
//			LCD_ShowNum(80,160,(u32)sum_1,6,16);
			
		}  
//		delay_ms(5);	  
	}

}


int ov2640(void)
{
	tid1=rt_thread_create("ov2640",
												ov2640_entry,RT_NULL,
												THREAD_STACK_SIZE,
												16,THREAD_TIMESLICE);
	if(tid1!=RT_NULL)
		rt_thread_startup(tid1);
	
	return 0;
}
















