#include "sys.h"
#include "usart3.h"
#include "delay.h"

unsigned char data;
unsigned char ucStctime1[8],ucStcAcc1[8],stcGyro1[8],stcAngle1[8];
unsigned char stcMag1[8],stcDStatus1[8],stcPress1[8],stcLonLat1[8],stcGPSV1[8];



void usart4_init(u32 bound)
{
   //GPIO???������??
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //��1?��GPIOC����?��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//��1?��USART4����?��
 
	//��??��1??��|��y???�䨮?��3��?
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10?�䨮??aUART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11?�䨮??aUART4
	
	//UART4???��????
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10��?GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//?�䨮?1|?��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//?��?��50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��?����?�䨮?��?3?
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��?��-
	GPIO_Init(GPIOC,&GPIO_InitStructure); //3?��??��Pc10��?PC11

   //UART43?��??������??
	USART_InitStructure.USART_BaudRate = bound;//2����??������??
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��?3��?a8??��y?Y??��?
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//��???����?1??
	USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D��?��??
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��2?t��y?Y����????
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//��?����?�꨺?
  USART_Init(UART4, &USART_InitStructure); //3?��??����??��1
	
  USART_Cmd(UART4, ENABLE);  //��1?����??��4 
	
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//?a???��1??D??

	//Usart4 NVIC ????
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//��??��1?D??�����̨�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//?��??��??��??3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//������??��??3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ�����̨���1?��
	NVIC_Init(&NVIC_InitStructure);	//?��?Y???����?2?��y3?��??��VIC??��??��?��


	
}


u8 ucRxBuffer[10];
u8	ucRxCnt1=0;
void UART4_IRQHandler(void)                	//��??��4?D??��t??3��D��
{
	unsigned char data;
	if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)
	{
	data = USART_ReceiveData(UART4);

		if(ucRxCnt1<8)
		{
			ucRxBuffer[ucRxCnt1++]=data;
			if(ucRxBuffer[0]!=0x55)
			{
				ucRxCnt1 =0;
			}
		}
		CopeSerial2Data1();
	}
}	


void CopeSerial2Data1(void)
{
	if(ucRxCnt1>=8)
	{
		switch(ucRxBuffer[1])
		{
			case 0x50:
				ucStctime1[0]=ucRxBuffer[2];
				ucStctime1[1]=ucRxBuffer[3];
				ucStctime1[2]=ucRxBuffer[4];
				ucStctime1[3]=ucRxBuffer[5];
				ucStctime1[4]=ucRxBuffer[6];
				ucStctime1[5]=ucRxBuffer[7];
				ucStctime1[6]=ucRxBuffer[8];
				ucStctime1[7]=ucRxBuffer[9];
				break;
			case 0x51:
				ucStcAcc1[0]=ucRxBuffer[2];
				ucStcAcc1[1]=ucRxBuffer[3];
				ucStcAcc1[2]=ucRxBuffer[4];
				ucStcAcc1[3]=ucRxBuffer[5];
				ucStcAcc1[4]=ucRxBuffer[6];
				ucStcAcc1[5]=ucRxBuffer[7];
				ucStcAcc1[6]=ucRxBuffer[8];
				ucStcAcc1[7]=ucRxBuffer[9];
				break;
			case 0x52:
				stcGyro1[0]=ucRxBuffer[2];
				stcGyro1[1]=ucRxBuffer[3];
				stcGyro1[2]=ucRxBuffer[4];
				stcGyro1[3]=ucRxBuffer[5];
				stcGyro1[4]=ucRxBuffer[6];
				stcGyro1[5]=ucRxBuffer[7];
				stcGyro1[6]=ucRxBuffer[8];
				stcGyro1[7]=ucRxBuffer[9];
				break;
			case 0x53:
				stcAngle1[0]=ucRxBuffer[2];
				stcAngle1[1]=ucRxBuffer[3];
				stcAngle1[2]=ucRxBuffer[4];
				stcAngle1[3]=ucRxBuffer[5];
				stcAngle1[4]=ucRxBuffer[6];
				stcAngle1[5]=ucRxBuffer[7];
				stcAngle1[6]=ucRxBuffer[8];
				stcAngle1[7]=ucRxBuffer[9];
				break;
			case 0x54:
				stcMag1[0]=ucRxBuffer[2];
				stcMag1[1]=ucRxBuffer[3];
				stcMag1[2]=ucRxBuffer[4];
				stcMag1[3]=ucRxBuffer[5];
				stcMag1[4]=ucRxBuffer[6];
				stcMag1[5]=ucRxBuffer[7];
				stcMag1[6]=ucRxBuffer[8];
				stcMag1[7]=ucRxBuffer[9];
				break;
			case 0x55:
				stcDStatus1[0]=ucRxBuffer[2];
				stcDStatus1[1]=ucRxBuffer[3];
				stcDStatus1[2]=ucRxBuffer[4];
				stcDStatus1[3]=ucRxBuffer[5];
				stcDStatus1[4]=ucRxBuffer[6];
				stcDStatus1[5]=ucRxBuffer[7];
				stcDStatus1[6]=ucRxBuffer[8];
				stcDStatus1[7]=ucRxBuffer[9];
				break;
			case 0x56:
				stcPress1[0]=ucRxBuffer[2];
				stcPress1[1]=ucRxBuffer[3];
				stcPress1[2]=ucRxBuffer[4];
				stcPress1[3]=ucRxBuffer[5];
				stcPress1[4]=ucRxBuffer[6];
				stcPress1[5]=ucRxBuffer[7];
				stcPress1[6]=ucRxBuffer[8];
				stcPress1[7]=ucRxBuffer[9];
				break;
			case 0x57:
				stcLonLat1[0]=ucRxBuffer[2];
				stcLonLat1[1]=ucRxBuffer[3];
				stcLonLat1[2]=ucRxBuffer[4];
				stcLonLat1[3]=ucRxBuffer[5];
				stcLonLat1[4]=ucRxBuffer[6];
				stcLonLat1[5]=ucRxBuffer[7];
				stcLonLat1[6]=ucRxBuffer[8];
				stcLonLat1[7]=ucRxBuffer[9];
				break;
			case 0x58:
				stcGPSV1[0]=ucRxBuffer[2];
				stcGPSV1[1]=ucRxBuffer[3];
				stcGPSV1[2]=ucRxBuffer[4];
				stcGPSV1[3]=ucRxBuffer[5];
				stcGPSV1[4]=ucRxBuffer[6];
				stcGPSV1[5]=ucRxBuffer[7];
				stcGPSV1[6]=ucRxBuffer[8];
				stcGPSV1[7]=ucRxBuffer[9];
				break;
		}
		ucRxCnt1=0;
	}
}



