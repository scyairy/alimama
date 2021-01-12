#include <rtthread.h>
#include "bsp_compass.h"
#include "bsp_oled.h"
#include "bsp_led.h"
#include "board.h"
#include "stdio.h"

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
				if(ucRxBuffer[7]==(u8)(0x0D+0X0A+ucRxBuffer[2]+ucRxBuffer[3]+ucRxBuffer[4]+ 0x2E +ucRxBuffer[6]) )
				{
					LED1=!LED1;
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

// int Angle_x,Angle_y,Angle_z;
// u8 angle_test;

// //JY61
// #ifdef RT_USE_JY61

// void compass_thread_entry(void* parameter)
// {
//     rt_uint8_t uart_rx_data0[11],temp;
//     if (uart_open("uart2") != RT_EOK)
//     {
//         rt_kprintf("uart open error.\n");
//          while (1)
//          {
//             rt_thread_delay(10);
//          }
//     }
// //    rt_kprintf("%s",uart_device);
//     while(1)
//     {
//         temp  = uart_getchar();
//         if(temp==0x55)
//         {
//             uart_rx_data0[0]  = temp;
//             temp = uart_getchar();
//             if(temp==0x53)
//             {
//                 uart_rx_data0[1]  = temp;
//                 uart_rx_data0[2]  = uart_getchar(); //RollL
//                 uart_rx_data0[3]  = uart_getchar(); //RollH
//                 uart_rx_data0[4]  = uart_getchar(); //PitchL
//                 uart_rx_data0[5]  = uart_getchar(); //PitchH
//                 uart_rx_data0[6]  = uart_getchar(); //YawL
//                 uart_rx_data0[7]  = uart_getchar(); //YawH
//                 uart_rx_data0[8]  = uart_getchar(); //TL
//                 uart_rx_data0[9]  = uart_getchar(); //TH
//                 uart_rx_data0[10] = uart_getchar(); //Sum=0x55+0x53+RollH+RollL+PitchH+PitchL+YawH+YawL+TH+TL
//                 Angle_x=(int)((uart_rx_data0[7]<<8)|uart_rx_data0[6])/32768.0*180.0;    //Roll=((RollH<<8)|RollL)/32768*180(°)
//                 Angle_y=(int)((uart_rx_data0[5]<<8)|uart_rx_data0[4])/32768.0*180.0;
//                 Angle_z=(int)((uart_rx_data0[3]<<8)|uart_rx_data0[2])/32768.0*180.0;
                
//                 if(angle_test)
//                 {
// //                    rt_kprintf("x:%d\ty:%d\tz:%d\n\r",Angle_x,Angle_y,Angle_z);
//                     OLED_ShowNum (34,2,Angle_x,3,16);
//                     OLED_ShowNum (34,4,Angle_y,3,16);
//                     OLED_ShowNum (34,6,Angle_z,3,16);
//                     rt_thread_delay(10);
//                 }
// //                    rt_kprintf("\n%d",Angle_y);
// //                    if (use_compass==1)//test code
// //                        rt_kprintf("x:%d\ty:%d\n\r",Angle_x,Angle_y);
//                 rt_thread_delay(5);
//             }
//         }
//     }
// }

// #include <finsh.h>

// static void show_compass()
// {
//     rt_kprintf("\nAngle_x:%d\tAnlge_y:%d\tAngle_z:%d",Angle_x,Angle_y,Angle_z);
// }

// FINSH_FUNCTION_EXPORT(show_compass, show the angle of the smart car in serial)



// void compass_thread_creat(void)
// {
//     rt_thread_t tid;
//     // 创建compass线程 
//     tid = rt_thread_create("compass",
//                     compass_thread_entry, 
//                     RT_NULL,
//                     0x200, 
//                     10,
//                     6);
//     // 创建成功则启动线程 
//     if (tid != RT_NULL)
//         rt_thread_startup(tid);
// //        rt_kprintf ("compass start sucessful\n");
// }


// #endif //end RT_USE_JY61
// //GY26
// #ifdef RT_USE_GY26

// void compass_thread_entry(void* parameter)
// {
//     rt_uint8_t uart_rx_data0[8];
//     if (uart_open("uart2") != RT_EOK)//打开串口
//     {
//         rt_kprintf("uart open error.\n");
//          while (1)
//          {
//             rt_thread_delay(10);
//          }
//     }
//     while(1)
//     {
//         if(use_compass==1||angle_test==1)
//         {
//             uart_rx_data0[0]  = uart_getchar();
//             if(uart_rx_data0[0]==13)
//             {
//                 uart_rx_data0[1]  = uart_getchar();
//                 uart_rx_data0[2]  = uart_getchar();
//                 uart_rx_data0[3]  = uart_getchar();
//                 uart_rx_data0[4]  = uart_getchar();
//                 uart_rx_data0[5]  = uart_getchar();
//                 uart_rx_data0[6]  = uart_getchar();
//                 uart_rx_data0[7]  = uart_getchar();
//                 Angle_x= (uart_rx_data0[2]-48)*100+(uart_rx_data0[3]-48)*10+uart_rx_data0[4]-48;
//             }
//             if(angle_test)
//             {
//                 oled.show_number (34,4,Angle_x,3,14);
//             }
// //            rt_kprintf("%d\n\r",Angle_x);
//             use_compass=0;
//         }
//         else
//         rt_thread_delay(5);
//     }
// }

// void compass_thread_creat(void)
// {
//     rt_thread_t tid;
//     // 创建test线程
//     tid = rt_thread_create("compass",
//                     compass_thread_entry, 
//                     RT_NULL,
//                     1024, 
//                     10, 
//                     5);
//     // 创建成功则启动线程 
//     if (tid != RT_NULL)
//         rt_thread_startup(tid);
// }

// void refresh_angle(void)
// {
//     USART_SendData(USART2,0x31);
//     use_compass=1;
// }

// #endif  //end RT_USE_GY26