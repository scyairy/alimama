#include "track.h"
#include "bsp_grayscale.h"
#include "bsp_motor.h"
#include "bsp_oled.h"
#include "bsp_led.h"
#include "robot_ui.h"
#include "JY61.h"


void ForwardTrack_PID1(rt_int16_t speed)
{
	static int16_t Integral_error,Last_error;
	u8 i;
	int16_t error = 0;         //ƫ��ֵ
	int16_t l_speed,r_speed;			 //�������ٶ�
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
    
    float Kp =0.1;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =12.0;		 //����ϵ����΢��ϵ��
		   
    while(1)
			{

			error=pre_calculate();
				
				rt_kprintf("error:%d",error);
				
			Integral_error += error;
			
			r_speed = (speed+(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
			l_speed = (speed-(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
				

				
			Last_error = error;
	
//////////////////����ٺ����������///////////////////////////
				if(l_speed > (speed+150))
					l_speed = (speed+150);
				if(r_speed > (speed+150))
					r_speed = (speed+150);
				if(l_speed <= 150)
					l_speed = 150;
				if(r_speed <= 150)
					r_speed = 150;
//////////////////����ٺ����������///////////////////////////
				
			  motorr(-l_speed,-r_speed);
			}
}	
double PIDCalc(PID*pp,double NextPoint)
{
	int dError,Error;
//	if(pp->SetPoint>NextPoint)
//		Error=pp->SetPoint-NextPoint;
//	else
		Error=NextPoint-pp->SetPoint;
	pp->SumError+=Error;
	dError=pp->LastError-pp->PrevError;
	pp->PrevError=pp->LastError;
	pp->LastError=Error;
	return (pp->Proportion * Error
				+ pp->Integral * pp->SumError
				+ pp->Derivative * dError
				);
}

double sensor1(void)
{
	rt_uint8_t Forward=0;
	Color = Front_WhiteGreen;
	 rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		Forward=f[3]+f[2]*2+f[1]*4+f[0]*8;
	rt_mutex_release(grayscale_mutex);
	
	rt_thread_delay(1);

	return Forward;
}
double sensor2(void)
{
	rt_uint8_t Forward=0;
	Color = Front_WhiteGreen;
	 rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		Forward=f[4]+f[5]*2+f[6]*4+f[7]*8;
	rt_mutex_release(grayscale_mutex);
	
	rt_thread_delay(1);

	return Forward;
}

void Measuring_Speed(void)
{
	double rOut;
	double rIn;
	
	sPID.Proportion = 1; // Set PID Coefficients
	sPID.Integral = 0.0;
	sPID.Derivative = 0.0;
	sPID.SetPoint = 0;

	rIn=sensor1();
	rOut=PIDCalc(&sPID,rIn);
	OLED_ShowNum(0,1,rIn,4,12);
	OLED_ShowNum(0,0,rOut,4,12);
	
}
//		if(Front_Med && Back_Med)           {mot(l_speed,r_speed);}             // test_1       tese_2
//    else if(Front_Med && Back_Right)    {mot(l_speed*0.87,r_speed*0.87);}   //0.86 0.98
//    else if(Front_Med && Back_Left)     {mot(l_speed*0.93,r_speed*0.83);}   //0.98 0.86

//    else if(Front_Left && Back_Right)   {mot(l_speed*0.68,r_speed*0.81);}   //0.78 0.87   0.75 0.85
//    else if(Front_Left && Back_Med)     {mot(l_speed*0.76,r_speed*0.88);}   //0.85 0.95
//    else if(Front_Left && Back_Left)    {mot(l_speed*0.76,r_speed*0.86);}   //0.82 0.93   0.85 0.93

//    else if(Front_Right && Back_Left)   {mot(l_speed*0.80,r_speed*0.73);}   //0.87 0.78   0.85 0.75
//    else if(Front_Right && Back_Med)    {mot(l_speed*0.88,r_speed*0.77);}   //0.95 0.85
//    else if(Front_Right && Back_Right)  {mot(l_speed*0.8,r_speed*0.75);}   //0.93 0.82   0.93 0.85
void cc_double_track(rt_int16_t l_speed,rt_int16_t r_speed)
{
		if(Front_Med && Back_Med)           {mot(l_speed*0.99,r_speed);}             // test_1       tese_2
    else if(Front_Med && Back_Right)    {mot(l_speed*0.84,r_speed*0.93);}   //0.86 0.98
    else if(Front_Med && Back_Left)     {mot(l_speed*0.91,r_speed*0.86);}   //0.98 0.86

    else if(Front_Left && Back_Right)   {mot(l_speed*0.70,r_speed*0.85);}   //0.78 0.87   0.75 0.85
    else if(Front_Left && Back_Med)     {mot(l_speed*0.76,r_speed*0.84);}   //0.85 0.95
    else if(Front_Left && Back_Left)    {mot(l_speed*0.78,r_speed*0.86);}   //0.82 0.93   0.85 0.93

    else if(Front_Right && Back_Left)   {mot(l_speed*0.85,r_speed*0.72);}   //0.87 0.78   0.85 0.75
    else if(Front_Right && Back_Med)    {mot(l_speed*0.84,r_speed*0.76);}   //0.95 0.85
    else if(Front_Right && Back_Right)  {mot(l_speed*0.86,r_speed*0.78);}   //0.93 0.82   0.93 0.85
    else    {mot(400,400);}
		rt_thread_delay(1);
}
void TY_double_track(rt_int16_t l_speed,rt_int16_t r_speed)
{
//	 rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		if(Front_Med && Back_Med)           {mot(l_speed*0.98,r_speed);}             // test_1       tese_2
    else if(Front_Med && Back_Right)    {mot(l_speed*0.80,r_speed*0.93);}   //0.86 0.98
    else if(Front_Med && Back_Left)     {mot(l_speed*0.88,r_speed*0.86);}   //0.98 0.86

    else if(Front_Left && Back_Right)   {mot(l_speed*0.70,r_speed*0.75);}   //0.78 0.87   0.75 0.85
    else if(Front_Left && Back_Med)     {mot(l_speed*0.72,r_speed*0.80);}   //0.85 0.95
    else if(Front_Left && Back_Left)    {mot(l_speed*0.74,r_speed*0.82);}   //0.82 0.93   0.85 0.93

    else if(Front_Right && Back_Left)   {mot(l_speed*0.75,r_speed*0.72);}   //0.87 0.78   0.85 0.75
    else if(Front_Right && Back_Med)    {mot(l_speed*0.78,r_speed*0.72);}   //0.95 0.85
    else if(Front_Right && Back_Right)  {mot(l_speed*0.80,r_speed*0.76);}   //0.93 0.82   0.93 0.85
    else    {mot(250,250);}
		rt_thread_delay(1);
//		rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
}

void quxianTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	 rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	
		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed*0.99,r_speed);}		
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.99,r_speed);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.99,r_speed);}
	       else  if(f[3]&&f[4])             {mot(l_speed*0.99,r_speed); }
				 else  if(f[3])                   {mot(l_speed*0.99,r_speed); }
				 else  if(f[4])                   {mot(l_speed*0.99,r_speed); }

	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.95,r_speed);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed,r_speed*0.96);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.93,r_speed); }
					else  if(f[4]&&f[5])             {mot(l_speed,r_speed*0.955); }
					else  if(f[2])                   {mot(l_speed*0.92,r_speed); }
					else  if(f[5])                   {mot(l_speed,r_speed*0.95); }
					
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.88,r_speed);}
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed,r_speed*0.90);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.82,r_speed); }
					else  if(f[5]&&f[6])             {mot(l_speed,r_speed*0.86); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.79,r_speed); }
					else  if(f[6]&&f[7])             {mot(l_speed,r_speed*0.83); }
					      else  if(f[0])             {mot(l_speed*0.1,r_speed); }
								else  if(f[1])             {mot(l_speed*0.78,r_speed); }
								else  if(f[6])             {mot(l_speed,r_speed*0.82); }
								else  if(f[7])             {mot(l_speed,r_speed*0.1); }
					
	else  {mot(l_speed*0.88,r_speed*0.88);}	


	
//		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed*0.85,r_speed);}		
//	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.70,r_speed);}
//  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.85,r_speed*0.97);}
//	       else  if(f[3]&&f[4])             {mot(l_speed*0.85,r_speed); }

//	
//	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.67,r_speed);}
//	else if(f[4]&&f[5]&&f[6]) {mot(l_speed,r_speed*0.85);}
//	       else  if(f[2]&&f[3])             {mot(l_speed*0.60,r_speed); }
//	       else  if(f[4]&&f[5])             {mot(l_speed,r_speed*0.86); }
//					
//	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.50,r_speed);}
//  else if(f[5]&&f[6]&&f[7]) {mot(l_speed,r_speed*0.78);}
//	        else  if(f[1]&&f[2])             {mot(l_speed*0.50,r_speed); }
//					else  if(f[5]&&f[6])             {mot(l_speed,r_speed*0.76); }
//					else  if(f[0]&&f[1])             {mot(l_speed*0.45,r_speed); }
//					else  if(f[6]&&f[7])             {mot(l_speed,r_speed*0.77); }
//					      else  if(f[0])             {mot(l_speed*0.30,r_speed); }						
//								else  if(f[7])             {mot(l_speed,r_speed*0.60); }
//					
//	else  {mot(l_speed*0.30,r_speed*0.95);}
	 rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);

	rt_thread_delay(2);

	
}
//#define Front_RRight  (f[7])
//#define Front_Right   (f[6] || f[5])
//#define Front_LLeft 	(f[0])
//#define Front_Left 	  (f[1] || f[2])
//#define Front_Med     (f[3] || f[4])
//#define Front_MMed    (f[2] || f[3] || f[4] || f[5])
//#define Front_edge    (f[0] || f[7])
//#define Front_All     (f[0]&&f[1]&&f[2]&&f[3]&&f[4]&&f[5]&&f[6]&&f[7])
//#define Front_Eigher  (f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7])
void Forward_Track_all(rt_int16_t l_speed,rt_int16_t r_speed)
{
      rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);

if(Front_Med && Back_Med)           		{mot(l_speed,r_speed);}             // test_1       tese_2
		
		
		else if((f[3] && f[4] && f[5]) && (b[3] && b[4]))    {mot(l_speed*0.935,r_speed);}   //0.955 1   0.905 1
    else if((f[2] && f[3] && f[4]) && (b[3] && b[4]))     {mot(l_speed,r_speed*0.935);}   //1 0.955  1 0.905
		
		else if((f[3] && f[4] && f[5]) && (b[3] && b[4] && b[5]))    {mot(l_speed*0.965,r_speed);}   //0.975 1    0.91 1
    else if((f[3] && f[4] && f[5]) && (b[3] && b[4] && b[5]))     {mot(l_speed,r_speed*0.965);}   //1 0.975   1 0.91
		
		else if((f[2] && f[3] && f[4]) && (b[2] && b[3] && b[4]))    {mot(l_speed,r_speed*0.965);}   // 1 0.975	  1 0.91
    else if((f[2] && f[3] && f[4]) && (b[2] && b[3] && b[4]))     {mot(l_speed*0.965,r_speed);}   //0.975 1 	0.91 1
			
    else if(Front_Med && Back_Right)    {mot(l_speed*0.915,r_speed);}   //0.92 1  0.90 1
    else if(Front_Med && Back_Left)     {mot(l_speed,r_speed*0.915);}   //1 0.92  1 0.90
		else if(Front_Left && Back_Med)     {mot(l_speed*0.915,r_speed);}   //1 0.92  1 0.90
		else if(Front_Right && Back_Med)    {mot(l_speed,r_speed*0.915);}   //0.92 1  0.90 1
		
		else if(Front_Right && Back_Right)  {mot(l_speed,r_speed*0.945);}   //1 0.93
		else if(Front_Left && Back_Left)    {mot(l_speed*0.945,r_speed);}   //0.93 1
		else if(Front_Left && Back_Right)   {mot(l_speed*0.65,r_speed*0.77);}   //0.75 0.87
		else if(Front_Right && Back_Left)   {mot(l_speed*0.77,r_speed*0.65);}   //0.87 0.75
		else if(Front_Left && Back_LLeft)    {mot(l_speed*0.895,r_speed*0.865);}		//0.74 0.865
		else if(Front_Right && Back_RRight)  {mot(l_speed*0.865,r_speed*0.895);}		//0.865 0.74
		
		else if(Front_Med && Back_LLeft)     {mot(l_speed*0.98,r_speed*0.765);}  	//0.98 0.765
		else if(Front_Med && Back_RRight)     {mot(l_speed*0.765,r_speed*0.98);}	//0.765 0.98
		
		
		else if(Front_RRight )              {mot(l_speed*0.8,r_speed*0.4);}
    else if(Front_LLeft )               {mot(l_speed*0.4,r_speed*0.8);}

    else if(Front_RRight )              {mot(l_speed*0.8,r_speed*0.76);}
    else if(Front_LLeft )               {mot(l_speed*0.76,r_speed*0.8);}

    else    {mot(400,400);}

      rt_mutex_release(grayscale_mutex);
        rt_thread_delay(2);
}

void SingleForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	if ((!f[2])&&f[3]&&f[4]&&(!f[5]))                 {mot(l_speed,       r_speed);}
    else if(f[2]&&f[3]&&f[4]&&f[5])                   {mot(l_speed,       r_speed);}
    //deviation:+-1
    else if(f[2]&&f[3]&&f[4])       mot(l_speed*0.94,  r_speed);
    else if(f[3]&&f[4]&&f[5])       mot(l_speed,       r_speed*0.94);
    //deviation:+-3
    else if(f[1]&&f[2]&&f[3])       mot(l_speed*0.89,  r_speed);
    else if(f[4]&&f[5]&&f[6])       mot(l_speed,       r_speed*0.90);
    //deviation:+-2
    else if(f[2]&&f[3])             mot(l_speed*0.915,  r_speed);
    else if(f[4]&&f[5])             mot(l_speed,       r_speed*0.92);
    //deviation:+-4
    else if(f[1]&&f[2])             mot(l_speed*0.85,   r_speed);
    else if(f[5]&&f[6])             mot(l_speed,       r_speed*0.85);
    //deviation:+-5
    else if (f[1])                  mot(l_speed*0.78,   r_speed);
    else if (f[6])                  mot(l_speed,       r_speed*0.78);
    //deviation:+-6
    else if (f[0]&&f[1])            mot(l_speed*0.7,   r_speed);
    else if (f[6]&&f[7])            mot(l_speed,       r_speed*0.7);
    
    else                            mot(l_speed*0.7,   r_speed*0.7);

	
	rt_mutex_release(grayscale_mutex);
	rt_thread_delay(2);
}
 void ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	   rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed,r_speed);}		
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.97,r_speed*0.98);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.98,r_speed*0.97);}
  else  if(f[3]&&f[4])      {mot(l_speed,r_speed);}
	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.90,r_speed*0.93);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.93,r_speed*0.90);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.91,r_speed*0.93); }
					else  if(f[4]&&f[5])             {mot(l_speed*0.93,r_speed*0.91); }
			          else  if(f[3])             {mot(l_speed*0.99,r_speed); }
				        else  if(f[4])             {mot(l_speed,r_speed*0.99); }
					
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.74,r_speed*0.84);}
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.84,r_speed*0.74);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.78,r_speed*0.86); }
					else  if(f[5]&&f[6])             {mot(l_speed*0.86,r_speed*0.78); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.55,r_speed*0.84); }
					else  if(f[6]&&f[7])             {mot(l_speed*0.84,r_speed*0.55); }
					

								
				      	else  if(f[2])             {mot(l_speed*0.86,r_speed*0.88); }
					      else  if(f[5])             {mot(l_speed*0.88,r_speed*0.86); }	
								else  if(f[1])             {mot(l_speed*0.60,r_speed*0.88); }
								else  if(f[6])             {mot(l_speed*0.88,r_speed*0.60); }	
					      else  if(f[0])             {mot(l_speed*0.50,r_speed*0.95); }
								else  if(f[7])             {mot(l_speed*0.95,r_speed*0.50); }	

			
	else  {mot(l_speed*0.88,r_speed*0.88);}	
		
//
	rt_mutex_release(grayscale_mutex);
	rt_thread_delay(1);	
}

 void Mount_ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	   rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed,r_speed);}

	else if(f[1]&&f[2]&&f[3]&&f[4])   {mot(l_speed*0.965,r_speed*0.98);}
	else if(f[3]&&f[4]&&f[5]&&f[6])   {mot(l_speed*0.98,r_speed*0.965);}
	
	else if(f[0]&&f[1]&&f[2]&&f[3])   {mot(l_speed*0.90,r_speed*0.93);}
	else if(f[4]&&f[5]&&f[6]&&f[7])   {mot(l_speed*0.93,r_speed*0.90);}
	
	
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.97,r_speed*0.98);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.98,r_speed*0.97);}
  else  if(f[3]&&f[4])      {mot(l_speed,r_speed);}
	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.90,r_speed*0.93);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.93,r_speed*0.90);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.91,r_speed*0.93); }
					else  if(f[4]&&f[5])             {mot(l_speed*0.93,r_speed*0.91); }
			          else  if(f[3])             {mot(l_speed*0.99,r_speed); }
				        else  if(f[4])             {mot(l_speed,r_speed*0.99); }
					
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.74,r_speed*0.84);}
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.84,r_speed*0.74);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.78,r_speed*0.86); }
					else  if(f[5]&&f[6])             {mot(l_speed*0.86,r_speed*0.78); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.55,r_speed*0.84); }
					else  if(f[6]&&f[7])             {mot(l_speed*0.84,r_speed*0.55); }
					

								
				      	else  if(f[2])             {mot(l_speed*0.86,r_speed*0.88); }
					      else  if(f[5])             {mot(l_speed*0.88,r_speed*0.86); }	
								else  if(f[1])             {mot(l_speed*0.60,r_speed*0.88); }
								else  if(f[6])             {mot(l_speed*0.88,r_speed*0.60); }	
					      else  if(f[0])             {mot(l_speed*0.50,r_speed*0.95); }
								else  if(f[7])             {mot(l_speed*0.95,r_speed*0.50); }	

			
	else  {mot(l_speed*0.88,r_speed*0.88);}	
		
