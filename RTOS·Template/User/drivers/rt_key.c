#include "sys.h"   
#include "stdlib.h"
#include "rt_key.h"
#include <rtthread.h>
#include "usart.h"
#include "operation.h"
#include "rt_oled.h"
#include "rt_led.h"

#define KEY_RCC	            RCC_AHB1Periph_GPIOG
#define KEY_PORT	        GPIOG
#define KEY_ENSURE_PIN	    GPIO_Pin_13
#define KEY_CANCEL_PIN	    GPIO_Pin_10
#define KEY_UP_PIN	        GPIO_Pin_12
#define KEY_DOWN_PIN	    GPIO_Pin_11

#define KEY_ALL_NSET		(0x0000|KEY_ENSURE_PIN|KEY_CANCEL_PIN|KEY_UP_PIN|KEY_DOWN_PIN)

void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//

 
} 
//rt_uint8_t key_value;
static vu8 s_key_debounce_count, s_key_long_count;
vu16	t_key_code;
static vu16 s_KeyCode;//�ڲ���鰴��ʹ��
 void key_thread_entry(void *paramete)
{	 
	while(1)
	{
		rt_key_scan();
		rt_thread_delay(10);
	}
	
}
void rt_key_scan(void)
{
    static vu16 s_KeyCode;//�ڲ���鰴��ʹ��
    static vu8 s_key_debounce_count, s_key_long_count;
    vu16	t_key_code;

    t_key_code = GPIO_ReadInputData(GPIOG) & KEY_ALL_NSET;

    if((t_key_code == KEY_ALL_NSET)||(t_key_code != s_KeyCode))
    {
        s_key_debounce_count = 0;	//��һ��
        s_key_long_count = 0;
    }
    else
    {
        if(++s_key_debounce_count == DEBOUNCE_SHORT_TIME)
        {   //�̰���
            switch(s_KeyCode)
            {
            case   KEY_ALL_NSET & (~KEY_UP_PIN):	  
                key_value=KEY2_PRES;

                break;

            case   KEY_ALL_NSET & (~KEY_DOWN_PIN):	  
                key_value=KEY1_PRES;
                break;

            case  KEY_ALL_NSET & (~KEY_ENSURE_PIN):		  
                key_value=KEY3_PRES;
                break;

            case   KEY_ALL_NSET & (~KEY_CANCEL_PIN):	
                key_value=KEY0_PRES;

                break;

            default:				  //������ϲ�������
                break;

            }
            /*  ���������������з�һ��ָ��*/
            rt_mb_send(key_value_mb, key_value);

#ifdef KEY_DEBUG
            printf("key %d is set\n", s_KeyCode);
#endif
        }
        if(s_key_debounce_count == DEBOUNCE_COUT_FIRST + DEBOUNCE_COUT_INTERVAL)
        {   //������
            s_key_debounce_count = DEBOUNCE_COUT_FIRST;
            ++s_key_long_count;
            /*
            switch(s_KeyCode)
            {
            case   KEY_ALL_NSET & (~KEY_UP_PIN):	   //up


            	break;

            case   KEY_ALL_NSET & (~KEY_DOWN_PIN):	  //down


            	break;
            }	*/
        }

        if(s_key_long_count == DEBOUNCE_LONG_TIME)
        {   //������

            s_key_long_count = DEBOUNCE_LONG_TIME;
        }
    }
    s_KeyCode = t_key_code;				// ���汾�μ�ֵ


}
