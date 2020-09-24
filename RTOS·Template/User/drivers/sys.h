#ifndef __SYS_H
#define __SYS_H	 
#include "stm32f4xx.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//系统时钟初始化	
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 
//#define USE_STDPERIPH_DRIVER
#include "misc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SYSTEM_SUPPORT_OS		0		//?¨??????????????·??§??OS
 
//??????×÷,????51?à????GPIO????????
//????????????,????<<CM3?¨??????>>??????(87??~92??).M4??M3?à??,?????????÷???·±???.
//IO????×÷?ê?¨??
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO?????·????
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO????×÷,??????????IO??!
//?·±?n????????16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //???? 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //???? 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //???? 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //???? 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //???? 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //???? 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //???? 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //???? 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //???? 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //????

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //???? 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //????

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //???? 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //????

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //???? 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //????

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //???? 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //????
////////////////////////////////////////////////////////////////////////////////// 
//Ex_NVIC_Config×¨???¨??
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 

#define FTIR   				1  		//????????·?
#define RTIR   				2  		//????????·?

//GPIO?è??×¨???ê?¨??
#define GPIO_MODE_IN    	0		//???¨????????
#define GPIO_MODE_OUT		1		//???¨????????
#define GPIO_MODE_AF		2		//AF????????
#define GPIO_MODE_AIN		3		//????????????

#define GPIO_SPEED_2M		0		//GPIO????2Mhz
#define GPIO_SPEED_25M		1		//GPIO????25Mhz
#define GPIO_SPEED_50M		2		//GPIO????50Mhz
#define GPIO_SPEED_100M		3		//GPIO????100Mhz

#define GPIO_PUPD_NONE		0		//??????????
#define GPIO_PUPD_PU		1		//????
#define GPIO_PUPD_PD		2		//????
#define GPIO_PUPD_RES		3		//±??? 

#define GPIO_OTYPE_PP		0		//???ì????
#define GPIO_OTYPE_OD		1		//???????? 

//GPIO????±à???¨??
#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 
////////////////////////////////////////////////////////////////////////////////// 
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq);		//?????±???è??
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq); //?±????????  
void Sys_Soft_Reset(void);      							//?????í????
void Sys_Standby(void);         							//???ú???? 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);	//?è?????????·
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//?è??NVIC·?×é
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//?è??????
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//????????????????(????GPIOA~I)
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO?????????è??
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO?è??????  
//????????±à????
void WFI_SET(void);		//????WFI????
void INTX_DISABLE(void);//??±??ù??????
void INTX_ENABLE(void);	//?????ù??????
void MSR_MSP(u32 addr);	//?è?????????·
////////////////////////////////////////////////////////////////////////////////////////////////
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif











