#include <rtthread.h>
#include "robot_run.h"
#include "operation.h"
#include "bsp_led.h"
#include "bsp_grayscale.h"
#include "map.h"
#include "track.h"
#include "bsp_motor.h"
#include "bsp_oled.h"
#include "route_selection.h"
#include "JY61.h"
#include "image.h"
void route_1_To_2(void)
{
	Down_Platform();	
	PID_to_Angle(400);
	motor_time(500,370,370);
	though_brige(310);
	motor_stop();
  rt_thread_delay(300);
	PID_to_Angle(500);
	Up_Platform();
	//turn_angle(180,500);
//	twinkling11111();
}
void route_2_To_4(void)
{
		turn_angle(-178,400);
	  rt_thread_delay(100);
		Down_Platform();
//	Road_to_time(350,200);//��ʱ���  ��Ҫ����
	  slow_acceleration(200,385,500);//��ֹ�Ͽ����С������
	  Tiyi_Laser_R_1point(385);
//	Laser_track_point(450,1);		
		motor_stop();
		rt_thread_delay(200);
		motor_time(300,-200,-200);//����һ��
		rt_thread_delay(100);
		turn_angle(-28,350);//30
		rt_thread_delay(100);		
		slow_acceleration(200,320,500);//��ֹ�Ͽ����С������
		PID_to_Angle(320);
//  Retarder_plate(1600);	
	  through_slow_down(200,210,1200);//�����ٰ壬�ر�ע��ʱ��
	  through_slow_down(210,200,1200);//�����ٰ壬�ر�ע��ʱ��
	  PID_Laser_R_1point(350);
	  motor_time(350,300,300);//ǰ��һ��		
	  rt_thread_delay(100);//��ת����ͣ0.1s
		
		turn_angle(26,350);
		rt_thread_delay(100);//��ת����ͣ0.1s
		
//	  Tiyi_Laser_R_1point(320);
//	motor_stop();
//	while(1);
		PID_to_Angle(450);
		Up_Platform();
//	twinkling11111();
}
void route_2_To_3(void)
{
		turn_angle(-178,400);
	  rt_thread_delay(100);
		Down_Platform();
//		Road_to_time(350,200);//��ʱ���  ��Ҫ����
	  slow_acceleration(200,385,500);//��ֹ�Ͽ����С������
	  Tiyi_Laser_R_1point(385);
//	  Laser_track_point(450,1);		
		motor_stop();
		rt_thread_delay(200);
		motor_time(270,-200,-200);//����һ��
		rt_thread_delay(100);
		turn_angle(-30,350);//30
		rt_thread_delay(100);		
		slow_acceleration(200,320,500);//��ֹ�Ͽ����С������
		Road_to_Angle(320);
	  through_slow_down(200,210,1200);//�����ٰ壬�ر�ע��ʱ��
	  through_slow_down(210,200,1200);//�����ٰ壬�ر�ע��ʱ��
	  Tiyi_Laser_R_1point(350);
	  motor_time(350,300,300);//ǰ��һ��		
	  rt_thread_delay(100);//��ת����ͣ0.1s
		turn_angle(-145,350);
		
		Tiyi_Laser_L_1point(320);
		Road_to_Angle(450);
		Up_Platform();
		

}
void route_3_To_5(void)
{
		
//		u8 which_gray[3]={5,6,7};
		Down_Platform();
		slow_acceleration(200,500,500);
		Tiyi_Laser_R_1point(500);
		motor_time(100,200,200);
		rt_thread_delay(100);//��⵽·��ͣ0.1s
		turn_angle(-36,300);//32
		rt_thread_delay(100);
		slow_acceleration(200,450,300);//��ֹ�Ͽ����С������
		Road_to_Angle(450);
		through_door(250,1600);	
		slow_acceleration(250,500,500);
		Tiyi_Laser_R_2point(500);
		motor_time(500,200,200);//210 200 200
		rt_thread_delay(100);//��⵽·��ͣ0.2s
		turn_angle(30,350);
		rt_thread_delay(100);//��⵽·��ͣ0.2s
		slow_acceleration(200,500,400);//��ֹ�Ͽ����С������
		Road_to_Angle(500);
		Up_Platform();
//		twinkling11111();
}

void route_3_To_6(void)
{
		turn_angle(178,350);
//		u8 which_gray[3]={5,6,7};
		Down_Platform();
		slow_acceleration(200,500,500);//��ֹ�Ͽ����С������
		Tiyi_Laser_R_1point(500);
		motor_time(150,300,300);
		rt_thread_delay(100);//��⵽·��ͣ0.1s
		turn_angle(-88,350);
		slow_acceleration(200,450,400);//��ֹ�Ͽ����С������
		Road_to_Angle(450);
		through_door(250,1600);//400,1100
	  slow_acceleration(200,500,400);
		Tiyi_Laser_R_1point(550);
		motor_time(100,200,200);
		rt_thread_delay(100);//��⵽·��ͣ0.2s
		turn_angle(90,400);
		through_qqb();//�����ΰ�
//		twinkling11111();
}

void route_4_To_5(void)
{

		turn_angle(178,400);
//		u8 which_gray[3]={0,1,2};
		Down_Platform();
    slow_acceleration(200,500,400);	
		PID_Laser_L_1point(500);		
		motor_time(100,200,200);
		rt_thread_delay(100);//ת�Ǻ�ͣ0.1s
		turn_angle(85,350);  //90
		rt_thread_delay(100);
		motor_time(100,200,200);
		PID_to_Angle(450);
		through_door(250,1600);//����  400  1200
		slow_acceleration(250,500,400);
		PID_Laser_L_1point(500);
		motor_time(100,200,200);
		rt_thread_delay(100);//��⵽·��ͣ0.2s	
		turn_angle(87,400);
		rt_thread_delay(100);
		slow_acceleration(200,450,300);//����ͻȻ������⵽����
		Road_to_Angle(500);
		Up_Platform();
//		twinkling11111();
}

void route_4_To_6(void)
{
		turn_angle(178,400);
//		u8 which_gray[3]={0,1,2};
		Down_Platform();
		slow_acceleration(200,500,400);
		Tiyi_Laser_L_1point(500);
		motor_time(150,200,200);
		rt_thread_delay(200);//��⵽·��ͣ0.1s
		turn_angle(32,350);  //34 400
		rt_thread_delay(100);
//		slow_acceleration(200,400,300);//��ֹ�Ͽ����С������
		Road_to_Angle(380);
		through_door(250,1600);//����ע��ʱ�䣡����������������������
		slow_acceleration(200,500,350);
		Tiyi_Laser_L_2point(500);
		motor_time(500,200,200);//��Ҫ����
		rt_thread_delay(100);//
		turn_angle(136,300);
//		motor_time(150,-300,-300);
		through_qqb();//�����ΰ�
//		twinkling11111();
}
void route_5_To_7(void)
{
	int mms=1000;
		turn_angle(178,400);
		Down_Platform();
		slow_acceleration(200,500,500);//
	  Tiyi_Laser_L_2point(500);
		motor_time(100,300,300);
	  rt_thread_delay(100);
		turn_angle(89,350);
		slow_acceleration(200,450,300);//��ֹ�Ͽ����С������
		Road_to_Angle(450);//����ϰ�
		while(1)
		{
			if(laser_l==1||laser_r==1)
			{
				rt_thread_delay(10);
				if(laser_l==1&&laser_r==0) break;//���ݼ��⴫�������
				if(laser_r==1) {motor_stop(); turn_angle(-3,300);break;}
			//	if(laser_l==1&&laser_r==1) {turn_angle(-3,300);break;}
			}
			else
			{mot(300,300);}
		}
		motor_stop();			
		rt_thread_delay(100);
		turn_angle_qqb(85,-150,300);
		while(mms--)
			
	{
				if(laser_l==1&&laser_r==0)
			{
				mot(300,300*0.9);
			}
			else if(laser_l==0&&laser_r==1)
			{
				mot(300*0.9,300);
			}				
		mot(225,225);
	}
//		{mot(300,300);}
		slow_acceleration(200,350,1000);
		Road_to_Angle(400);
		through_slow_down(255,240,900);//1.8s
		through_slow_down(240,255,900);//1.8s		
		through_slow_down(220,210,1200);//1.8s
//		through_slow_down(220,210,3500);//1.8s
		Road_to_Angle(350);//����ϰ�
		motor_time(100,0,170);//����˦һ˦
    rt_thread_delay(50);
    motor_time(100,170,0);
    Tiyi_Up_Mount(250,1);
		motor_stop();
//		twinkling11111();
}

