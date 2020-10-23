#include "sys.h"
#include "usart4.h"
#include "delay.h"

u16 USART4_RX_STA=0; 
u8 num=0;
u8  USART4_RX_BUF[USART4_REC_LEN];

void usart3_init(u32 bound)
{  

   //GPIO???������??
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //��1?��GPIOC����?��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//��1?��USART4����?��
 
	//��??��1??��|��y???�䨮?��3��?
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOC10?�䨮??aUART4
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOC11?�䨮??aUART4
	
	//UART4???��????
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10��?GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//?�䨮?1|?��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//?��?��50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��?����?�䨮?��?3?
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��?��-
	GPIO_Init(GPIOB,&GPIO_InitStructure); //3?��??��Pc10��?PC11

   //UART43?��??������??
	USART_InitStructure.USART_BaudRate = bound;//2����??������??
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��?3��?a8??��y?Y??��?
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//��???����?1??
	USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D��?��??
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��2?t��y?Y����????
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//��?����?�꨺?
  USART_Init(USART3, &USART_InitStructure); //3?��??����??��1
	
  USART_Cmd(USART3, ENABLE);  //��1?����??��4 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//?a???��1??D??

	//Usart4 NVIC ????
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//��??��1?D??�����̨�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//?��??��??��??3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//������??��??3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ�����̨���1?��
	NVIC_Init(&NVIC_InitStructure);	//?��?Y???����?2?��y3?��??��VIC??��??��?��
  	

}

void USART3_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART4_RX_STA&0x8000)==0)//����δ���
		{
			if(USART4_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART4_RX_STA=0;//���մ���,���¿�ʼ
				else USART4_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART4_RX_STA|=0x4000;
				else
				{
					USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
					USART4_RX_STA++;
					if(USART4_RX_STA>(USART4_REC_LEN-1))USART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 

} 
	u8 t;
	u8 len;
	u8 z,s1,s2,s3,s4;
void URSART3_Recieve(void)
{
		USART_SendData(USART3,0x44);
		delay_ms(500);
		if(USART4_RX_STA&0x8000)
		{					   
			len=USART4_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				if(USART4_RX_BUF[t]==46)
				{
						z=USART4_RX_BUF[t-1]-48;
						s1=USART4_RX_BUF[t+1]-48;
						s2=USART4_RX_BUF[t+2]-48;
						s3=USART4_RX_BUF[t+3]-48;
						
						break;
				}
			
			}

	
			USART4_RX_STA=0;
}
		}

