#include "bsp_key.h"
#include "operation.h"

#ifdef KEY_DEBUG
#define printf               rt_kprintf /*ʹ��rt_kprintf�����*/
#else
#define printf(...)                     /*�����*/
#endif

#define KEY_RCCC	            RCC_AHB1Periph_GPIOC
#define KEY_RCCA	            RCC_AHB1Periph_GPIOA
#define KEY_PORTC	        GPIOC
#define KEY_PORTA	        GPIOA
#define KEY_ENSURE_PIN	    GPIO_Pin_12
#define KEY_CANCEL_PIN	    GPIO_Pin_10
#define KEY_UP_PIN	        GPIO_Pin_11
#define KEY_DOWN_PIN	    GPIO_Pin_15




#define KEY_ALL_NSET		(0x0000|KEY_CANCEL_PIN|KEY_ENSURE_PIN|KEY_UP_PIN|KEY_DOWN_PIN)
#define KEY_OTHER		    (0x0000|KEY_ENSURE_PIN|KEY_UP_PIN|KEY_DOWN_PIN)

static void key_scan(void);

static rt_uint32_t key_value;

void key_thread_entry(void* parameter)
{

    while(1)
    {
        key_scan();
        rt_thread_delay(5);
    }
}

void key_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(KEY_RCCC|KEY_RCCA, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = KEY_ENSURE_PIN|KEY_CANCEL_PIN|KEY_UP_PIN|KEY_DOWN_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
  GPIO_Init(KEY_PORTA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = KEY_CANCEL_PIN;
  GPIO_Init(KEY_PORTC, &GPIO_InitStructure);
}

void key_scan(void)
{
    static vu16 s_KeyCode;//�ڲ���鰴��ʹ��
    static vu8 s_key_debounce_count, s_key_long_count;
    vu16	t_key_code;

    t_key_code = (GPIO_ReadInputData(KEY_PORTA) & KEY_OTHER)|(GPIO_ReadInputData(KEY_PORTC) & KEY_CANCEL_PIN);

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
                key_value=KEY_VALUE_UP;

                break;

            case   KEY_ALL_NSET & (~KEY_DOWN_PIN):	  
                key_value=KEY_VALUE_DOWN;
                break;

            case  KEY_ALL_NSET & (~KEY_ENSURE_PIN):		  
                key_value=KEY_VALUE_ENSURE;
                break;

            case   KEY_ALL_NSET & (~KEY_CANCEL_PIN):	
                key_value=KEY_VALUE_CANCEL;

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





