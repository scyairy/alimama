#include "JY61.h"
#include "bsp_oled.h"
#include "bsp_led.h"
#include "delay.h"

static unsigned char ucRxCnt = 0;	
static unsigned char ucRxBuffer[250];
unsigned char data;
unsigned char ucStctime[8],ucStcAcc[8],stcGyro[8],stcAngle[8];
unsigned char stcMag[8],stcDStatus[8],stcPress[8],stcLonLat[8],stcGPSV[8];

void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART1时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);    
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART3,USART_FLAG_TC);	
	USART_Cmd(USART3, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStructure);
}

void USART3_IRQHandler(void)
{ 
	
	unsigned char data;
	rt_interrupt_enter();
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
	data = USART_ReceiveData(USART3);
//		data =	(unsigned char) USART3->DR;
		if(ucRxCnt<11)
		{
			ucRxBuffer[ucRxCnt++]=data;
			if(ucRxBuffer[0]!=0x55)
			{
				ucRxCnt =0;
//				return;
			}
		}
		CopeSerial2Data();
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
//	USART_ClearITPendingBit(USART3,USART_IT_ORE);
	rt_interrupt_leave();
}
void CopeSerial2Data(void)
{
//	ucRxBuffer[ucRxCnt++]=ucDATA;
//	if(ucRxBuffer[0] != 0x55)
//	{
//		ucRxCnt=0;
//		return;
//	}
//	if (ucRxCnt<11) {return;} 
	if(ucRxCnt>=11)
	{
		switch(ucRxBuffer[1])
		{
			case 0x50:
				ucStctime[0]=ucRxBuffer[2];
				ucStctime[1]=ucRxBuffer[3];
				ucStctime[2]=ucRxBuffer[4];
				ucStctime[3]=ucRxBuffer[5];
				ucStctime[4]=ucRxBuffer[6];
				ucStctime[5]=ucRxBuffer[7];
				ucStctime[6]=ucRxBuffer[8];
				ucStctime[7]=ucRxBuffer[9];
				break;
			case 0x51:
				ucStcAcc[0]=ucRxBuffer[2];
				ucStcAcc[1]=ucRxBuffer[3];
				ucStcAcc[2]=ucRxBuffer[4];
				ucStcAcc[3]=ucRxBuffer[5];
				ucStcAcc[4]=ucRxBuffer[6];
				ucStcAcc[5]=ucRxBuffer[7];
				ucStcAcc[6]=ucRxBuffer[8];
				ucStcAcc[7]=ucRxBuffer[9];
				break;
			case 0x52:
				stcGyro[0]=ucRxBuffer[2];
				stcGyro[1]=ucRxBuffer[3];
				stcGyro[2]=ucRxBuffer[4];
				stcGyro[3]=ucRxBuffer[5];
				stcGyro[4]=ucRxBuffer[6];
				stcGyro[5]=ucRxBuffer[7];
				stcGyro[6]=ucRxBuffer[8];
				stcGyro[7]=ucRxBuffer[9];
				break;
			case 0x53:
				stcAngle[0]=ucRxBuffer[2];
				stcAngle[1]=ucRxBuffer[3];
				stcAngle[2]=ucRxBuffer[4];
				stcAngle[3]=ucRxBuffer[5];
				stcAngle[4]=ucRxBuffer[6];
				stcAngle[5]=ucRxBuffer[7];
				stcAngle[6]=ucRxBuffer[8];
				stcAngle[7]=ucRxBuffer[9];
				break;
			case 0x54:
				stcMag[0]=ucRxBuffer[2];
				stcMag[1]=ucRxBuffer[3];
				stcMag[2]=ucRxBuffer[4];
				stcMag[3]=ucRxBuffer[5];
				stcMag[4]=ucRxBuffer[6];
				stcMag[5]=ucRxBuffer[7];
				stcMag[6]=ucRxBuffer[8];
				stcMag[7]=ucRxBuffer[9];
				break;
			case 0x55:
				stcDStatus[0]=ucRxBuffer[2];
				stcDStatus[1]=ucRxBuffer[3];
				stcDStatus[2]=ucRxBuffer[4];
				stcDStatus[3]=ucRxBuffer[5];
				stcDStatus[4]=ucRxBuffer[6];
				stcDStatus[5]=ucRxBuffer[7];
				stcDStatus[6]=ucRxBuffer[8];
				stcDStatus[7]=ucRxBuffer[9];
				break;
			case 0x56:
				stcPress[0]=ucRxBuffer[2];
				stcPress[1]=ucRxBuffer[3];
				stcPress[2]=ucRxBuffer[4];
				stcPress[3]=ucRxBuffer[5];
				stcPress[4]=ucRxBuffer[6];
				stcPress[5]=ucRxBuffer[7];
				stcPress[6]=ucRxBuffer[8];
				stcPress[7]=ucRxBuffer[9];
				break;
			case 0x57:
				stcLonLat[0]=ucRxBuffer[2];
				stcLonLat[1]=ucRxBuffer[3];
				stcLonLat[2]=ucRxBuffer[4];
				stcLonLat[3]=ucRxBuffer[5];
				stcLonLat[4]=ucRxBuffer[6];
				stcLonLat[5]=ucRxBuffer[7];
				stcLonLat[6]=ucRxBuffer[8];
				stcLonLat[7]=ucRxBuffer[9];
				break;
			case 0x58:
				stcGPSV[0]=ucRxBuffer[2];
				stcGPSV[1]=ucRxBuffer[3];
				stcGPSV[2]=ucRxBuffer[4];
				stcGPSV[3]=ucRxBuffer[5];
				stcGPSV[4]=ucRxBuffer[6];
				stcGPSV[5]=ucRxBuffer[7];
				stcGPSV[6]=ucRxBuffer[8];
				stcGPSV[7]=ucRxBuffer[9];
				break;
		}
		ucRxCnt=0;
	}
}

void show_angle(void)				//显示值的定义可以看define
{
	OLED_ShowNum(0,0,Angle_x,3,14);
	rt_thread_delay(10);
	OLED_ShowNum(0,1,Angle_y,3,14);
	rt_thread_delay(10);
	OLED_ShowNum(0,2,Angle_z,3,14);
	rt_thread_delay(10);
}