void Tiyi_5_To_8(void)
{
		turn_angle(178,400);
		Down_Platform();
		slow_acceleration(200,500,500);
	  Tiyi_Laser_L_2point(500);
		motor_time(100,200,200);
	  rt_thread_delay(100);
		turn_angle(90,400);
		slow_acceleration(200,400,300);//��ֹ�Ͽ����С������
		PID_to_Angle(400);//����ϰ�
		while(1)
		{
			if(laser_l==1||laser_r==1)
			{
				rt_thread_delay(10);
				if(laser_l==1&&laser_r==0) break;//���ݼ��⴫�������
				if(laser_r==1) {motor_stop(); turn_angle(-3,300);break;}
			//	if(laser_l==1&&laser_r==1) {turn_angle(-3,300);break;}
			}
			else
			{mot(300,300);}
		}
		motor_stop();			
		rt_thread_delay(100);
		turn_angle_qqb(-90,300,-150);
		while(!Infrared2)
		{mot(300,300);}
		slow_acceleration(200,350,1000);
		PID_to_Angle(400);
		UP_montain(300,2);
		///Run_Mount(400);
		motor_stop();
//		twinkling11111();
}

void route_5_To_8(void)
{
	int mms=1000;
	turn_angle(178,400);
		Down_Platform();
		slow_acceleration(200,500,600);//
	  Tiyi_Laser_L_2point(500);
		motor_time(90,300,300);
	  rt_thread_delay(100);
		turn_angle(89,350);
		slow_acceleration(200,400,300);//��ֹ�Ͽ����С������
		Road_to_Angle(450);//����ϰ�
		while(1)
		{
			if(laser_l==1||laser_r==1)
			{
				rt_thread_delay(10);
				if(laser_l==1&&laser_r==0) break;//���ݼ��⴫�������
				if(laser_r==1) {motor_stop(); turn_angle(-3,300);break;}
			//	if(laser_l==1&&laser_r==1) {turn_angle(-3,300);break;}
			}
			else
			{mot(300,300);}
		}
		motor_stop();			
		rt_thread_delay(100);
		turn_angle_qqb(-85,215,-110);
		while(mms--)//Infrared2
		{
			if(laser_l==1&&laser_r==0)
			{
				mot(300,300*0.9);
			}
			else if(laser_l==0&&laser_r==1)
			{
				mot(300*0.9,300);
			}				
		mot(225,225);
		}
//		motor_stop();
//		rt_thread_delay(100);
//		
//		slow_acceleration(200,350,1000);
//		motor_stop();
//		while(1);
		Road_to_Angle(350);//����ϰ�
		
//		motor_time(170,0,200);//����˦һ˦
//    rt_thread_delay(50);
//    motor_time(170,200,0);
		
    Tiyi_Up_Mount(300,2);
//		twinkling11111();

}
void route_6_To_7(void)
{
	through_qqb_two();
	rt_thread_delay(300);
	slow_acceleration(200,350,500);
	Tiyi_Laser_R_1point(320);
	motor_time(200,200,200);
	Tiyi_Laser_R_1point(450);
	motor_time(150,200,200);
	rt_thread_delay(100);
	motor_stop();
	rt_thread_delay(200);
	turn_angle(-89,350);
	rt_thread_delay(100);
  slow_acceleration(200,500,500);
  Road_to_Angle(500);
	while(1)
		{
			if(laser_l==1||laser_r==1)
			{
				rt_thread_delay(10);
				if(laser_l==1&&laser_r==0) break;//���ݼ��⴫�������
				if(laser_r==1) {motor_stop(); turn_angle(-3,300);break;}
			//	if(laser_l==1&&laser_r==1) {turn_angle(-3,300);break;}
			}
			else
			{mot(300,300);}
		}
		motor_stop();
		rt_thread_delay(300);
		turn_angle_qqb(90,-150,300);
		while(!Infrared2)
		{mot(300,300);}
		slow_acceleration(200,350,1000);
		Road_to_Angle(450);
		through_slow_down(255,240,900);
		through_slow_down(240,255,900);	
		through_slow_down(220,210,1200);
//		through_slow_down(220,210,3500);//1.8s
		Road_to_Angle(320);//����ϰ�
		motor_time(100,0,170);//����˦һ˦
    rt_thread_delay(50);
    motor_time(100,170,0);
    Tiyi_Up_Mount(250,1);
		motor_stop();
//		twinkling11111();
	
}

