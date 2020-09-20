#include "bsp_image.h"
#include "bsp_oled.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "operation.h"

uint16_t image_data[3];
uint16_t image[3];

void Image_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART1时钟
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);    
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART2,USART_FLAG_TC);	
	USART_Cmd(USART2, ENABLE);
}

uint16_t Send_Receive(uint16_t data)
{
	uint16_t i,Res;
	USART_SendData(USART2,data);
		for(i=0;i<3;)
		{
			Res=USART_ReceiveData(USART2);
			if(Res!='\0')
			{
				image_data[i]=Res;
				i++;
			}
		}
	return image_data[0];
}


uint16_t Second_Plat_Image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=Send_Receive(97);
	if(USART_RX_BUF==48)
	{
		image[0]=4;						//设置第一个宝藏
	}
	else if(USART_RX_BUF==49)
	{
		image[0]=3;						//设置第一个宝藏
	}
	OLED_Clear();
	OLED_ShowNum(0,0,image[0],4,12);
	
	return image[0];
}
uint16_t Third_Plat_Image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=Send_Receive(98);
	if(USART_RX_BUF==48)
	{
		image[1]=5;						//设置第二个宝藏
	}
	else if(USART_RX_BUF==49)
	{
		image[1]=6;						//设置第二个宝藏
	}
	OLED_Clear();
	OLED_ShowNum(0,0,image[1],4,12);
	
	return image[1];
}
uint16_t Fourth_Plat_Image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=Send_Receive(99);
	if(USART_RX_BUF==48)
	{
		image[1]=5;						//设置第二个宝藏
	}
	else if(USART_RX_BUF==49)
	{
		image[1]=6;						//设置第二个宝藏
	}
	OLED_Clear();
	OLED_ShowNum(0,0,image[1],4,12);
	
	return image[1];
}
uint16_t Fifth_Plat_Image(void)
{
	uint16_t USART_RX_BUF;
	USART_RX_BUF=Send_Receive(100);
	if(USART_RX_BUF==48)
	{
		image[2]=7;						//设置第三个宝藏
	}
	else if(USART_RX_BUF==49)
	{
		image[2]=8;						//设置第三个宝藏
	}
	OLED_Clear();
	OLED_ShowNum(0,0,image[2],4,12);
	
	return image[2];
}
uint16_t Sixth_Plat_Image(void)
{
		uint16_t USART_RX_BUF;
		USART_RX_BUF=Send_Receive(101);
		if(USART_RX_BUF==48)
		{
			image[2]=7;						//设置第三个宝藏
		}
		else if(USART_RX_BUF==49)
		{
			image[2]=8;						//设置第三个宝藏
		}
		OLED_Clear();
		OLED_ShowNum(0,0,image[2],4,12);
		
		return image[2];
}