//
	rt_mutex_release(grayscale_mutex);
	rt_thread_delay(1);	
}
void highspeed_ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	   rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed,r_speed);}		
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.96,r_speed*0.99);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.98,r_speed*0.97);}
  else  if(f[3]&&f[4])      {mot(l_speed,r_speed);}
	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.92,r_speed*0.98);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.98,r_speed*0.95);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.94,r_speed*0.98); }
					else  if(f[4]&&f[5])             {mot(l_speed*0.97,r_speed*0.95); }
			          else  if(f[3])             {mot(l_speed*0.98,r_speed); }
				        else  if(f[4])             {mot(l_speed,r_speed*0.99); }
					
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.84,r_speed*0.94);}
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.96,r_speed*0.86);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.86,r_speed*0.92); }
					else  if(f[5]&&f[6])             {mot(l_speed*0.92,r_speed*0.84); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.80,r_speed*0.92); }
					else  if(f[6]&&f[7])             {mot(l_speed*0.94,r_speed*0.83); }					

								
				      	else  if(f[2])             {mot(l_speed*0.92,r_speed*0.97); }
					      else  if(f[5])             {mot(l_speed*0.98,r_speed*0.94); }	
								else  if(f[1])             {mot(l_speed*0.84,r_speed*0.90); }
								else  if(f[6])             {mot(l_speed*0.94,r_speed*0.82); }	
					      else  if(f[0])             {mot(l_speed*0.75,r_speed*0.90); }
								else  if(f[7])             {mot(l_speed*0.90,r_speed*0.75); }	

			
	else  {mot(l_speed*0.88,r_speed*0.88);}	

	rt_mutex_release(grayscale_mutex);
	rt_thread_delay(1);	
}

void White_Black_Track(rt_int16_t l_speed,rt_int16_t r_speed)
{
	   rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed,r_speed);}

	else if(f[1]&&f[2]&&f[3]&&f[4])   {mot(l_speed*0.92,r_speed*0.97);}
	else if(f[3]&&f[4]&&f[5]&&f[6])   {mot(l_speed*0.97,r_speed*0.92);}
	
	else if(f[0]&&f[1]&&f[2]&&f[3])   {mot(l_speed*0.85,r_speed*0.92);}
	else if(f[4]&&f[5]&&f[6]&&f[7])   {mot(l_speed*0.92,r_speed*0.85);}
	
	
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.89,r_speed*0.97);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.97,r_speed*0.89);}
  else  if(f[3]&&f[4])      {mot(l_speed*0.9,r_speed*0.9);}
	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.78,r_speed*0.93);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.93,r_speed*0.78);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.78,r_speed*0.93); }
					else  if(f[4]&&f[5])             {mot(l_speed*0.93,r_speed*0.78); }
			          else  if(f[3])             {mot(l_speed*0.98,r_speed); }
				        else  if(f[4])             {mot(l_speed,r_speed*0.98); }
					
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.64,r_speed*0.88);}
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.88,r_speed*0.64);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.62,r_speed*0.90); }
					else  if(f[5]&&f[6])             {mot(l_speed*0.90,r_speed*0.62); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.55,r_speed*0.88); }
					else  if(f[6]&&f[7])             {mot(l_speed*0.91,r_speed*0.55); }
					

								
				      	else  if(f[2])             {mot(l_speed*0.70,r_speed*0.88); }
					      else  if(f[5])             {mot(l_speed*0.88,r_speed*0.70); }	
								else  if(f[1])             {mot(l_speed*0.60,r_speed*0.90); }
								else  if(f[6])             {mot(l_speed*0.90,r_speed*0.60); }	
					      else  if(f[0])             {mot(l_speed*0.50,r_speed*0.95); }
								else  if(f[7])             {mot(l_speed*0.97,r_speed*0.50); }	

			
	else  {mot(l_speed*0.80,r_speed*0.80);}	
		
//
	rt_mutex_release(grayscale_mutex);
	rt_thread_delay(1);	
}
//		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed,r_speed);}		
//	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.96,r_speed*0.99);}
//  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.98,r_speed*0.97);}
//  else  if(f[3]&&f[4])      {mot(l_speed,r_speed);}
//	
//	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.91,r_speed);}
//	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.98,r_speed*0.96);}
//	        else  if(f[2]&&f[3])             {mot(l_speed*0.88,r_speed*0.98); }
//					else  if(f[4]&&f[5])             {mot(l_speed*0.97,r_speed*0.93); }
//			          else  if(f[3])             {mot(l_speed*0.97,r_speed); }
//				        else  if(f[4])             {mot(l_speed,r_speed*0.99); }
//					
//	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.84,r_speed*0.94);}
//  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.98,r_speed*0.90);}
//	        else  if(f[1]&&f[2])             {mot(l_speed*0.78,r_speed*0.94); }
//					else  if(f[5]&&f[6])             {mot(l_speed*0.96,r_speed*0.84); }
//					else  if(f[0]&&f[1])             {mot(l_speed*0.75,r_speed*0.94); }
//					else  if(f[6]&&f[7])             {mot(l_speed*0.94,r_speed*0.83); }
//					

//								
//				      	else  if(f[2])             {mot(l_speed*0.92,r_speed*0.99); }
//					      else  if(f[5])             {mot(l_speed*0.99,r_speed*0.95); }	
//								else  if(f[1])             {mot(l_speed*0.78,r_speed*0.94); }
//								else  if(f[6])             {mot(l_speed*0.94,r_speed*0.82); }	
//					      else  if(f[0])             {mot(l_speed*0.75,r_speed*0.95); }
//								else  if(f[7])             {mot(l_speed*0.95,r_speed*0.75); }	

//			
//	else  {mot(l_speed*0.88,r_speed*0.88);}	

void SIX_Track(rt_int16_t l_speed,rt_int16_t r_speed)
{
	   rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	
		if(f[2]&&f[3]&&f[4]&&f[5])   {mot(l_speed*0.80,r_speed);}		
	else if(f[2]&&f[3]&&f[4]) {mot(l_speed*0.80,r_speed*0.98);}
  else if(f[3]&&f[4]&&f[5]) {mot(l_speed*0.78,r_speed);}
	       else  if(f[3]&&f[4])             {mot(l_speed*0.80,r_speed); }


	
	else if(f[1]&&f[2]&&f[3]) {mot(l_speed*0.65,r_speed);}
	else if(f[4]&&f[5]&&f[6]) {mot(l_speed*0.70,r_speed*0.86);}
	        else  if(f[2]&&f[3])             {mot(l_speed*0.70,r_speed*0.90); }
					else  if(f[4]&&f[5])             {mot(l_speed*0.78,r_speed*0.90); }
			 	  else  if(f[3])                   {mot(l_speed*0.68,r_speed); }
				  else  if(f[4])                   {mot(l_speed*0.70,r_speed*0.92); }	
					else  if(f[2])                   {mot(l_speed*0.65,r_speed); }
					else  if(f[5])                   {mot(l_speed*0.65,r_speed*0.90); }

				 
	else if(f[0]&&f[1]&&f[2]) {mot(l_speed*0.55,r_speed);}	
  else if(f[5]&&f[6]&&f[7]) {mot(l_speed*0.55,r_speed*0.85);}
	        else  if(f[1]&&f[2])             {mot(l_speed*0.60,r_speed); }
					else  if(f[5]&&f[6])             {mot(l_speed*0.70,r_speed*0.85); }
					else  if(f[0]&&f[1])             {mot(l_speed*0.55,r_speed*0.90); }
					else  if(f[6]&&f[7])             {mot(l_speed*0.60,r_speed*0.70); }
					      else  if(f[0])             {mot(l_speed*0.50,r_speed*0.9); }
								else  if(f[1])             {mot(l_speed*0.52,r_speed*0.89); }
								else  if(f[6])             {mot(l_speed*0.60,r_speed*0.75); }
								else  if(f[7])             {mot(l_speed*0.60,r_speed*0.70); }
					
	else  {mot(l_speed*0.60,r_speed*0.85);}	
	
	rt_mutex_release(grayscale_mutex);
//	rt_thread_delay(2);	
}