void route_6_To_8(void)
{
	through_qqb_two();
	rt_thread_delay(300);
	slow_acceleration(200,350,500);
	Tiyi_Laser_R_1point(350);
	motor_time(180,300,300);
	Tiyi_Laser_R_1point(450);
	motor_time(170,200,200);
	rt_thread_delay(100);
	motor_stop();
	rt_thread_delay(200);
	turn_angle(-89,350);
	rt_thread_delay(100);
  slow_acceleration(200,500,600);
  Road_to_Angle(500);
	while(1)
		{
			if(laser_l==1||laser_r==1)
			{
				rt_thread_delay(10);
				if(laser_l==1&&laser_r==0) break;//���ݼ��⴫�������
				if(laser_r==1) {motor_stop(); turn_angle(-3,300);break;}
			//	if(laser_l==1&&laser_r==1) {turn_angle(-3,300);break;}
			}
			else
			{mot(300,300);}
		}
		motor_stop();			
		rt_thread_delay(100);
//		turn_angle_qqb(-90,350,-200);
		turn_angle_qqb(-88,250,-120);
		while(!Infrared2)
		{
			if(laser_l==1&&laser_r==0)
			{
				mot(300,300*0.9);
			}
			else if(laser_l==0&&laser_r==1)
			{
				mot(300*0.9,300);
			}				
		mot(230,230);
		}
		slow_acceleration(200,350,300);
		Road_to_Angle(400);//����ϰ�
		
//		motor_time(170,0,200);//����˦һ˦
//    rt_thread_delay(50);
//    motor_time(170,200,0);
		
    Tiyi_Up_Mount(300,2);
		///Run_Mount(400);
//		twinkling11111();

}
////////////////////////////
void liuleliule_8(void)
{
	int ms=1000;
		int time=224*3.6;
  turn_angle(176,400);
	Tiyi_down_8(250,2);
	rt_thread_delay(50);
	slow_acceleration(200,350,600);
	Color = Front_WhiteGreen;
//	while(ms--)
//	{		
//		ForwardTrack(450,450);	
//    rt_thread_delay(1);		
//	}
//  motor_stop();
//	rt_thread_delay(200);
	Road_to_Angle_8(350);
	motor_time(110,380,300);
	Xiepo_Laser8_time(380,310);
//	motor_time(1200,225,220);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300   1500,205,200
	rt_thread_delay(100);
	turn_angle_qqb(90,-300,200);
	turn_right_angle(300,0);
	motor_time(300,250,250);//��б��
	slow_acceleration(250,350,500);
	PID_Laser_L_1point(350);
//	Laser_track_point(450,0);
	motor_time(120,300,300);
	rt_thread_delay(200);
	turn_angle(87,400);
	rt_thread_delay(100);
	slow_acceleration(250,400,500);
	PID_Laser_R_1point(350);
//		track2one_point(7,450);
	motor_time(140,300,300);//130wen
	rt_thread_delay(100);
	turn_angle(-90,400);
	rt_thread_delay(100);
  PID_Laser_L_1point(330);
//	Road_to_time(400,1000);//����
	PID_to_Angle(450);//ѭ������  500
	through_door(250,1500);//����
		slow_acceleration(250,450,500);
	PID_Laser_L_1point(550);
//	track2one_point(0,450);
	motor_time(150,200,200);
	rt_thread_delay(200);
	turn_angle(-90,400);
	rt_thread_delay(200);
	PID_Laser_L_1point(500);
			motor_time(400,300,300);
			rt_thread_delay(100);
			turn_angle(25,350);//23
			slow_acceleration(250,400,400);
//			rt_thread_delay(1000);
			PID_to_Angle(300);
			
			
		motor_time(100,0,170);//����˦һ˦
    rt_thread_delay(50);
    motor_time(100,170,0);
	
	
////	if((Angle_x>3&&Angle_x<20)||(Angle_x>256&&Angle_x<300)) Road_to_time(400,1000);//���û���žͼ���mot
//	
//	slow_acceleration(250,450,600);
//	Tiyi_Laser_L_1point(550);
////	track2one_point(0,450);
//	motor_time(100,250,250);
//	rt_thread_delay(200);
//	turn_angle(-90,400);
//	rt_thread_delay(200);
//	Tiyi_Laser_L_1point(450);
//			motor_time(300,300,300);
//			rt_thread_delay(100);
//			turn_angle(26 ,350);
//			slow_acceleration(250,400,400);
////			rt_thread_delay(1000);
//			Road_to_Angle(400);
			while(1)//ȷ������
	{
		if(Angle_x<356&&Angle_x>330) break;//��⵽С���Ƕ����̱����ϰ�
		laser_track(250);
	}

	while(1)
		{
		//����ʱ��  224
		if (time==0) break;
		laser_track(300);
		rt_thread_delay(1);
		time--;
	}
	motor_stop();//
	rt_thread_delay(1200);
	//////////////////////////���ϵڶ���
	while(1){
	if(Angle_x>330&&Angle_x<350)//���δ����ѹ��ȥִ����δ���
	{
	motor_time(300,200,200);//�ѹ���
	rt_thread_delay(1000);
		//laser_track(100);
		//rt_thread_delay(1);
	}
	else break;
	} 

//	track2one_point(0,400);
	PID_Laser_L_1point(320);
	motor_time(350,300,300);
	rt_thread_delay(100);
	rt_kprintf("jiaodu1:%d\n",Angle_z);
//	motor_time(120,400,400);
	turn_angle(-28,350);
	rt_thread_delay(100);
	rt_kprintf("jiaodu2:%d\n",Angle_z);
	slow_acceleration(250,380,300);
	PID_to_Angle(380);
	Up_Platform();
	rt_thread_delay(300);
	turn_angle(-178,350);
}
////////////////////////////
void liuleliule_7(void)
{
		int time=224*3.6;
		turn_angle(180,400);
//		int16_t ax0;
//	ax0=Angle_x;

//	OLED_Clear();
//	OLED_ShowNum(60,0,ax0,4,14);
//	while(1)//ƽ̨��
//	{
//		show_angle();
//		if((Angle_x>5) && (Angle_x<20)) break;
//		else mot(270,270);
//		rt_thread_delay(1);
//	}
//	rt_thread_delay(100);	
//	while(1)//��
//	{
//		show_angle();
//		if(Angle_x==ax0 || (Angle_x==(ax0+3)%360) || (Angle_x==(ax0-3+360)%360)|| (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360) || (Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360))break;
//		else
//		{
//			Color = Front_WhiteBlack;
//			ForwardTrack(250,250);
//		}
//		rt_thread_delay(1);	
//	}
  Tiyi_down_8(230,1);
	motor_stop();
	slow_acceleration(250,400,400);
//		Road_to_Angle(400);//����ϰ�
		Road_to_Angle(320);
		through_slow_down(240,255,900);//1.8s
		through_slow_down(255,240,900);//1.8s
		through_slow_down(210,220,1200);//1.8s
//  	Retarder_plate(2600);	
		motor_stop();
	rt_thread_delay(100);
	while(!Infrared2)
	{
		Color = Front_WhiteGreen;
		ForwardTrack(350,350);
		rt_thread_delay(1);
	}
		motor_stop();
	rt_thread_delay(50);
//	motor_time(1200,205,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300   1500,205,200
	Xiepo_Laser7_time(850,310);
	rt_thread_delay(100);
//	while(1)
//	{
//			rt_thread_delay(1);
//		  Color = Front_WhiteGreen;
//			if(f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7]) break;
//			mot(205,200);
//		
//	}
//	motor_time(1600,210,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300  1500,205,20
//	//motor_time(360,300,300);//��⵽��б�º�mot     ����ʱ��Ҫ��
//	rt_thread_delay(1000);
	//turn_angle_qqb(-90,600);
	turn_angle_qqb(-90,200,-400);
	turn_right_angle(400,0);
	motor_time(300,250,250);//��б��
	slow_acceleration(250,350,500);
	Tiyi_Laser_L_1point(400);
//	Laser_track_point(450,0);
	motor_time(100,300,300);
	rt_thread_delay(200);
	turn_angle(87,350);
	rt_thread_delay(200);
	slow_acceleration(250,400,500);
	Tiyi_Laser_R_1point(450);
//		track2one_point(7,450);
	motor_time(120,300,300);
	rt_thread_delay(100);
	turn_angle(-88,350);
	rt_thread_delay(100);
	Tiyi_Laser_R_1point(350);
//	slow_acceleration(250,450,500);
//	Road_to_time(400,1000);//����
	Road_to_Angle(500);//ѭ������  500
	through_door(250,1600);//����
//	if((Angle_x>3&&Angle_x<20)||(Angle_x>256&&Angle_x<300)) Road_to_time(400,1000);//���û���žͼ���mot
	
	slow_acceleration(250,450,500);
	Tiyi_Laser_L_1point(550);
//	track2one_point(0,450);
	motor_time(140,200,200);
	rt_thread_delay(200);
	turn_angle(-89,350);
	rt_thread_delay(200);
		//�������������������������������������������������ȵ�һƥ
		track2one_point(0,500);
			motor_time(200,300,300);
			rt_thread_delay(200);
			turn_angle(23,400);//22
			rt_thread_delay(100);
//	Tiyi_Laser_L_1point(500);
//			motor_time(400,310,310);
////				rt_thread_delay(2000);
//			rt_thread_delay(100);
//			turn_angle(24,350);
//			rt_thread_delay(100);
//			slow_acceleration(250,400,400);
//			rt_thread_delay(1000);
			Road_to_Angle(300);
			
		motor_time(100,0,170);//����˦һ˦
    rt_thread_delay(50);
    motor_time(100,170,0);
			
			while(1)//ȷ������
	{
		if(Angle_x<356&&Angle_x>330) break;//��⵽С���Ƕ����̱����ϰ�
		laser_track(250);
	}

	while(1){
		//����ʱ��  224
		if (time==0) break;
		laser_track(300);
		rt_thread_delay(1);
		time--;
	}
	motor_stop();//
	rt_thread_delay(1000);
	//////////////////////////���ϵڶ���
	while(1){
	if(Angle_x>330&&Angle_x<350)//���δ����ѹ��ȥִ����δ���
	{
	motor_time(300,200,200);//�ѹ���
	rt_thread_delay(1000);
		//laser_track(100);
		//rt_thread_delay(1);
	}
	else break;
	} 

//	track2one_point(0,400);
	Tiyi_Laser_L_1point(320);
	motor_time(350,300,300);
	rt_thread_delay(100);
	rt_kprintf("jiaodu1:%d\n",Angle_z);
//	motor_time(120,400,400);
	turn_angle(-28,350);
	rt_thread_delay(100);
	rt_kprintf("jiaodu2:%d\n",Angle_z);
//	slow_acceleration(250,380,300);
	Road_to_Angle(380);
	Up_Platform();
	rt_thread_delay(300);
	turn_angle(-178,350);
}

