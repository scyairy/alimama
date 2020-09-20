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
#define led0_pin                    (GPIO_Pin_15)

#define Infrared1                   PDin(11)//改过
#define Infrared2										PDin(10)//改过
#define laser_l                      PBin(12)
#define laser_r                      PBin(13)

#define LED0											  PGout(15)
#define LED1											  PGout(14)





//static void led_thread_entry(void* parameter);

void led_init(void);
//int rt_led_init(void);
void Infrared_Init(void);
void Laser_Init(void);
void ILL_Init(void);
void twinkling(void);		//LED闪烁

#endif
