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

/* 指向互斥量的指针 */
rt_mutex_t grayscale_mutex = RT_NULL;


//定义采集定灰度值的数组
int16_t  FrontGrayscale[24];              
int16_t  BackGrayscale[24];
int16_t  MidGrayscale[24];
uint8_t Color = 0;
uint8_t f[8];
uint8_t b[8];


//外部声明 前灰度用于储存灰度值的数组
int16_t F_White[8];                    
int16_t F_Green[8];                    
int16_t F_Black[8];                    
//外部声明 后灰度用于储存灰度值的数组
int16_t B_White[8];                   
int16_t B_Green[8];                  
int16_t B_Black[8];   
//外部声明 用于储存中心线灰度值的数组
int16_t Mid_Grayscle[8];
//定义存储在Flash中和从Flash中读取后的存储的数组
int16_t  F_white_green[8];              //用于储存白色和绿色的平均值
int16_t  F_white_black[8];               //用于储存白色和灰色的平均值
int16_t  F_green_black[8];               //用于储存灰色和绿色的平均值

int16_t B_white_green[8];              //用于储存白色和绿色的平均值
int16_t B_white_black[8];               //用于储存白色和灰色的平均值
int16_t B_green_black[8];               //用于储存灰色和绿色的平均值




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
//初始化灰度传感器
void grayscale_init(void)
{
	//初始化CD4051
    //前灰度:PF4 PF5 PF6
    //后灰度:PF7 PF8 PF9
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


	
    //初始化灰度传感器的ADC
	//前灰度:PC2 
	//后灰度:PC3
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;         //模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;    //不带上下拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);               //初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE); //复位结束	 
 
	
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                    //独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;     //DMA失能
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                 //预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);                                   //初始化
	
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      //12位模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                               //非扫描模式	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //关闭连续转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //右对齐	
    ADC_InitStructure.ADC_NbrOfConversion = 1;                                  //1个转换在规则序列中 也就是只转换规则序列1 
    ADC_Init(ADC1, &ADC_InitStructure);                                         //ADC初始化
 
	ADC_Cmd(ADC1, ENABLE);  
}

//函数功能：获得ch通道的AD值
//ch:通道值 5和6
uint16_t Get_Adc(uint8_t ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_84Cycles );	         //ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
	ADC_SoftwareStartConv(ADC1);		                                         //使能指定的ADC1的软件转换启动功能		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                              //等待转换结束
	return ADC_GetConversionValue(ADC1);	                                     //返回最近一次ADC1规则组的转换结果
}

//灰度平均值滤波
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
//采集定灰度值 
//入口参数为此文件中定义的全局变量(TOP)
void Get_FGrayscale(int16_t* grayscale)
{

//1
		A=1;B=1;C=1;
		grayscale[7]=Get_Adc_Average(12,3);//获取通道5的转换值，3次取平
		
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


int16_t pre_calculate()//计算偏移量
{
    int error = 0;         //偏差值
    int H_SETPOINT = 4800 ; //前灰度传感器居中参照值 4800（需要修改）
	int16_t grayscale[8];//采集的 实时灰度值
	int All_Channel = 0;   //灰度值总和
	int All_Result = 0;    //处理过后的灰度总和
	int Finall_Result = 0; //最终处理值
	uint8_t i;//循环计数器
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
	int All_Channel = 0;   //灰度值总和
	int All_Result = 0;    //处理过后的灰度总和
	int Finall_Result = 0; //最终处理值
	uint8_t i;//循环计数器	
	for ( i = 0; i<8; i++) 
	{
		All_Channel = All_Channel+BackGrayscale[i];
		All_Result  = All_Result+(BackGrayscale[i]*(i+1));

	}
    Finall_Result = All_Result*1000/All_Channel;
return Finall_Result;	
}