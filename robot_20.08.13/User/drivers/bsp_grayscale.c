#include <rtthread.h>
#include "bsp_grayscale.h"
#include "bsp_oled.h"


#define GrayscaleControl_RCC	            RCC_AHB1Periph_GPIOF
#define GrayscaleControl_PORT	            GPIOF
#define A0_PIN	    GPIO_Pin_4
#define A1_PIN	    GPIO_Pin_5
#define A2_PIN	    GPIO_Pin_6
#define B0_PIN	    GPIO_Pin_7
#define B1_PIN	    GPIO_Pin_8
#define B2_PIN	    GPIO_Pin_9

#define GrayscaleControl_PIN  (A0_PIN|A1_PIN|A2_PIN|B0_PIN|B1_PIN|B2_PIN)



ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t get_grayscale_stack[512];
static struct rt_thread get_grayscale_thread;

/* ָ�򻥳�����ָ�� */
rt_mutex_t grayscale_mutex = RT_NULL;


//����ɼ����Ҷ�ֵ������
int16_t  FrontGrayscale[24];              
int16_t  BackGrayscale[24];
int16_t  MidGrayscale[24];
uint8_t Color = 0;
uint8_t f[8];
uint8_t b[8];


//�ⲿ���� ǰ�Ҷ����ڴ���Ҷ�ֵ������
int16_t F_White[8];                    
int16_t F_Green[8];                    
int16_t F_Black[8];                    
//�ⲿ���� ��Ҷ����ڴ���Ҷ�ֵ������
int16_t B_White[8];                   
int16_t B_Green[8];                  
int16_t B_Black[8];   
//�ⲿ���� ���ڴ��������߻Ҷ�ֵ������
int16_t Mid_Grayscle[8];
//����洢��Flash�кʹ�Flash�ж�ȡ��Ĵ洢������
int16_t  F_white_green[8];              //���ڴ����ɫ����ɫ��ƽ��ֵ
int16_t  F_white_black[8];               //���ڴ����ɫ�ͻ�ɫ��ƽ��ֵ
int16_t  F_green_black[8];               //���ڴ����ɫ����ɫ��ƽ��ֵ

int16_t B_white_green[8];              //���ڴ����ɫ����ɫ��ƽ��ֵ
int16_t B_white_black[8];               //���ڴ����ɫ�ͻ�ɫ��ƽ��ֵ
int16_t B_green_black[8];               //���ڴ����ɫ����ɫ��ƽ��ֵ




static void get_grayscale_entry(void* parameter)
{


	while(1)
	{	
		rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		switch(Color)
		{
			case Front_WhiteGreen:
				F_Compare(F_white_green);break;
			case Front_WhiteBlack:
				F_Compare(F_white_black);break;
			case Front_GreenBlack:
				F_Compare(F_green_black);break;
			case Back_WhiteGreen:
				B_Compare(B_white_green);break;
			case Back_WhiteBlack:
				B_Compare(B_white_black);break;
			case Back_GreenBlack:
				B_Compare(B_green_black);break;
			case Front_Back_WhiteGreen:
				{
					F_Compare(F_white_green);
					B_Compare(B_white_green);
					break;
				}
			case Front_Back_WhiteBlack:
				{
					F_Compare(F_white_black);
					B_Compare(B_white_black);
					break;
				}
			case Front_Back_GreenBlack:
			{
				F_Compare(F_green_black);
				B_Compare(B_green_black);
				break;
			}
			default:
				break;
		}
		rt_mutex_release(grayscale_mutex);
		rt_thread_delay(2);
		
	}
}
int GetGrayscale(void)
{
	rt_err_t result;
	
	grayscale_mutex = rt_mutex_create("grayscale_mutex", RT_IPC_FLAG_FIFO);
	/* init get_grayscale_thread */
    result = rt_thread_init(&get_grayscale_thread,
                            "getgray",
                            get_grayscale_entry,
                            RT_NULL,
                            (rt_uint8_t*)&get_grayscale_stack[0],
                            sizeof(get_grayscale_stack),
                            16,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&get_grayscale_thread);
    }
	
	return 0;	
	
}
//��ʼ���Ҷȴ�����
void grayscale_init(void)
{
	//��ʼ��CD4051
    //ǰ�Ҷ�:PF4 PF5 PF6
    //��Ҷ�:PF7 PF8 PF9
    GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
    RCC_AHB1PeriphClockCmd(GrayscaleControl_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GrayscaleControl_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GrayscaleControl_PORT, &GPIO_InitStructure);


	
    //��ʼ���Ҷȴ�������ADC
	//ǰ�Ҷ�:PC2 
	//��Ҷ�:PC3
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;         //ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;    //����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);               //��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE); //��λ����	 
 
	
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                    //����ģʽ
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;     //DMAʧ��
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                 //Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);                                   //��ʼ��
	
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      //12λģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                               //��ɨ��ģʽ	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //�ر�����ת��
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //��ֹ������⣬ʹ���������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //�Ҷ���	
    ADC_InitStructure.ADC_NbrOfConversion = 1;                                  //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC_Init(ADC1, &ADC_InitStructure);                                         //ADC��ʼ��
 
	ADC_Cmd(ADC1, ENABLE);  
}

