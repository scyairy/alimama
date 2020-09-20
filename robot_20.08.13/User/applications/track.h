#ifndef __TRACK_H
#define __TRACK_H	

#include <rtthread.h>
#include <stm32f4xx.h>
#include <finsh.h>

typedef struct PID
{
	double SetPoint;
	
	double Proportion;
	double Integral;
	double Derivative;
	
	float LastError;
	float PrevError;
	float SumError;
}	PID;

static PID sPID;
static PID*sptr=&sPID;


//#define Front_RRight  (f[7] || f[6])
//#define Front_Right   (f[7] || f[6] || f[5])
//#define Front_LLeft 	(f[0] || f[1])
//#define Front_Left 	  (f[0] || f[1] || f[2])
#define Front_RRight  (f[7])
#define Front_Right   (f[6] || f[5])
#define Front_LLeft 	(f[0])
#define Front_Left 	  (f[1] || f[2])
#define Front_Med     (f[3] || f[4])
#define Front_MMed    (f[2] || f[3] || f[4] || f[5])
#define Front_edge    (f[0] || f[7])
#define Front_All     (f[0]&&f[1]&&f[2]&&f[3]&&f[4]&&f[5]&&f[6]&&f[7])
#define Front_Eigher  (f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7])
//#define Front_RRight  (f[7])
//#define Front_Right   (f[6] || f[5])
//#define Front_LLeft 	(f[0])
//#define Front_Left 	  (f[1] || f[2])
//#define Front_Med     (f[3] || f[4])
//#define Front_MMed    (f[2] || f[3] || f[4] || f[5])
//#define Front_edge    (f[0] || f[7])
//#define Front_All     (f[0]&&f[1]&&f[2]&&f[3]&&f[4]&&f[5]&&f[6]&&f[7])
//#define Front_Eigher  (f[0]||f[1]||f[2]||f[3]||f[4]||f[5]||f[6]||f[7])
//#define Back_RRight   (b[0] || b[1])
//#define Back_Right    (b[0] || b[1] || b[2])
//#define Back_LLeft 	  (b[7] || b[6])
//#define Back_Left  	  (b[7] || b[6] || b[5])
#define Back_LLeft   (b[0])
#define Back_Left   (b[1] || b[2])
#define Back_RRight 	  (b[7])
#define Back_Right  	  (b[5] || b[6])
#define Back_Med		  (b[3] || b[4])
#define Back_MMed     (b[2] || b[3] || b[4] || b[5])
#define Back_edge     (b[0] || b[7])
#define Back_All      (b[0]&&b[1]&&b[2]&&b[3]&&b[4]&&b[5]&&b[6]&&b[7])
#define Back_Eigher		(b[0]||b[1]||b[2]||b[3]||b[4]||b[5]||b[6]||b[7])
double PIDCalc(PID*pp,double NextPoint);
double sensor1(void);
double sensor2(void);
void Measuring_Speed(void);
void ForwardTrack_PID1(rt_int16_t speed);
void ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed);
void BackTrack(rt_int16_t l_speed,rt_int16_t r_speed);
void SingleForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed);
void Forward_Track_all(rt_int16_t l_speed,rt_int16_t r_speed);

void RoadTrack(rt_uint16_t time_ms,rt_uint16_t speed);
void BoardTrack(rt_uint16_t time_ms,rt_uint16_t speed);
void HRoadTrack(rt_uint16_t time_ms,rt_uint16_t speed);
void HBoardTrack(rt_uint16_t time_ms,rt_uint16_t speed);
void Road_to_Board(rt_uint16_t speed);
void HRoad_to_Board(rt_uint16_t speed);

void Gray_to_Road(rt_uint16_t speed);
void through_slow_down(rt_uint16_t L_speed,rt_uint16_t R_speed,rt_uint16_t times);	//减速板和门函数