void BackTrack(rt_int16_t l_speed,rt_int16_t r_speed)
{
	rt_mutex_take(grayscale_mutex,RT_WAITING_FOREVER);
	l_speed=-l_speed;
	r_speed=-r_speed;
	
	if(b[2]&&b[3]&&b[4]&&b[5])   {mot(l_speed,r_speed);}		
	else if(b[2]&&b[3]&&b[4]) {mot(l_speed*0.97,r_speed*0.98);}
  else if(b[3]&&b[4]&&b[5]) {mot(l_speed*0.98,r_speed*0.97);}
	       else  if(b[3]&&b[4])             {mot(l_speed,r_speed); }
				 else  if(b[3])                   {mot(l_speed*0.98,r_speed); }
				 else  if(b[4])                   {mot(l_speed,r_speed); }

	
	else if(b[1]&&b[2]&&b[3]) {mot(l_speed*0.90,r_speed*0.93);}
	else if(b[4]&&b[5]&&b[6]) {mot(l_speed*0.93,r_speed*0.90);}
	        else  if(b[2]&&b[3])             {mot(l_speed*0.91,r_speed*0.93); }
					else  if(b[4]&&b[5])             {mot(l_speed*0.93,r_speed*0.91); }

					
	else if(b[0]&&b[1]&&b[2]) {mot(l_speed*0.74,r_speed*0.84);}
  else if(b[5]&&b[6]&&b[7]) {mot(l_speed*0.84,r_speed*0.74);}
	        else  if(b[1]&&b[2])             {mot(l_speed*0.78,r_speed*0.86); }
					else  if(b[5]&&b[6])             {mot(l_speed*0.86,r_speed*0.78); }
					else  if(b[0]&&b[1])             {mot(l_speed*0.55,r_speed*0.84); }
					else  if(b[6]&&b[7])             {mot(l_speed*0.84,r_speed*0.55); }
				      	else  if(b[2])             {mot(l_speed*0.86,r_speed*0.88); }
				      	else  if(b[5])             {mot(l_speed*0.88,r_speed*0.86); }	
								else  if(b[1])             {mot(l_speed*0.60,r_speed*0.88); }
								else  if(b[6])             {mot(l_speed*0.88,r_speed*0.60); }	
					      else  if(b[0])             {mot(l_speed*0.50,r_speed*0.95); }
								else  if(b[7])             {mot(l_speed*0.95,r_speed*0.50); }	
					
	else  {mot(l_speed*0.88,r_speed*0.88);}
	
	rt_mutex_release(grayscale_mutex);
	
	rt_thread_delay(2);

}

void RoadTrack(rt_uint16_t time_ms,rt_uint16_t speed)		//ǰ�ҶȰ���Ѱ��
{
	Color = Front_WhiteGreen;
	if(time_ms <= 1) return;
	while(time_ms--)
	{
		ForwardTrack(speed,speed);
	}
	motor_stop();
}

void BoardTrack(rt_uint16_t time_ms,rt_uint16_t speed)		//ǰ�ҶȰ׻�Ѱ��
{
	Color = Front_WhiteBlack;
	if(time_ms <= 1) return;
	while(time_ms--)
	{
		ForwardTrack(speed,speed);
		rt_thread_delay(2);
	}
	motor_stop();
}

void HRoadTrack(rt_uint16_t time_ms,rt_uint16_t speed)		//��ҶȰ���Ѱ��   xiugaiguo
{
	Color = Back_WhiteGreen;
	if(time_ms <= 1) return;
	while(time_ms--)
	{
		if(b[0]) break;		
		BackTrack(speed,speed);
	}
	motor_stop();
}

void Tiyi_BackTrack(rt_uint16_t time200,rt_uint16_t speed)		//����ϵ��ҶȰ���Ѱ��//1000/1
{
	Color = Back_WhiteGreen;
	while(time200--){
	BackTrack(speed,speed);}
		while(!laser_l)
	{
		BackTrack(speed,speed);
	}
	motor_stop();
}

void HBoardTrack(rt_uint16_t time_ms,rt_uint16_t speed)		//��ҶȰ׻�Ѱ��
{
	Color = Back_WhiteBlack;
	if(time_ms <= 1) return;
	while(time_ms--)
	{
		BackTrack(speed,speed);
	}
	motor_stop();
}

void motor_time(rt_uint16_t time,rt_uint16_t L_speed,rt_uint16_t R_speed)//1000/1
{
	time=time;
	while(time--)
	{
		mot(L_speed,R_speed);
		rt_thread_delay(1);
	}
	   motor_stop();
	
}

void Up_Platform(void)			//��ƽ̨  �Ĺ�
{
//	OLED_Clear();
//	while(1)
//	{
//		if((Angle_x==0) || Angle_x==1 || Angle_x==359 || Angle_x==2 || Angle_x==358)  break;
//		else
//		{
//			Color = Front_WhiteBlack;
//			ForwardTrack(250,250);
//		}
//		rt_thread_delay(1);
//	}
	motor_time(700,250,250);//750
//	turn_angle(-1,200);//��ƽ̨������ת2��
//	motor_stop();
//		while(1)
//	{
////		if((Angle_x==0) || Angle_x==1 || Angle_x==359 || Angle_x==2 || Angle_x==358)  break;
//		if(Angle_x>355 || Angle_x<5)	break;
//		else
//		{
//			Color = Front_WhiteBlack;
//		ForwardTrack(280,280);
//		}
//		rt_thread_delay(1);
//	}
	motor_stop();

}

void Down_Platform(void)		//��ƽ̨
{
	int16_t ax0;
	ax0=Angle_x;

//	OLED_Clear();
//	OLED_ShowNum(60,0,ax0,4,14);
	while(1)//ƽ̨��
	{
//		show_angle();
		if((Angle_x>5) && (Angle_x<20)) break;
		else mot(250,250);
		rt_thread_delay(1);
	}
	rt_thread_delay(20);	
			Color = Front_WhiteBlack;	
	while(1)//��
	{
//		show_angle();
		if(Angle_x==ax0 || (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360) || 
			(Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360)||
		(Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360))
    {break;}
		else
		{
			ForwardTrack(250,250);
		}
		rt_thread_delay(1);	
	}
	motor_stop();
}