//�������ܣ����chͨ����ADֵ
//ch:ͨ��ֵ 5��6
uint16_t Get_Adc(uint8_t ch)   
{
	  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_84Cycles );	         //ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
	ADC_SoftwareStartConv(ADC1);		                                         //ʹ��ָ����ADC1�����ת����������		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                              //�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	                                     //�������һ��ADC1�������ת�����
}

//�Ҷ�ƽ��ֵ�˲�
int16_t Get_Adc_Average(int8_t ch,int8_t times)
{
	int32_t temp_val=0;
	int8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		__nop();__nop();__nop();
	}
	return temp_val/times;
} 

int HD(uint8_t i,uint8_t j)
{
	int t;
	if(i==1)
	{
		switch(j)
		{
			case 1:D=0;E=0;F=0;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 2:D=0;E=1;F=0;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 3:D=1;E=0;F=0;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 4:D=1;E=1;F=0;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 5:D=0;E=0;F=1;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 6:D=0;E=1;F=1;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 7:D=1;E=0;F=1;__nop();__nop();__nop();t=Get_Adc(13);break;
			case 8:D=1;E=1;F=1;__nop();__nop();__nop();t=Get_Adc(13);break;
			default:break;
	     }
	}
	else if(i==2)
	{
		switch(j)
		{
			case 1:A=1;B=1;C=1;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 2:A=0;B=1;C=1;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 3:A=1;B=0;C=1;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 4:A=0;B=0;C=1;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 5:A=1;B=1;C=0;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 6:A=0;B=1;C=0;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 7:A=1;B=0;C=0;__nop();__nop();__nop();t=Get_Adc(12);break;
			case 8:A=0;B=0;C=0;__nop();__nop();__nop();t=Get_Adc(12);break;
			default:break;
	     }
	}
		
	return t;
}
//�ɼ����Ҷ�ֵ 
//��ڲ���Ϊ���ļ��ж����ȫ�ֱ���(TOP)
void Get_FGrayscale(int16_t* grayscale)
{

//1
		A=1;B=1;C=1;
		grayscale[7]=Get_Adc_Average(12,3);//��ȡͨ��5��ת��ֵ��3��ȡƽ
		
//     5
		A=0;B=1;C=1;
		grayscale[6]=Get_Adc_Average(12,3);
		
//		2
		A=1;B=0;C=1;
		grayscale[5]=Get_Adc_Average(12,3);
		
//		6
		A=0;B=0;C=1;
		grayscale[4]=Get_Adc_Average(12,3);
		
//		3
		A=1;B=1;C=0;
		grayscale[3]=Get_Adc_Average(12,3);
		
//		7
		A=0;B=1;C=0;
		grayscale[2]=Get_Adc_Average(12,3);
		
//		4
		A=1;B=0;C=0;
		grayscale[1]=Get_Adc_Average(12,3);
		
//		8
		A=0;B=0;C=0;
		grayscale[0]=Get_Adc_Average(12,3);
		
}

void Get_BGrayscale(int16_t* grayscale)
{
		D=0;E=0;F=0;
		grayscale[7]=Get_Adc_Average(13,3);           
		
		D=1;E=0;F=0;
		grayscale[6]=Get_Adc_Average(13,3);           
		
		D=0;E=1;F=0;
		grayscale[5]=Get_Adc_Average(13,3);            
		
		D=1;E=1;F=0;
		grayscale[4]=Get_Adc_Average(13,3);           
		
		D=0;E=0;F=1;
		grayscale[3]=Get_Adc_Average(13,3);           
		
		D=1;E=0;F=1;
		grayscale[2]=Get_Adc_Average(13,3);            
		
		D=0;E=1;F=1;
		grayscale[1]=Get_Adc_Average(13,3);           
		
		D=1;E=1;F=1;
		grayscale[0]=Get_Adc_Average(13,3);           		
}

void F_Compare(int16_t* aver)
{
	uint8_t i;
	int16_t grayscale[8];
	Get_FGrayscale(grayscale);
	for(i=0;i<8;i++)
	{
		f[i] = (grayscale[i]>aver[i]);
	}
}

void B_Compare(int16_t* aver)
{
	uint8_t i;
	int16_t grayscale[8];
	Get_BGrayscale(grayscale);
	for(i=0;i<8;i++)
	{
		b[i] = (grayscale[i]>aver[i]);
	}
}


int16_t pre_calculate()//����ƫ����
{
    int error = 0;         //ƫ��ֵ
    int H_SETPOINT = 4800 ; //ǰ�Ҷȴ��������в���ֵ 4800����Ҫ�޸ģ�
	int16_t grayscale[8];//�ɼ��� ʵʱ�Ҷ�ֵ
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
	uint8_t i;//ѭ��������
//	H_SETPOINT=Get_H_SETPOINT();
	Get_FGrayscale(grayscale);
	
	for ( i = 0; i<8; i++) 
	{
		All_Channel = All_Channel+grayscale[i];
		All_Result  = All_Result+(grayscale[i]*(i+1));

	}
    Finall_Result = All_Result*1000/All_Channel;
	error = H_SETPOINT - Finall_Result; 

    return error;	
}
int16_t Get_H_SETPOINT()
{
	int SETPOINT=0;
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
	uint8_t i;//ѭ��������	
	for ( i = 0; i<8; i++) 
	{
		All_Channel = All_Channel+BackGrayscale[i];
		All_Result  = All_Result+(BackGrayscale[i]*(i+1));

	}
    Finall_Result = All_Result*1000/All_Channel;
return Finall_Result;	
}