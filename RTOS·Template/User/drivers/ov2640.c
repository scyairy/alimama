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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//OV2640 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
  
//初始化OV2640 
//配置完以后,默认输出是1600*1200尺寸的图片!! 
//返回值:0,成功
//    其他,错误代码

u8 ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式


#define jpeg_buf_size 2  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志


//提取R,G,B的参数
#define RGB_R 0XF800
#define RGB_G 0X07E0
#define RGB_B 0X001F
u16 R,G,B;

u32 sum_x,sum_y,sum_1,sum_2;

#define yuv_buf_size 220*280*2/4
//#define yuv_buf_size 2*200*2/4	//定义YUV数据缓存yuv_buf的大小(单位是字节总数除以4，因为数组为32位)(*4字节)
__align(4) u32 yuv_buf[yuv_buf_size];	//yuv数据缓存buf【__align(4)为四字节对齐】

/////////////////////////////////////////////
u8 OV2640_Init(void)
{ 
	u16 i=0;
	u16 reg;
	//设置IO     	
  	RCC->AHB1ENR|=1<<6;		//使能外设PORTG时钟    
 	GPIO_Set(GPIOG,PIN9|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PG9,15推挽输出
 	OV2640_PWDN=0;	//POWER ON
	delay_ms(10);
	OV2640_RST=0;	//复位OV2640
	delay_ms(10);
	OV2640_RST=1;	//结束复位 
  	SCCB_Init();        		//初始化SCCB 的IO口	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//操作sensor寄存器
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//软复位OV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//读取厂家ID 低八位
	if(reg!=OV2640_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//读取厂家ID 低八位
	if(reg!=OV2640_PID)
	{
		printf("HID:%d\r\n",reg);
		//return 2;
	}   
 	//初始化 OV2640,采用SXGA分辨率(1600*1200)  
	for(i=0;i<sizeof(ov2640_uxga_init_reg_tbl)/2;i++)
	{
	   	SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0],ov2640_uxga_init_reg_tbl[i][1]);
 	} 
  	return 0x00; 	//ok
} 
//OV2640切换为JPEG模式
void OV2640_JPEG_Mode(void) 
{
	u16 i=0;
	//设置:YUV422格式
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	//设置:输出JPEG数据
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}



//OV2640切换为YUV422模式
void OV2640_YUV422_Mode(void) 
{
	//设置:YUV422格式
	SCCB_WR_Reg(0xFF, 0x00);//设置:YUV422输出
	SCCB_WR_Reg(0xDA, 0x01);//YUV422.YVYU	
}





