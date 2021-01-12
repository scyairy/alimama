#ifndef __BSP_LED_H__
#define __BSP_LED_H__
#include "sys.h"

// led define

//#define led1_rcc                    RCC_AHB1Periph_GPIOF
//#define led1_gpio                   GPIOF
//#define led1_pin                    (GPIO_Pin_9)

//#define led2_rcc                    RCC_AHB1Periph_GPIOF
//#define led2_gpio                   GPIOF
//#define led2_pin                    (GPIO_Pin_10)

//#define LED1 PFout(9)
//#define LED2 PFout(10)


#define led1_rcc                    RCC_AHB1Periph_GPIOG
#define led1_gpio                   GPIOG
#define led1_pin                    (GPIO_Pin_14)

#define led0_rcc                    RCC_AHB1Periph_GPIOG
#define led0_gpio                   GPIOG
#define led0_pin                    (GPIO_Pin_6)

#define Infrared1                   PEin(0)//�м����
#define Infrared2					PEin(9)//�󷽺���
#define Infrared3                   PEin(11)//����м����                 
#define Infrared4                   PEin(13)//�Ҳ��м����
#define laser_l                     PCin(1)//���ǰ������laser1
#define laser_r                     PDin(2)//�Ҳ�ǰ������laser2
#define laser3                      PCin(11)//����
#define laser4                      PCin(5)//����

#define LED0											  PGout(6)
#define LED1											  PGout(14)





//static void led_thread_entry(void* parameter);

void led_init(void);
//int rt_led_init(void);
void Infrared_Init(void);
void Laser_Init(void);
void ILL_Init(void);
void twinkling(void);		//LED��˸

#endif