//ǰ�Ҷ��ܵ�ѭ�����壬�Ҷȼ��
void Road_to_Board(rt_uint16_t speed)   
{
    while(1)
	{
		if((Angle_x>340) && (Angle_x<350)) break;//��⵽�ϰ�����ѭ��
		Color = Front_GreenBlack;
		if(f[0]&&f[1]&&f[2]&&f[3]&&f[4]&&f[5]&&f[6]&&f[7]) break;
		 else
		{
			Color = Front_WhiteGreen;
			ForwardTrack(speed,speed);
		}
		rt_thread_delay(1);		
	}
}
//ǰ�Ҷ��ܵ�ѭ�����壬�Ƕȼ��
void Road_to_Angle(rt_uint16_t speed)    //�Ĺ�
{
Color = Front_WhiteGreen;	
if(speed<390)
{	
    while(1)
	{
		if((Angle_x>300) && (Angle_x<350))break;
		 else
		{	
//				ForwardTrack_PID1(speed);
			ForwardTrack(speed,speed);			
		}	
		rt_thread_delay(1);	
	}
}
else
{
    while(1)
	{
		if((Angle_x>300) && (Angle_x<350))break;
		 else
		{	
//			ForwardTrack_PID1(speed);
			highspeed_ForwardTrack(speed,speed);			
		}	
		rt_thread_delay(1);	
	}



}
	
	
	
	motor_stop();
}
void PID_to_Angle(rt_uint16_t speed)//scy��
{
	static int16_t Integral_error,Last_error;
	u8 i;
	int16_t error = 0;         //ƫ��ֵ
	int16_t l_speed,r_speed;			 //�������ٶ�
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
    
    float Kp =0.1;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =12.0;		 //����ϵ����΢��ϵ��
	Color = Front_WhiteGreen;
    while(1)
			{
				
				if((Angle_x>300) && (Angle_x<350))break;
							error=pre_calculate();
				
						rt_kprintf("error:%d",error);
				
					Integral_error += error;
			
					r_speed = (speed+(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
					l_speed = (speed-(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
				

				
					Last_error = error;
	
//////////////////����ٺ����������///////////////////////////
				if(l_speed > (speed+150))
					l_speed = (speed+150);
				if(r_speed > (speed+150))
					r_speed = (speed+150);
				if(l_speed <= 150)
					l_speed = 150;
				if(r_speed <= 150)
					r_speed = 150;
//////////////////����ٺ����������///////////////////////////
				
					motorr(-l_speed,-r_speed);
		rt_thread_delay(1);	
	
			}
			
			
			motor_stop();
}
void Road_to_Angle_8(rt_uint16_t speed)    //scy��
{
Color = Front_WhiteGreen;	
if(speed<390)
{	
    while(1)
	{
		if((Angle_x>300) && (Angle_x<355))break;
		 else
		{	
//				ForwardTrack_PID1(speed);
			ForwardTrack(speed,speed);			
		}	
		rt_thread_delay(1);	
	}
}
else
{
    while(1)
	{
		if((Angle_x>300) && (Angle_x<355))break;
		 else
		{	
//			ForwardTrack_PID1(speed);
			highspeed_ForwardTrack(speed,speed);			
		}	
		rt_thread_delay(1);	
	}



}
	
	
	
	motor_stop();
}

//��Ҷ��ܵ�Ѱ������
void HRoad_to_Board(rt_uint16_t speed)
{
	while(1)
	{
		Color = Back_GreenBlack; 
		if(b[0]&&b[1]&&b[2]&&b[3]&&b[4]&&b[5]&&b[6]&&b[7]) break;
		 else
		{
			Color = Back_WhiteGreen;
			BackTrack(speed,speed);
		}				 
	}
}

void Gray_to_Road(rt_uint16_t speed)
{
	while(1)
	{
		Color = Front_WhiteBlack;
		if(f[0]&&f[1]&&f[2]&&f[3]&&f[4]&&f[5]&&f[6]&&f[7]) break;
//		if((!f[0])&&(!f[1])&&(!f[2])&&(!f[5])&&(!f[6])&&(!f[7]))break;
		else 
		{
			Color = Front_WhiteBlack;
			SingleForwardTrack(speed,speed);
		}
		rt_thread_delay(1);
	}
}

void Run_Mount(rt_uint16_t speed)			//����庯��,�Ƕȼ��
{
	Color = Front_WhiteBlack;
	while(1)
	{
		
		if(Angle_x==0 || Angle_x==1 || Angle_x==2 || Angle_x==3 || Angle_x==357 || Angle_x==359 || Angle_x==358)break;
		else
			ForwardTrack(speed,speed);
	}
	motor_time(100,200,200);
	motor_stop();

}
void Run_Mount_7(rt_uint16_t speed)			//����庯��,�Ƕȼ��
{
	while(1)
	{
		Color = Front_WhiteGreen;
		if(Angle_x==0 || Angle_x==1 || Angle_x==2 || Angle_x==359 || Angle_x==358)break;
		else
			ForwardTrack(speed,speed);
	}
	motor_time(100,200,200);
	motor_stop();

}

void through_slow_down(rt_uint16_t L_speed,rt_uint16_t R_speed,rt_uint16_t slow_times)		//���ٰ���ź���    5000  10
{
slow_times=slow_times/3;
	while(slow_times!=1)
	{
			Color = Front_WhiteBlack;
			ForwardTrack(L_speed,R_speed);
		  rt_thread_delay(1);
		  slow_times--;		
	}
	motor_stop();
}

void SIX_laser_track(u16 speed)
{
    if(laser_l==0&&laser_r==1)
        mot(speed*0.80,speed);
    else if(laser_l==1&&laser_r==0)
        mot(speed,speed*0.8);
    else
        mot(speed,speed);
}

//�ź��ѭ��
void laser_track(u16 speed)
{
    if(laser_l==0&&laser_r==1)
        mot(speed*0.90,speed);
    else if(laser_l==1&&laser_r==0)
        mot(speed,speed*0.9);
    else
        mot(speed,speed);
}

void Xiepo_LaserTrack_8(u16 speed)
{
    if(laser_l==0&&laser_r==1)
        mot(speed*0.75,speed);
    else if(laser_l==1&&laser_r==0)
        mot(speed,speed*0.83);
    else
        mot(speed*0.95,speed);
}	
void Xiepo_LaserTrack_7(u16 speed)
{
    if(laser_l==0&&laser_r==1)
        mot(speed*0.85,speed);
    else if(laser_l==1&&laser_r==0)
        mot(speed,speed*0.75);
    else
        mot(speed,speed*0.95);//0.95
}	
void Xiepo_LaserTrack_7_home(u16 speed)
{
    if(laser_l==0&&laser_r==1)
        mot(speed*0.85,speed);
    else if(laser_l==1&&laser_r==0)
        mot(speed,speed*0.75);
    else
        mot(speed,speed*0.88);//0.95
}	

//ѭ����·��
void track2one_point(u8 point,u16 speed)
{
    if(speed<=500)  Color = Front_WhiteGreen;
		else 	Color = Front_Back_WhiteGreen;	
    while(!f[point])
    {
       			if(speed<=500){
						ForwardTrack(speed,speed);
						rt_thread_delay(1);
					}
					else{
						cc_double_track(speed,speed);//�³ɴ���˫�Ҷ�
						rt_thread_delay(1);
					}
    }
    motor_stop();
}

//ѭ�����ڶ���·��
void track2two_point(u8 point,u8 point1,u16 speed)
{
		
    Color = Front_WhiteGreen;
    while(!f[point])
    {
        ForwardTrack(speed,speed);
        rt_thread_delay(5);
    }
		
		while(!f[point1])
		{
		   ForwardTrack(speed,speed);
       rt_thread_delay(5);
		}
		
    motor_stop();
}
//ѭ����ת��
void track2three_point(u8 point,u8 point1,u8 *ass,u16 speed)
{
		
    Color = Front_WhiteGreen;
    while(!f[point])
    {
        ForwardTrack(speed,speed);
        rt_thread_delay(5);
    }
		while(!f[point1])
		{
		   ForwardTrack(speed*0.8,speed*0.8);
       rt_thread_delay(5);
		}
		while(!(f[ass[0]]&&f[ass[1]]&&f[ass[2]]))
		{
			ForwardTrack(speed*0.8,speed*0.8);
       rt_thread_delay(5);
		}
		  rt_thread_delay(100);//��⵽·������0.1s
    motor_stop();
}

//ͨ������
void though_brige(u16 speed)
{
		while(!(Angle_x<5||Angle_x>355))
    {
        laser_track(speed);
        rt_thread_delay(5);
    }
   motor_time(100,330,330);

    while((Angle_x<5||Angle_x>355))
    {
        laser_track(speed);
        rt_thread_delay(5);
    }
//		motor_stop();
//		while(1);
    motor_time(200,330,330);
    while((Angle_x<28&&Angle_x>2))
    {
        laser_track(230);
        rt_thread_delay(5);
    }

//		while((Angle_x<5||Angle_x>355))
//    {
//        laser_track(250);
//        rt_thread_delay(5);
//    }

}
void JY61_bridge(u16 speed)
{
	int up=0;
	int down=0;
	if(Angle_x>350&&Angle_x<355	)  up=1;
	if(Angle_x>5&&Angle_x<20)   down=1;
    while(1)
    {
				if(Angle_x>350&&Angle_x<355	)  up=1;
				if(Angle_x>5&&Angle_x<20)   down=1;
				if(up&&down) break;
        
				JY61_track(speed);
    }

}
void JY61_track(u16 speed)
{
				if(Angle_z>2&&Angle_z<10){
				mot(speed,speed*0.85);
				 rt_thread_delay(200);
				}
				if(Angle_z>350&&Angle_z<358){					
				mot(speed*0.85,speed);	
				 rt_thread_delay(200);
				}
				else if(Angle_z<=2&&Angle_z>=358)
						mot(speed,speed);

}

void JY61_laser(u16 speed)
{
	 while(!(Angle_x<5||Angle_x>355))
    {
        laser_track(speed);
        rt_thread_delay(5);
			while((laser_l==1&&laser_r==1)&&(Angle_z>5||Angle_z<355))
			{
					if(Angle_z>3&&Angle_z<20)
						mot(speed,speed*0.75);
					else if(Angle_z<357&&Angle_z>340)
						mot(speed*0.75,speed);					
			}
			motor_time(200,300,300);
		}	
//    motor_time(200,300,300);
    while((Angle_x<5||Angle_x>355))
    {
        laser_track(speed);
        rt_thread_delay(5);
			while((laser_l==1&&laser_r==1)&&(Angle_z>5||Angle_z<355))
			{
					if(Angle_z>3&&Angle_z<20)
						mot(speed,speed*0.75);
					else if(Angle_z<357&&Angle_z>340)
						mot(speed*0.75,speed);					
			}
			motor_time(200,300,300);
    }
     //   motor_time(200,300,300);
    while(!(Angle_x<5||Angle_x>355))
    {
        laser_track(speed);
        rt_thread_delay(5);
			while((laser_l==1&&laser_r==1)&&(Angle_z>5||Angle_z<355))
			{
					if(Angle_z>3&&Angle_z<20)
						mot(speed,speed*0.75);
					else if(Angle_z<357&&Angle_z>340)
						mot(speed*0.75,speed);					
			}
				motor_time(200,300,300);
    }
		
}

//ѭ��һ��ʱ��
void PID_to_time(rt_uint16_t speed,rt_uint32_t ms)
{
	static int16_t Integral_error,Last_error;
	u8 i;
	int16_t error = 0;         //ƫ��ֵ
	int16_t l_speed,r_speed;			 //�������ٶ�
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
    
    float Kp =0.1;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =12.0;		 //����ϵ����΢��ϵ��
//	ms=ms/3;
    while(ms--) 
			{
			error=pre_calculate();
				
				rt_kprintf("error:%d",error);
				
			Integral_error += error;
			
			r_speed = (speed+(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
			l_speed = (speed-(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
				

				
			Last_error = error;
	
//////////////////����ٺ����������///////////////////////////
				if(l_speed > (speed+150))
					l_speed = (speed+150);
				if(r_speed > (speed+150))
					r_speed = (speed+150);
				if(l_speed <= 150)
					l_speed = 150;
				if(r_speed <= 150)
					r_speed = 150;
//////////////////����ٺ����������///////////////////////////
				
			  motorr(-l_speed,-r_speed);
		
			}
			motor_stop();
}
void Road_to_time(rt_uint16_t speed,rt_uint32_t ms) //1000=1s  
{
    ms=ms/3;
    while(1)
	{
     if(speed<500)  Color = Front_WhiteGreen;
		else 	Color = Front_Back_WhiteGreen;	
		if(ms--)
        {
					if(speed<500){
					///	Color = Front_WhiteGreen;
						ForwardTrack(speed,speed);
						rt_thread_delay(1);
					}
					else{
				//		Color = Front_Back_WhiteGreen;	
						cc_double_track(speed,speed);//�³ɴ���˫�Ҷ�
						rt_thread_delay(1);
					}
        }
        else
		{
			break;
		}
	}
	 motor_stop();
}

void through_door(rt_uint16_t speed,rt_uint32_t door_ms)//���1000/1
{
    door_ms=door_ms/3;
	Color = Front_WhiteBlack;
    while(1)
	{
    
		if(door_ms-->1)
        {
					ForwardTrack(speed,speed);
          rt_thread_delay(1);
        }
        else
		{
			break;
		}
	}
}

void xiepo_turn_angle(int l_speed,int r_speed)
{

    rt_thread_delay(25);


//    if((f[0]|f[1]||f[2]))//��б��
//    {
//        while(!((f[3]||f[4])&&(b[4]||b[3])))//�����趨�ٶ�תһ��
//        {
//            mot(-l_speed,r_speed);
//            rt_thread_delay(5);
//        }
//        motor_stop();
//        rt_thread_delay(10);
//    
//    }
//		else if((f[5]|f[6]||f[7]))
//		{
//        while(!((f[3]||f[4])&&(b[4]||b[3])))//�����趨�ٶ�תһ��
//        {
//            mot(l_speed,-r_speed);
//            rt_thread_delay(5);
//        }
//        motor_stop();
//        rt_thread_delay(10);
//    
//    }
		

}

void Road_to_Angle_y(rt_uint16_t l_speed,rt_uint16_t r_speed)   
{
	Color = Front_WhiteGreen;	
    while(!Infrared2)//
	{
		SIX_Track(l_speed,r_speed);		//r_speed*1.11
		rt_thread_delay(1);	
	}
	motor_stop();
}

void turn_Angle_y(rt_uint16_t speed)
{
		int time=224*4.0;
	while(1)
	{
		if(Angle_y==0||Angle_y==359||Angle_y==358||Angle_y==1) break;
		else mot(-speed*0.5,speed);
	}

}
void through_qqb(void)//�����ΰ�
{
	int time;
	  Tiyi_BackTrack(500,400);		//����ϵ��ҶȰ���Ѱ��
		rt_thread_delay(200);
		turn_angle(18,400);
		Road_to_Angle_y(300,300);//1100  1150//*1.015
		rt_thread_delay(100);	
		//turn_angle(16,500);//����ת��   22
		turn_angle(17,250);//����ת��   22
		rt_thread_delay(300);
//		motor_time(100,0,100);//����˦һ˦
//    rt_thread_delay(50);
//    motor_time(100,100,0);
	while(1)//ȷ������
	{
		if(Angle_x<356&&Angle_x>330) break;//��⵽С���Ƕ����̱����ϰ�
		laser_track(250);
	}

	while(1){
		//����ʱ��  224
		if (time==0) break;
		laser_track(310);
		rt_thread_delay(1);
		time--;
	}
	motor_stop();//
	rt_thread_delay(1200);
	//////////////////////////���ϵڶ���
	while(1){
	if(Angle_x>330&&Angle_x<350)//���δ����ѹ��ȥִ����δ���
	{
	motor_time(300,195,200);//�ѹ���
	rt_thread_delay(1000);
		//laser_track(100);
		//rt_thread_delay(1);
	}
	else break;
	} 
rt_thread_delay(300);
while(Infrared2)
{	
	mot(180,200);		
}	
motor_stop();
//while(1);
rt_thread_delay(200);
	turn_angle(26,300);
	rt_thread_delay(200);
//    while(1)//ѭ��
//	{
//		static int time=230*2;
//		if(time==0) break;
//        Color = Front_WhiteGreen;

//			quxianTrack(300*0.6,300);
//       //rt_thread_delay(1);
//        time--;
//	}
	//track2one_point(0,300);
	//Road_to_time(300,250);
	while(!laser_l)
	{
		 Color = Front_WhiteGreen;
			SIX_Track(300,325);
	}
	//Laser_track_point(300,0);
	//if(!(f[4]&&f[3]&&!f[5]))turn_right_angle(150);
	//motor_time(250,180,400);
	//turn_angle(5,200);
	motor_time(150,300,300);
	rt_thread_delay(100);
	turn_angle(90,350);
	rt_thread_delay(100);
	Road_to_Angle(350);	
	Up_Platform();//��6��
	motor_time(100,300,300);
	
}
void through_qqb_two(void)//��Ҫ�޸Ĳ���
{
//	turn_angle(178,400);
	Down_Platform();
//	track2one_point(0,300);
	//motor_time(75,400,400);
	while(!laser_l)
	{
		 Color = Front_WhiteGreen;
			ForwardTrack(330,330);
	}
	motor_time(250,200,200);	
	rt_thread_delay(150);
	turn_angle(90,350);
	    while(1)//ѭ��,�Ƕȼ��
	{
		rt_kprintf("X:%d\nY:%d\n",Angle_x,Angle_y);
		if(Angle_x>300&&Angle_x<353)  break;
//		if(!Infrared2)  break;
      Color = Front_WhiteGreen;
			SIX_Track(280,325);
	}
    motor_stop();
	  rt_thread_delay(200);
		turn_angle(14,300);
	  rt_thread_delay(100);
//		motor_time(100,0,100);//����˦һ˦
//    rt_thread_delay(50);
//    motor_time(100,100,0);	
	  through_slow_down(200,210,1350);//�����ٰ壬�ر�ע��ʱ��
	  through_slow_down(210,200,1350);//�����ٰ壬�ر�ע��ʱ��
//		through_slow_down(200,200,950);//1.8s
	  motor_stop();
	  rt_thread_delay(300);
		turn_angle(16,300);//15,400
		//track2one_point(7,400);
	
//		 int time=240*2.38;   //235
		 while(!laser_r)//�¼��ٰ��ѭ��
	{
				Color = Front_WhiteGreen;
				SIX_Track(300,300*1.05);//1.12
				rt_thread_delay(1);
	}
    motor_time(400,280,300);
//	while(!laser_l)
//	{	
//       Color = Front_WhiteGreen;		
//	     ForwardTrack(330,330);
//	}
	
	motor_stop();
	rt_thread_delay(200);
  turn_angle(20,350);
//	
	motor_stop();

//	Road_to_Angle(350);//���������
//	int time=240*4;
//	  while(1)//�¼��ٰ��ѭ��
//	{

//		if(time==0) break;
//		if(laser_r==1) break;
//				Color = Front_WhiteGreen;
//				quxianTrack(300*0.80,300);
//				rt_thread_delay(1);
//        time--;
//	}
	
//		int times=240*10;
//	  while(1)//�Һ������ѭ��һ�ξ���
//	{
//	
//		if(times==0) break;
//		if(laser_r==1) 	{LED1=0;break;}
//				Color = Front_WhiteGreen;
//				quxianTrack(300*0.80,300);
//				rt_thread_delay(1);
//        times--;
//	}

//	int outtime=700;
//	while(1)
//	{
//			if(laser_l==1) break;
//			if(outtime==0||Infrared2)
//			{
//				HRoadTrack(900,300);
//				turn_angle(-30,300);
//				rt_thread_delay(1000);
//				Road_to_time(300,300);
//				break;
//			}
//			Color = Front_WhiteGreen;
//			ForwardTrack(300,300);
//			rt_thread_delay(1);
//			outtime--;
//	}
	
}


void turn_angle_qqb(int ang,int l_speed,int r_speed)//ת�����ҿ�spped��������
{
    int angle0,angle1,angle2;//angle0Ϊ��ʼ�ǣ�angle1ΪĿ��Ƕȣ�angle2Ϊ�״�ת�ǣ�Ϊ��ʹת�Ǹ�����ȣ���������ʽת����
//    use_compass=1;
    rt_thread_delay(25);
    angle0=Angle_z;
    angle1=(angle0+ang+360)%360;
    angle2=(angle0+ang/2+360)%360;

        while(!(angle2==Angle_z||angle2==Angle_z-1||angle2==Angle_z+1||angle2==Angle_z+2))//�����趨�ٶ�תһ��
        {
            mot(l_speed,r_speed);
            rt_thread_delay(5);
        }
        motor_stop();
        rt_thread_delay(10);
        while(!(angle1==Angle_z||angle1==Angle_z-1||angle1==Angle_z+1))//��100���ٶ�ת��ָ���Ƕ�
        {
            mot(l_speed*0.5,r_speed*0.5);
            rt_thread_delay(5);
        }
    
//    use_compass=0;
}

void turn_right_angle(void)
{
  rt_uint16_t flag=0; 
   Color = Front_WhiteGreen;
    rt_thread_delay(25);
		while(!((f[3]||f[4])))//�����趨�ٶ�תһ��
		{
			if(f[5]||f[6]||f[7])
			{
				mot(150,0);
				rt_thread_delay(5);
				flag=1;
			}
			else if(f[0]||f[1]||f[2])
			{   
				mot(0,150);
				rt_thread_delay(5);
                flag=2;
			}
			
		}
		if(flag==1)
		{
		  turn_angle(2,150);
		}
		else if(flag==2)
		{
		  turn_angle(-2,150);
		}
		rt_thread_delay(10);

}

void down_zf(u16 speed,u16 mode)
{
	rt_uint16_t flag=0;
	if(mode==1)//һ��ƽ̨
	{
		int16_t ax0;
	ax0=Angle_x;

	OLED_Clear();
	OLED_ShowNum(60,0,ax0,4,14);
	while(1)//ƽ̨��
	{
		show_angle();
		if((Angle_x>5) && (Angle_x<20)) break;
		else mot(300,300);
		rt_thread_delay(1);
	}
	while(1)//��
	{
		show_angle();
		if(Angle_x==ax0 || (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360) || (Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360))break;
		else
		{
			Color = Front_WhiteBlack;
			ForwardTrack(300,300);
		}
		rt_thread_delay(1);	
	}
//		rt_uint16_t flag=0;
}
	if(mode==2)//����ƽ̨
	{
		while(1)
		{

				Road_to_time(200,1000+flag*1000);
				
				while(1)
				{
					if(Angle_z<5||Angle_z>355) break;
					Color = Front_WhiteBlack;
					ForwardTrack(speed,speed);		
				}
				if(flag==1) break;
				flag=1;
		}
	}
	motor_stop();
}

void back_to_last_point(int16_t wrong_angle,int16_t right_angle,int16_t time,int16_t speed)
{
	//rt_thread_delay(30);
	int celect_angle[1000],i=0;
	double average=0,sum_angle=0;
	int a=1;//cishu
	Color = Front_WhiteGreen;
	while(1)
	{
		if(time<=0) break;
		else if(a>=100) break;//caiji100cishuju
		
		celect_angle[a]=Angle_z;
		ForwardTrack(speed,speed);
		//rt_thread_delay(1);
		a++;
		time--;		
		rt_thread_delay(10);		

	}
	motor_stop();
	for(i=0;i<100;i++)
	{
		sum_angle=sum_angle+celect_angle[i];
	}
	average=sum_angle/100;
	if(average<wrong_angle+10&&average>wrong_angle+10)
	{
		motor_stop();
		HRoadTrack(400,400);//��Ҷ�	295 ��ҶȺ����޸Ĺ�
		motor_time(200,-400,-400);//
		turn_angle(right_angle-wrong_angle,400);
	}
	else if(average<right_angle+6&&average>right_angle+6) 
	{}
	
}

void PID_Laser_L_1point(u16 speed)
{
		static int16_t Integral_error,Last_error;
	u8 i;
	int16_t error = 0;         //ƫ��ֵ
	int16_t l_speed,r_speed;			 //�������ٶ�
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
    
    float Kp =0.1;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =12.0;		 //����ϵ����΢��ϵ��
	Color = Front_WhiteGreen;
    while(1)
			{
				
				if(laser_l)break;
							error=pre_calculate();
				
						rt_kprintf("error:%d",error);
				
					Integral_error += error;
			
					r_speed = (speed+(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
					l_speed = (speed-(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
				

				
					Last_error = error;
	
//////////////////����ٺ����������///////////////////////////
				if(l_speed > (speed+150))
					l_speed = (speed+150);
				if(r_speed > (speed+150))
					r_speed = (speed+150);
				if(l_speed <= 150)
					l_speed = 150;
				if(r_speed <= 150)
					r_speed = 150;
//////////////////����ٺ����������///////////////////////////
				
					motorr(-l_speed,-r_speed);
		rt_thread_delay(1);	
	
			}
			
			
			motor_stop();
}
void PID_Laser_R_1point(u16 speed)
{
		static int16_t Integral_error,Last_error;
	u8 i;
	int16_t error = 0;         //ƫ��ֵ
	int16_t l_speed,r_speed;			 //�������ٶ�
	int All_Channel = 0;   //�Ҷ�ֵ�ܺ�
	int All_Result = 0;    //�������ĻҶ��ܺ�
	int Finall_Result = 0; //���մ���ֵ
    
    float Kp =0.1;          //����ϵ��              600_0.1_0_12
	float Ki = 0.0;
    float Kd =12.0;		 //����ϵ����΢��ϵ��
	Color = Front_WhiteGreen;
    while(1)
			{
				
				if(laser_r)break;
							error=pre_calculate();
				
						rt_kprintf("error:%d",error);
				
					Integral_error += error;
			
					r_speed = (speed+(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
					l_speed = (speed-(error*Kp+Integral_error*Ki+(error-Last_error)*Kd));
				

				
					Last_error = error;
	
//////////////////����ٺ����������///////////////////////////
				if(l_speed > (speed+150))
					l_speed = (speed+150);
				if(r_speed > (speed+150))
					r_speed = (speed+150);
				if(l_speed <= 150)
					l_speed = 150;
				if(r_speed <= 150)
					r_speed = 150;
//////////////////����ٺ����������///////////////////////////
				
					motorr(-l_speed,-r_speed);
		rt_thread_delay(1);	
	
			}
			
			
			motor_stop();
}
void Tiyi_Laser_L_1point(u16 speed)
{
   Color = Front_WhiteGreen;
	 while(!laser_l)
	{	
//			ForwardTrack_PID1(speed);
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}
	motor_stop();
}
void Tiyi_Laser_R_1point(u16 speed)
{
   Color = Front_WhiteGreen;
	 while(!laser_r)
	{		
//			ForwardTrack_PID1(speed);
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}
	motor_stop();
}

void Tiyi_Laser_L_2point(u16 speed)
{
   Color = Front_WhiteGreen;
	 while(!laser_l)
	{									
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}
	rt_thread_delay(200);	
	while(!laser_l)
	{									
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}		
	motor_stop();
}

void Tiyi_Laser_R_2point(u16 speed)
{
   Color = Front_WhiteGreen;
	 while(!laser_r)
	{									
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}
	rt_thread_delay(200);
	while(!laser_r)
	{									
			ForwardTrack(speed,speed);//���ٵ��Ҷ�
	}		
	motor_stop();
}

void Laser_track_point(u16 speed,u16 l_r)
{
	u8 whichone=2;

	 while(1)
	{		if(speed>=500)
			{
				Color = Front_Back_WhiteGreen;	
				cc_double_track(speed,speed);//�³ɴ���˫�Ҷ�
			}
			else
			{	
			Color = Front_WhiteGreen;	
			ForwardTrack(speed,speed);
			}
			if(l_r==0) whichone=laser_l;//�������ѭ����
			else whichone=laser_r;//�������ѭ����
			if(whichone==1) break;
		else{
			if(speed>=500)
			{
				Color = Front_Back_WhiteGreen;	
				cc_double_track(speed,speed);//�³ɴ���˫�Ҷ�
			}
			else
			{
	
			Color = Front_WhiteGreen;	
			ForwardTrack(speed,speed);//���ٵ��Ҷ�

			}
		}
    
	}
	motor_stop();
}

void slow_acceleration(rt_uint32_t uint_speed,rt_uint32_t end_speed,rt_uint32_t ms)
{
	    rt_uint32_t mms;
    ms/=6;

    mms=ms;
    Color = Front_WhiteGreen;
    for(;mms>0;mms--)
    {
        ForwardTrack(   uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))),
                        uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))));//Forward_Track_all // ForwardTrack
//        rt_kprintf("\n%d",uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))));
        rt_thread_delay(1);
    }
    motor_stop();
}

void Tiyi_accelerate(rt_uint32_t uint_speed,rt_uint32_t end_speed,rt_uint32_t ms)//��ʼ�ٶȣ�ĩ�ٶȣ�ʱ��
{
	    rt_uint32_t mms;
    ms/=6;

    mms=ms;
    Color = Front_Back_WhiteGreen;
	if(uint_speed<=end_speed)
		{
    for(;mms>0;mms--)
    {
        TY_double_track(   uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))),
                        uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))));//Forward_Track_all // ForwardTrack
//        rt_kprintf("\n%d",uint_speed+((rt_uint32_t)((end_speed-uint_speed)*((1-(float)mms/ms)))));
        rt_thread_delay(3);
    }
    motor_stop();
	}
		else {
		    for(;mms>0;mms--)
    {
        TY_double_track(   uint_speed-((rt_uint32_t)((uint_speed-end_speed)*((1-(float)mms/ms)))),
                        uint_speed-((rt_uint32_t)((uint_speed-end_speed)*((1-(float)mms/ms)))));//Forward_Track_all // ForwardTrack

        rt_thread_delay(1);
    }
    motor_stop();
		
		
		
		}
}

void twinkling11111(void)	//��˸
{
	LED1=1;
	LED1=!LED1;
	rt_thread_delay(300);
	LED1=!LED1;
	rt_thread_delay(300);
	LED1=!LED1;
	rt_thread_delay(300);
	LED1=!LED1;
	LED1=1;
}

void UP_montain(u16 speed,u16 mode)
{      
	
	rt_uint16_t flag=0;
		while(1)                           //����庯��
	{    	  	
		if((Angle_x==0) || Angle_x==1 || Angle_x==359 || Angle_x==2 || Angle_x==358)  
		 {
		    if(mode==1) break;
			else if(mode==2&&flag ==0)
			{  
        Road_to_Angle(300);
				while(1)
				{
			  if((Angle_x==0) || Angle_x==1 || Angle_x==359 || Angle_x==2 || Angle_x==358) 
			  {
				flag=1;
			    break;
			  }
			
       else
			{				
         Color = Front_WhiteBlack;// Front_WhiteGreen
			   ForwardTrack(speed,speed);	
			}					 
		  }		
			}
			  else if(flag==1)	break;	
		 }
		else
		{
			Color = Front_WhiteBlack;
			ForwardTrack(speed,speed);
		}
		rt_thread_delay(1);
	} 
	motor_time(100,200,200);//ȷ���ܵ�����м�

}




void Down_montain(rt_uint16_t speed)
{
	while(1)
	{
		Color = Front_WhiteBlack;
		if(Angle_x>7 && Angle_x<90)break;
		else
			ForwardTrack(250,250);
	}
	while(1)
	{
		Color = Front_WhiteBlack;
		if(Angle_x<5 || Angle_x>355)break;
		else
			ForwardTrack(speed,speed);
	}
	while(1)
	{
		Color = Front_WhiteBlack;
		if(Angle_x>7 && Angle_x<90)break;
		else
			ForwardTrack(250,250);
	}
	while(1)
	{
		Color = Front_WhiteBlack;
		if(Angle_x<5 || Angle_x>355)break;
		else
			ForwardTrack(speed,speed);
	}
}

void down_7(void)
{
	int16_t ax0;
	int time=200;
		int times=150;
	ax0=Angle_x;


	OLED_Clear();
	OLED_ShowNum(60,0,ax0,4,14);
	while(1)//ƽ̨��
	{
	//	show_angle();
		//if((Angle_x>5) && (Angle_x<20)) break;
		if(times==0) break;
		else mot(250,240);
		rt_thread_delay(1);
	}
	while(1)
	{
		if(time==0) break;
			Color = Front_WhiteGreen;
			ForwardTrack(250,240);
				rt_thread_delay(1);
		time--;
	}
		rt_thread_delay(1000);
	while(1)//��
	{
		show_angle();
		if(Angle_x>=30&&Angle_x<=240) break;
		else
		{
			Color = Front_WhiteGreen;
			ForwardTrack(250,240);
		}
		rt_thread_delay(1);	

}
	}

	void Laser_time(rt_uint16_t time_ms,rt_uint16_t speed)
{
				while(time_ms--)          
			{
				laser_track(300);
				rt_thread_delay(1);			
			}	

}
void Xiepo_Laser8_time(rt_uint16_t xiepo_time,rt_uint16_t speed)
{
				while(xiepo_time!=1)          
			{
				Xiepo_LaserTrack_8(speed);
				xiepo_time--;
				rt_thread_delay(1);			
			}	
			motor_stop();

}
void Xiepo_Laser7_time(rt_uint16_t xiepo_time,rt_uint16_t speed)
{
				while(xiepo_time!=1)          
			{
				Xiepo_LaserTrack_7_home(speed);
				xiepo_time--;
				rt_thread_delay(1);			
			}	
			motor_stop();

}



void Tiyi_down_8(u16 speed,u16 mode)//mode=1,һ��ƽ̨//mode=2����ƽ̨
{
	rt_uint16_t flag=0;
	int16_t ax0;
	ax0=Angle_x;
	OLED_Clear();
	OLED_ShowNum(60,0,ax0,4,14);
	if(mode==1)//һ��ƽ̨
	{	

	while(1)//ƽ̨��
	{
		show_angle();
		if((Angle_x>5) && (Angle_x<28)) break;
		else mot(250,250);
		rt_thread_delay(1);
	}
	while(1)//��
	{
		show_angle();
		if(Angle_x==ax0 || (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360)
			|| (Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360)
		|| (Angle_x==(ax0+3)%360) || (Angle_x==(ax0-3+360)%360))
		{break;}
		else
		{
			Color = Front_WhiteBlack;
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}
}
//////////////////////////////////////////////
//////////////////////////////////////////////////
	if(mode==2)//����ƽ̨
	{
	while(1)//ƽ̨��
	{
//		show_angle();
		rt_kprintf("angx:%d\n",Angle_x);
		if((Angle_x>5) && (Angle_x<28)) break;
		else mot(220,220);
		rt_thread_delay(1);
	}
	while(1)//��һ����
	{
//		show_angle();
		rt_kprintf("angx:%d\n",Angle_x);
				if(Angle_x==ax0 || (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360)
			|| (Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360)
		|| (Angle_x==(ax0+3)%360) || (Angle_x==(ax0-3+360)%360))
		{break;}
		else
		{
			Color = Front_WhiteBlack;
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}
//////////////////////////////
	while(1)//��ɽ��
		{rt_kprintf("angx:%d\n",Angle_x);
		if((Angle_x>5) && (Angle_x<28)) break;
		else //����ƽ̨�ϣ��Ͱ׻�Ѱ��
		{
			Color = Front_WhiteBlack;
			White_Black_Track(speed,speed);		
		}			
	}
	while(1)//�ڶ�����
	{
//		show_angle();
		rt_kprintf("angx:%d\n",Angle_x);
				if(Angle_x==ax0 || (Angle_x==(ax0+2)%360) || (Angle_x==(ax0-2+360)%360)
			|| (Angle_x==(ax0+1)%360) || (Angle_x==(ax0-1+360)%360)
		|| (Angle_x==(ax0+3)%360) || (Angle_x==(ax0-3+360)%360))
		{break;}
		else
		{
			Color = Front_WhiteBlack;
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}	
	}
	motor_stop();
}

void Tiyi_Up_Mount(u16 speed,u16 mode)//mode=1,һ��ƽ̨//mode=2����ƽ̨
{

	Color = Front_WhiteBlack;
	if(mode==1)//һ��ƽ̨
	{	
	while(1)//��
	{
		show_angle();
    if(Angle_x==0 || Angle_x==1 || Angle_x==2|| Angle_x==3 || Angle_x==359|| Angle_x==356 || Angle_x==357)
		{break;}
		else
		{
			
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}
}
//////////////////////////////////////////////
//////////////////////////////////////////////////
	if(mode==2)//����ƽ̨
	{

	while(1)//�����һ��ƽ̨
	{
		rt_kprintf("ang_x:%d\n",Angle_x);
if(Angle_x==0 || Angle_x==1 || Angle_x==2|| Angle_x==3 || Angle_x==359|| Angle_x==356 || Angle_x==357|| Angle_x==356)
		{break;}
		else
		{
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}
//////////////////////////////
	while(1)//��ɽ��
	{
		if((Angle_x>300) && (Angle_x<350))break;
		else //����ƽ̨�ϣ��Ͱ׻�Ѱ��
		{
			White_Black_Track(speed,speed);		
		}			
	}
	while(1)//�ڶ�ƽ̨��
	{
				rt_kprintf("ang_x:%d\n",Angle_x);
		if(Angle_x==0 || Angle_x==1 || Angle_x==2|| Angle_x==3 || Angle_x==359|| Angle_x==356 || Angle_x==357)
		{break;}
		else
		{
			White_Black_Track(speed,speed);
		}
		rt_thread_delay(1);	
	}	
	}
	motor_time(150,200,200);//ȷ���ܵ�����м�
}



void Retarder_plate(rt_uint16_t slow_times)		//���ٰ�    5000  10
{

	while(slow_times!=1)
	{
			if(Angle_y<358&&Angle_y>350)
			{mot(180,200);}
			else if(Angle_y<10&&Angle_y>0)
			{mot(190,180);}
			else{mot(180,195);}
		  rt_thread_delay(1);
//			rt_kprintf("%d\n",Angle_y);
		  slow_times--;		
	}
	motor_stop();
}

void SIX_Retarder_plate(rt_uint16_t slow_times)		//���ٰ�    5000  10
{

	while(slow_times!=1)
	{
			if(Angle_y<358&&Angle_y>350)
			{mot(180,220);}
			else if(Angle_y<10&&Angle_y>0)
			{mot(190,190);}
			else{mot(180,210);}
		  rt_thread_delay(1);
//			rt_kprintf("%d\n",Angle_y);
		  slow_times--;		
	}
	motor_stop();
}
void Road_To_platform(rt_uint16_t speed)		//���ٰ�    5000  10
{
Color = Front_Back_WhiteGreen;	

    while(1)
	{
		if((Angle_x>300) && (Angle_x<350))break;
		 else
		{			
		TY_double_track(speed,speed);			
		}	
		rt_thread_delay(1);	
	}

	motor_stop();
}