//OV2640切换为RGB565模式
void OV2640_RGB565_Mode(void) 
{
	u16 i=0;
	//设置:RGB565输出
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
} 
//自动曝光设置参数表,支持5个等级
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
//OV2640自动曝光等级设置
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
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
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
//色度设置
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
//亮度设置
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
//对比度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(u8 contrast)
{
	u8 reg7d0val=0X20;//默认为普通模式
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
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV2640_Special_Effects(u8 eft)
{
	u8 reg7d0val=0X00;//默认为普通模式
	u8 reg7d1val=0X80;
	u8 reg7d2val=0X80; 
	switch(eft)
	{
		case 1://负片
			reg7d0val=0X40; 
			break;	
		case 2://黑白
			reg7d0val=0X18; 
			break;	 
		case 3://偏红色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://偏绿色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://偏蓝色
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://复古
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
//彩条测试
//sw:0,关闭彩条
//   1,开启彩条(注意OV2640的彩条是叠加在图像上面的)
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//设置传感器输出窗口 
//sx,sy,起始地址
//width,height:宽度(对应:horizontal)和高度(对应:vertical)
void OV2640_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sy>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endy>>2);			//设置Vref的end的高8位
	
	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//设置Href的start和end的最低3位
	SCCB_WR_Reg(0X17,sx>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endx>>3);			//设置Href的end的高8位
}
//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由该函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
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
	SCCB_WR_Reg(0X5A,outw&0XFF);		//设置OUTW的低八位
	SCCB_WR_Reg(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//设置OUTH/OUTW的高位 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//设置图像开窗大小
//由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
//该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
//注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
//     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
//     自动计算缩放比例,输出给外部设备.
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
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
	SCCB_WR_Reg(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	SCCB_WR_Reg(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	SCCB_WR_Reg(0X53,offx&0XFF);		//设置offx的低八位
	SCCB_WR_Reg(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//该函数设置图像尺寸大小,也就是所选格式的输出分辨率
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:图像宽度和图像高度
//返回值:0,设置成功
//    其他,设置失败
u8 OV2640_ImageSize_Set(u16 width,u16 height)
{ 
	u8 temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

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


u16 Binary(u16 threshold,u16 pixel)
{
    static u16 Gray;
	  /*******提取R,G,B值*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
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

u16 Redball(u16 threshold,u16 pixel)
{
    static u16 Gray;
	  /*******提取R,G,B值*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	  Gray = (u16)((R*634+G*613+B*232));
	if(R>14&&R-0.5*G>6&&R-B>6)//如果，Y值大于等于阀值
	{
		 Gray=1;	//二值化像素为白
	}
	else
	{
		Gray =0;	//二值化像素为黑
	}
	
	return Gray;	//返回灰度图像值(RGB565格式显示)
}

u16 Redball_x_y(u16 pixel)
{
    u16 x_y;
	  /*******提取R,G,B值*******/
	  R = (pixel&RGB_R)>>11;
    G = (pixel&RGB_G)>>5;
    B = (pixel&RGB_B);
	if(R>14&&R-0.5*G>6&&R-B>6)//如果，Y值大于等于阀值
	{
		 x_y=1;	//二值化像素为白
	}
	else
	{
	   x_y=0;	//二值化像素为黑
	}
	
	return x_y;	//返回灰度图像值(RGB565格式显示)
}


////函数功能：YUV422格式转灰度图像(RGB565格式显示)
////yuv422：yuv格式数据
//u16 yuv422_to_Gray(u16 yuv422)
//{
//	u16 Gray;	//用于储存灰度值变量(RGB565格式显示)
////	u8 temp;	//用于储存yuv422格式数据中的亮度值Y量
//	
////	temp = (u8)yuv422>>8;//把yuv422格式数据中的亮度值Y量提取出来赋值给temp变量（Y值在高字节，根据OV2640寄存器设置决定）

//	Gray =(((yuv422>>(8+3))<<11)|((yuv422>>(8+2))<<5)|((yuv422>>(8+3))<<0));//Y量转为灰度值(RGB565格式显示)

//	
//	return Gray;	//返回灰度图像值(RGB565格式显示)
//} 

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

static void ov2640_entry(void *parameter)
{
	u8 key=0;
	u8 x,y,flag;
	u16 threshold=80;	//二值化时用到的阀值变量   threshold=128   TEST
		oled.show_string(0,0, "no ok:",16);
	while(OV2640_Init())//初始化OV2640
	{
		oled.show_string(0,0, "no ok..:",20);
		delay_ms(200);
	} 
//	LCD_ShowString(30,30,200,16,16,"OV2640 OK");
//	LCD_ShowString(30,50,240,16,16,"KEY_UP: fazhi++");	//按键上：阀值增加
//	LCD_ShowString(30,70,240,16,16,"  KEY1: fazhi--");	//按键1：阀值减少

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
	{	//RGB565测试
		//RGB数据直接显示在LCD上面

//		LCD_Clear(WHITE);
//		POINT_COLOR=RED; 	
		DCMI_Init1();			//DCMI配置
//		DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,1,0);//DCMI DMA配置
		////////////////////////////////////////////////////////////////////////
		//OV2640经过DCMI_DMA把图像数据传输到定义的数组yuv_buf中
		//(u32)&yuv_buf: 【存储器地址】取自定义的数组地址并转换为32位
		//yuv_buf_size： 【存储器长度】单位是字节总数除以4，因为数据为32位
		//2：            【存储器位宽】 0,8位,   1,16位,   2,32位
		//1：            【存储器增长方式】 0,不增长;    1,增长
		DCMI_DMA_Init((u32)&yuv_buf,yuv_buf_size,2,1);//DCMI DMA配置
		////////////////////////////////////////////////////////////////////////
		OV2640_OutSize_Set(220,280);//OV2640输出图像尺寸为：176X144
//		OV2640_OutSize_Set(1600,1200);//OV2640输出图像尺寸为：176X144
		////////////////////////////////////////////////////////////////////////	
		LED0_PWM_VAL=2475;
		while(1)
		{ 
			u16 i,temp_h,temp_l,temp,gray0,gray1;
			u16 Graymax=0;
			u16 Graymin=65535;
			delay_ms(10);

			OV2640_RGB565_Mode();
			DCMI_Start(); 		//启动传输
			DCMI_Stop(); //停止显示
////////////////////////////////////////////////////////////////////////
//			LCD_Set_Window(0,0,220,280);	//设置窗口       -----屏幕显示二值化图--------
//			LCD_WriteRAM_Prepare();	//开始写入GRAM
//			
			sum_x = 0;
			sum_1 = 0;
			for(i=0;i<220*280/2;i=i+2) 				//宽和高
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
















