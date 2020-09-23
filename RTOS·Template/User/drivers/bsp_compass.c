#include "bsp_compass.h"
#include "rt_oled.h"
#include "rt_led.h"

/*
	GY26模块说明
	该模块是通过磁传感器中的两个相互垂直轴同时感应地球磁场的磁分量而得到的方位角度
	注意：要远离干扰磁场，譬如电机，可以架高
	该模块输出波特率为9600bps，与JY61输出的波特率不同，JY26波特率为115200
	校准:该模块9号引脚(CAL)接一按键至地(GND)，第一次按下，进入校准模式,在无外在干扰情况下，
			 保持模块水平,缓慢旋转一周(时长约一分钟),再次按下按键LED灭，校准结束。
*/

static unsigned char ucRxBuffer[250];
static unsigned char ucRxCnt = 0;	
uint16_t Angle;
unsigned char data;

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
	
	USART_InitStructure.USART_BaudRate=9600;
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=7;
	NVIC_Init(&NVIC_InitStructure);
}

void USART3_IRQHandler(void)
{ 
	rt_interrupt_enter();
	if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET)
	{
		data =	(unsigned char) USART3->DR;
		if(ucRxCnt<8)
		{
			ucRxBuffer[ucRxCnt++]=data;
			if(ucRxBuffer[0]!=0x0D)
			{
				ucRxCnt=0;
			}
		}
		else
		{
			if(ucRxBuffer[1]==0X0A)
			{
				if(ucRxBuffer[7]==(u8)(0x0D+0X0A+ucRxBuffer[2]+ucRxBuffer[3]+ucRxBuffer[4]+0x2E+ucRxBuffer[6]) )
				{
					LED0=!LED0;
					Angle=(ucRxBuffer[2]&0x0F)*100+(ucRxBuffer[3]&0x0F)*10+(ucRxBuffer[4]&0x0F);
					ucRxCnt =0;
				}
				else
				{
					ucRxCnt =0;
				}
			}
			else
			{
				ucRxCnt =0;
			}
		}
	}
	rt_interrupt_leave();
}


/*
	磁偏角科普:
		磁北线与真北线之间的夹角（如图）， 地球表面任一点的磁子午圈同地理子午
	圈的夹角。 不同的地点磁偏角不同， 同一地点不同的时间磁偏角也不同。 模块
	的磁偏角=原始角度+设定磁偏角度， 即顺时针加上。 例如： 磁偏角为 0 的 100
	度， 此时更改磁偏角为 10 度， 那么模块输出为 100+10=110 度。
*/
void show_angle(void)		//接收到的是磁偏角
{
	USART_SendData(USART3,0X31);//每次角度测量都要发送一个0X31命令
	rt_thread_delay(25);
	OLED_ShowNum(0,0,Angle,3,14);
}