void route_8_To_7(void)
{
//	turn_angle(180,400);
//	//down_zf(300,2);//�����
//	Down_montain(250);
//	Road_to_time(450,500);//ѭ������ǰ
////	rt_thread_delay(1000);
//	while(1)//����ǰѭ��
//	{
//			rt_thread_delay(1);
//		  Color = Front_WhiteGreen;
//			if(!(f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7])) break;
//			ForwardTrack(305,300);
//		
//	}
//	while(1)
//	{
//		
//		if(f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7]) break;
//		mot(205,200);
//	}
//	 turn_angle(176,400);
	Tiyi_down_8(250,2);
	rt_thread_delay(50);
	slow_acceleration(200,350,600);
	Color = Front_WhiteGreen;
	while(!Infrared2)
	{		
		ForwardTrack(410,400);	
    rt_thread_delay(1);		
	}
//  motor_stop();
//	rt_thread_delay(200);
//	Xiepo_Laser8_time(1400,310);
	while(Infrared2)
{
	Xiepo_LaserTrack_8(310);
	rt_thread_delay(10);	
}
rt_thread_delay(200);	
	while(Infrared2)
{
	Xiepo_LaserTrack_8(310);
	rt_thread_delay(10);	
}
//  motor_stop();
//	while(1);
//		Road_to_time(300,500);
	  slow_acceleration(250,400,500);
		Road_to_Angle(400);//����ϼ��ٴ�
		through_slow_down(255,240,900);//1.8s
		through_slow_down(240,255,900);//1.8s		
		through_slow_down(220,210,1200);//1.8s
//    through_slow_down(220,210,3500);//1.8s
//	  Retarder_plate(2600);
//		through_slow_down(220,200,2800);//1.8s
		Road_to_Angle(300);//����ϰ�

    motor_time(100,0,170);//����˦һ˦
    rt_thread_delay(50);
    motor_time(100,170,0);
//		UP_montain(250,1);
	Tiyi_Up_Mount(250,1);
		motor_stop();
}
void route_7_To_8(void)
{
//		turn_angle(180,400);
	//	down_zf(300,1);//�����
//	down_7();
  Tiyi_down_8(220,1);
	motor_stop();
	slow_acceleration(250,400,400);
//		Road_to_Angle(400);//����ϰ�
		Road_to_Angle(320);
		through_slow_down(240,255,900);//1.8s		
		through_slow_down(255,240,900);//1.8s
		through_slow_down(210,220,1200);//1.8s
//	  through_slow_down(210,220,3700);//1.8s
//  	Retarder_plate(2600);	
		motor_stop();
	rt_thread_delay(100);
	while(!Infrared2)
	{
		Color = Front_WhiteGreen;
		ForwardTrack(350,350);
		rt_thread_delay(1);
	}
		motor_stop();
	rt_thread_delay(50);
//	motor_time(1200,205,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300   1500,205,200
	while(Infrared2)
	{
	Xiepo_LaserTrack_7(310);
	rt_thread_delay(10);
	}
	rt_thread_delay(100);
	slow_acceleration(250,400,300);
	Road_to_Angle(400);
	
	Tiyi_Up_Mount(250,2);
  motor_stop();

}
///////////////////////////
void all_route(void)
{		
int result_image=0;
	int putin=97;
	result_image=get_image(putin);
	while(1)
	{
		
		if(putin==97)
		{
			switch(result_image)
			{
				case 3:route_2_To_3();putin=98;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
				case 4:route_2_To_4();putin=99;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
			}	
		}
		
			else if(putin==98)
			{			
				switch(result_image)
				{
					case 5:route_3_To_5();putin=100;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
					case 6:route_3_To_6();putin=101;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
				}	
			}
		
				else if(putin==99)
			{
			
				switch(result_image)
				{
					case 5:route_4_To_5();putin=100;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
					case 6:route_4_To_6();putin=101;result_image=get_image(putin);rt_thread_delay(400);turn_angle(-178,350);twinkling11111();break;
				}	
			}
			
			else if(putin==100)
			{
			
				switch(result_image)
				{
					case 7:route_5_To_7();turn_angle(-178,350);twinkling11111();route_7_To_8();turn_angle(-178,350);liuleliule_8();break;
					case 8:route_5_To_8();turn_angle(-178,350);twinkling11111();route_8_To_7();turn_angle(-178,350);liuleliule_7();break;
				}
					break;
			}
			else if(putin==101)
			{
			
				switch(result_image)
				{
					case 7:route_6_To_7();rt_thread_delay(300);turn_angle(-178,350);twinkling11111();liuleliule_7();break;
					case 8:route_6_To_8();rt_thread_delay(300);turn_angle(-178,350);twinkling11111();liuleliule_8();break;
				}	
				break;
			}
		}
	
		
	
}
void route_7_To_5(void)
{
		down_zf(300,1);//�����
		Road_to_Angle(400);//����ϰ�
		through_slow_down(220,200,2800);//1.8s		
		Road_to_time(450,800);//ѭ������ǰ
  	while(1)
			{
				rt_thread_delay(1);
				Color = Front_WhiteGreen;
				if(f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7]) break;
		    mot(205,200);
			}
	motor_time(1600,210,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300  1500,205,20
	rt_thread_delay(1000);
	turn_angle_qqb(-90,200,-400);
	motor_time(300,250,250);//��б��
	Road_to_time(450,1000);
	Laser_track_point(450,1);
	motor_time(100,200,200);
	turn_angle(-90,400);
	motor_time(100,200,200);
	Road_to_Angle(450);
	Up_Platform();

	
}

