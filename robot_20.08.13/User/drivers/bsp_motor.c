#include "bsp_motor.h"
#include "JY61.h"
#include "bsp_compass.h"

//��������ʼ�� 
//PF1 PF3
static void Dir_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         //ʹ��GPIOFʱ��

   //��ʼ������
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
   GPIO_Init(GPIOD, &GPIO_InitStructure);                         //��ʼ��
 	
}

//���PWM��ʼ�� 
//PF6 PF7
static void PWM_Init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	      //TIM10ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 	      //ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);         //GPIOF6����Ϊ��ʱ��10
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);       //GPIOF7����Ϊ��ʱ��11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;           //GPIOF6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                   //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	           //�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);                          //��ʼ��PF6
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                       //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;      //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;                          //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                //��ʼ����ʱ��3
	
	//��ʼ��TIM3 Channel3\4 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;       //�������:TIM����Ƚϼ���
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);                      //����Tָ���Ĳ�����ʼ������TIM1 4OC1	
	
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);              //ʹ��TIM10��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);              //ʹ��TIM10��CCR1�ϵ�Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPEʹ�� 
	TIM_Cmd(TIM3,ENABLE);   
}
	

/*
	���ֺ���
	dir=1:�������
	dir=0:������ǰ
*/
void motor_L(uint16_t dir,uint16_t speed)
{
	Motor_Left_DIR=!dir;
	if(dir==1)Motor_Left_PWM=1054-speed;
	if(dir==0)Motor_Left_PWM=speed;
}
/*
	���ֺ���
	dir=1:�������
	dir=0:������ǰ
*/
void motor_R(uint16_t dir,uint16_t speed)
{

		Motor_Right_DIR=dir;
	if(dir==0)Motor_Right_PWM=1054-speed;
	if(dir==1)Motor_Right_PWM=speed;
}
static void motor(uint8_t dir1,uint16_t L_Speed,uint8_t dir2,uint16_t R_Speed)
{
//	float times;//300=0.985//350=0.989//400=0.992//450=0.996//500=1
//	if(L_Speed<=300){times=0.996;}
//	else if(L_Speed<=350){times=0.985;}
//	else if(L_Speed<=400){times=0.992;}
//	else if(L_Speed<=450){times=0.996;}
//	else {times=1;}
//	
//	if(L_Speed==400||R_Speed==400) times=1;
//	if(L_Speed==500||R_Speed==500) times=1.02;
//	if(L_Speed==550||R_Speed==550) times=0.983;
//	if(L_Speed==600||R_Speed==600) times=0.984;*times
	motor_L(dir1,L_Speed*0.98);
	motor_R(dir2,R_Speed);
}

/*
//dir:0��ǰ  1���
//RL_Tire��0���� 1����
static void motor(uint8_t dir,uint8_t RL_Tire,int16_t speed)
{   
	if(dir==0)
	{
		if(RL_Tire==0)
		{
			Motor_Right_DIR=1;
			Motor_Right_PWM=speed;

		}
		else if(RL_Tire==1)
		{
			
			Motor_Left_DIR=0;
			Motor_Left_PWM=750-speed;
		
		}
	}
	else if(dir==1)
	{
		if(RL_Tire==0)
		{
		    Motor_Right_DIR=0;
			Motor_Right_PWM=750-speed;
		}
		else if(RL_Tire==1)
		{
			Motor_Left_DIR=1;
			Motor_Left_PWM=speed;
		}
	}
}
*/

void motor_stop(void)
{
	Motor_Right_DIR=1;
	Motor_Right_PWM=1;
	Motor_Left_DIR=1;
	Motor_Left_PWM=1;
}

/*
	�˶�����
	Speed_Left:�����ٶ�
	Speed_Right:�����ٶ�
*/
void mot(int16_t Speed_Left,int16_t Speed_Right)
{
	if(Speed_Left>=0&&Speed_Right>=0)motor(1,Speed_Left,1,Speed_Right);
	if(Speed_Left<0&&Speed_Right>0)motor(0,-Speed_Left,1,Speed_Right);
	if(Speed_Left<0&&Speed_Right<0)motor(0,-Speed_Left,0,-Speed_Right);
	if(Speed_Left>0&&Speed_Right<0)motor(1,Speed_Left,0,-Speed_Right);
}

void turn_angle(int ang,int speed)
{
    int angle0,angle1,angle2;//angle0Ϊ��ʼ�ǣ�angle1ΪĿ��Ƕȣ�angle2Ϊ�״�ת�ǣ�Ϊ��ʹת�Ǹ�����ȣ���������ʽת����
//    use_compass=1;
    rt_thread_delay(25);
    angle0=Angle_z;
    angle1=(angle0+ang+360)%360;
    angle2=(angle0+ang/2+360)%360;
    if(ang>0)
    {
        while(!(angle2==Angle_z||angle2==Angle_z-1||angle2==Angle_z+1||angle2==Angle_z+2))//�����趨�ٶ�תһ��
        {
            mot(-speed,speed);
            rt_thread_delay(5);
        }
        motor_stop();
        rt_thread_delay(10);
        while(!(angle1==Angle_z||angle1==Angle_z-1||angle1==Angle_z+1))//��100���ٶ�ת��ָ���Ƕ�
        {
            mot(-170,170);
            rt_thread_delay(5);
        }
    }
    else
    {
        while(!(angle2==Angle_z||angle2==Angle_z-1||angle2==Angle_z-2||angle2==Angle_z+1))
        {
            mot(speed,-speed);
            rt_thread_delay(5);
        }
        motor_stop();
        rt_thread_delay(10);
        while(!(angle1==Angle_z||angle1==Angle_z-1||angle1==Angle_z-2||angle1==Angle_z+1))
        {
            mot(150,-150);
            rt_thread_delay(5);
        }
    }
    motor_stop();
//    use_compass=0;
		rt_thread_delay(100);
}


void motorr(int left_speed,int right_speed)
{
	mot(- left_speed,- right_speed);
}
/*
void mot(int16_t speed_left,int16_t speed_right)
{
	if(speed_left>=0&&speed_right>=0)
	{
		motor(0,1,speed_left);      //������ǰ
		motor(0,0,speed_right);      //������ǰ
	}
	else
	{
		if(speed_left<0&&speed_right<0)
		{
			motor(1,1,-speed_left); //�������
			motor(1,0,-speed_right); //�������
		}
		else if(speed_left<0&&speed_right>0)
		{
			motor(1,1,-speed_left); //�������
			motor(0,0,speed_right);  //������ǰ
		}
		else
		{
			motor(0,1,speed_left);  //������ǰ
			motor(1,0,-speed_right); //�������
		}
	}
}
*/

void motor_init(void)
{
    Dir_Init();
	PWM_Init(1054-1,7-1);
	motor_stop();
}














