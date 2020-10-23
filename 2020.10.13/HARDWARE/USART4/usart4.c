#include "sys.h"
#include "usart4.h"
#include "delay.h"

u16 USART4_RX_STA=0; 
u8 num=0;
u8  USART4_RX_BUF[USART4_REC_LEN];

void usart3_init(u32 bound)
{  

   //GPIO???úéè??
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ê1?üGPIOCê±?ó
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ê1?üUSART4ê±?ó
 
	//′??ú1??ó|òy???′ó?ó3é?
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOC10?′ó??aUART4
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOC11?′ó??aUART4
	
	//UART4???ú????
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10ó?GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//?′ó?1|?ü
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//?ù?è50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //í?íì?′ó?ê?3?
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //é?à-
	GPIO_Init(GPIOB,&GPIO_InitStructure); //3?ê??ˉPc10￡?PC11

   //UART43?ê??ˉéè??
	USART_InitStructure.USART_BaudRate = bound;//2¨ì??êéè??
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×?3¤?a8??êy?Y??ê?
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ò???í￡?1??
	USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D￡?é??
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?Tó2?têy?Yá÷????
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//ê?·￠?￡ê?
  USART_Init(USART3, &USART_InitStructure); //3?ê??ˉ′??ú1
	
  USART_Cmd(USART3, ENABLE);  //ê1?ü′??ú4 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//?a???à1??D??

	//Usart4 NVIC ????
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//′??ú1?D??í¨μà
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//?à??ó??è??3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//×óó??è??3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQí¨μàê1?ü
	NVIC_Init(&NVIC_InitStructure);	//?ù?Y???¨μ?2?êy3?ê??ˉVIC??′??÷?￠
  	

}

void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据
		
		if((USART4_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART4_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART4_RX_STA=0;//接收错误,重新开始
				else USART4_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART4_RX_STA|=0x4000;
				else
				{
					USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
					USART4_RX_STA++;
					if(USART4_RX_STA>(USART4_REC_LEN-1))USART4_RX_STA=0;//接收数据错误,重新开始接收	  
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
			len=USART4_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
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