void route_8_To_5(void)
{
	turn_angle(180,400);
	Down_montain(250);
	while(1)
	{
		if(Infrared2) break;
		Color = Front_WhiteGreen;
		ForwardTrack(250,250);
		motor_stop();
	}
	rt_thread_delay(1000);
	motor_time(1200,205,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300   1500,205,200
	rt_thread_delay(100);
	turn_angle_qqb(90,-400,200);
		motor_time(300,250,250);//��б��
		Road_to_time(450,1000);
		Laser_track_point(450,0);
	
	motor_time(100,200,200);
		turn_angle(-90,400);
	motor_time(100,200,200);
		Road_to_Angle(450);
		Up_Platform();
}


void route_7_To_6(void)
{
					down_zf(300,1);//�����
		Road_to_Angle(400);//����ϰ�
		through_slow_down(220,200,2800);//1.8s		
			Road_to_time(450,800);//ѭ������ǰ
	
//	while(1)
//	{
//		if(Angle_y<354&&Angle_y>300) break;
//			motor_time(5000,305,300);
//	}
	while(1)
	{
			rt_thread_delay(1);
		  Color = Front_WhiteGreen;
			if(f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7]) break;
			mot(205,200);
		
	}
	motor_time(1600,210,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300  1500,205,20
	//motor_time(360,300,300);//��⵽��б�º�mot     ����ʱ��Ҫ��
	rt_thread_delay(1000);
	//turn_angle_qqb(-90,600);
			turn_angle_qqb(-90,200,-400);
//	turn_right_angle(400,0);
			motor_time(300,250,250);//��б��
	Road_to_time(450,1000);
	Laser_track_point(450,1);
	motor_time(100,200,200);
	turn_angle(90,400);
	motor_time(100,200,200);
	Laser_track_point(450,0);
	motor_time(100,200,200);
	turn_angle(180,400);
	through_qqb();

}


void route_8_To_6(void)
{
turn_angle(180,400);
	Down_montain(250);

	while(1)
	{
		if(Infrared2) break;
		Color = Front_WhiteGreen;
		ForwardTrack(250,250);
		motor_stop();
	}

	rt_thread_delay(1000);
	motor_time(1200,205,200);//��⵽��б�º�mot     ����ʱ��Ҫ��360,300,300   1500,205,200

	rt_thread_delay(100);
	turn_angle_qqb(90,-400,200);
			motor_time(300,250,250);//��б��
		Road_to_time(450,1000);
		Laser_track_point(450,0);
	motor_time(100,200,200);
		turn_angle(90,400);
	motor_time(100,200,200);
	Laser_track_point(450,0);
	motor_time(100,200,200);
	turn_angle(180,400);
	through_qqb();
	
}