void Down_Platform(void);
void Up_Platform(void);
void Road_to_Angle_8(rt_uint16_t speed);
void PID_to_Angle(rt_uint16_t speed);
void laser_track(u16 speed);//激光循迹
void track2one_point(u8 point,u16 speed);//循迹到路口
void though_brige(u16 speed);//过桥
void Road_to_Angle(rt_uint16_t speed);  //改过
void Road_to_time(rt_uint16_t speed,rt_uint32_t ms);//循迹一段时间
void JY61_bridge(u16 speed);
void JY61_track(u16 speed);
void track2two_point(u8 point,u8 point1,u16 speed);////循迹到第二个路口
void track2three_point(u8 point,u8 point1,u8 *a,u16 speed);
void JY61_laser(u16 speed);//JY61和激光共同循迹
void through_door(rt_uint16_t speed,rt_uint32_t ms);
void xiepo_turn_angle(int l_speed,int r_speed);

void turn_Angle_y(rt_uint16_t speed);
void motor_time(rt_uint16_t time,rt_uint16_t L_speed,rt_uint16_t R_speed);
void Run_Mount(rt_uint16_t speed);		//登珠峰函数,角度检测
void through_qqb(void);//过6号的跷跷板
void quxianTrack(rt_int16_t l_speed,rt_int16_t r_speed);//圆圈循迹
void turn_angle_qqb(int ang,int _speed,int r_speed);//根据灰度转指定角度
void turn_right_angle();//根据灰度转角
void down_zf(u16 speed,u16 mode);//下珠峰
void back_to_last_point(int16_t wrong_angle,int16_t right_angle,int16_t time,int16_t speed);
void through_qqb_two(void);//6号平台后半段
void Laser_track_point(u16 speed,u16 l_r);
void cc_double_track(rt_int16_t l_speed,rt_int16_t r_speed);
void slow_acceleration(rt_uint32_t uint_speed,rt_uint32_t end_speed,rt_uint32_t ms);
void twinkling11111(void);
void UP_montain(u16 speed,u16 mode);
void Down_montain(rt_uint16_t speed);
void Run_Mount_7(rt_uint16_t speed);	//登珠峰函数,角度检测
void down_7(void);
void Laser_time(rt_uint16_t time_ms,rt_uint16_t mode);
void Road_to_Angle_y(rt_uint16_t l_speed,rt_uint16_t r_speed) ;  

void SIX_laser_track(u16 speed);
void SIX_Track(rt_int16_t l_speed,rt_int16_t r_speed);


void Tiyi_accelerate(rt_uint32_t uint_speed,rt_uint32_t end_speed,rt_uint32_t ms);//初始速度，末速度，时间
void Tiyi_down_8(u16 speed,u16 mode);//mode=1,一阶平台//mode=2二阶平台


void Retarder_plate(rt_uint16_t slow_times);		//减速板    5000  10
void highspeed_ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed);
void PID_Laser_L_1point(u16 speed);
void PID_Laser_R_1point(u16 speed);
void Tiyi_Laser_R_1point(u16 speed);
void Tiyi_Laser_R_2point(u16 speed);
void Tiyi_Laser_L_2point(u16 speed);
void Tiyi_Laser_L_1point(u16 speed);
void Tiyi_BackTrack(rt_uint16_t time200,rt_uint16_t speed);		//激光断点后灰度白绿寻迹
void Tiyi_Up_Mount(u16 speed,u16 mode);//mode=1,一阶平台//mode=2二阶平台
void White_Black_Track(rt_int16_t l_speed,rt_int16_t r_speed);

void Xiepo_Laser8_time(rt_uint16_t time_ms,rt_uint16_t speed);
void Xiepo_Laser7_time(rt_uint16_t time_ms,rt_uint16_t speed);
void Xiepo_LaserTrack_8(u16 speed);
void Xiepo_LaserTrack_7(u16 speed);
void Xiepo_LaserTrack_7_home(u16 speed);

void SIX_Retarder_plate(rt_uint16_t slow_times);

void TY_double_track(rt_int16_t l_speed,rt_int16_t r_speed);
void Road_To_platform(rt_uint16_t speed);
void Mount_ForwardTrack(rt_int16_t l_speed,rt_int16_t r_speed);


void PID_to_time(rt_uint16_t speed,rt_uint32_t ms);
#endif
