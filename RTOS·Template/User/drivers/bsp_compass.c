#include "bsp_compass.h"
#include "rt_oled.h"
#include "rt_led.h"

/*
	GY26ģ��˵��
	��ģ����ͨ���Ŵ������е������໥��ֱ��ͬʱ��Ӧ����ų��Ĵŷ������õ��ķ�λ�Ƕ�
	ע�⣺ҪԶ����Ŵų���Ʃ���������Լܸ�
	��ģ�����������Ϊ9600bps����JY61����Ĳ����ʲ�ͬ��JY26������Ϊ115200
	У׼:��ģ��9������(CAL)��һ��������(GND)����һ�ΰ��£�����У׼ģʽ,�������ڸ�������£�
			 ����ģ��ˮƽ,������תһ��(ʱ��Լһ����),�ٴΰ��°���LED��У׼������
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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART1ʱ��
	
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
	��ƫ�ǿ���:
		�ű������汱��֮��ļнǣ���ͼ���� ���������һ��Ĵ�����Ȧͬ��������
	Ȧ�ļнǡ� ��ͬ�ĵص��ƫ�ǲ�ͬ�� ͬһ�ص㲻ͬ��ʱ���ƫ��Ҳ��ͬ�� ģ��
	�Ĵ�ƫ��=ԭʼ�Ƕ�+�趨��ƫ�Ƕȣ� ��˳ʱ����ϡ� ���磺 ��ƫ��Ϊ 0 �� 100
	�ȣ� ��ʱ���Ĵ�ƫ��Ϊ 10 �ȣ� ��ôģ�����Ϊ 100+10=110 �ȡ�
*/
void show_angle(void)		//���յ����Ǵ�ƫ��
{
	USART_SendData(USART3,0X31);//ÿ�νǶȲ�����Ҫ����һ��0X31����
	rt_thread_delay(25);
	OLED_ShowNum(0,0,Angle,3,14);
}
