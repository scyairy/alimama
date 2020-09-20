#include <rtthread.h>
#ifndef __OPERATION_H__
#define __OPERATION_H__

enum FRAME
{
	RobotRun = 1,
    GrayScale,
	Laser,
	Compass,
	Motor,
	ShotDown,
	Image,
	Second_Platform,
	Third_Platform,
	Fourth_Platform,
	Fifth_Platform,
	Sixth_Platform,
	Gray_Front,
	Gray_Back,  
    Gray_Bool  
	
};





rt_err_t operation(void);
void RobotRun_Operation(void);
void GrayFront_Operation(void);
void GrayBack_Operation(void);
void Laser_Operation(void);
void Compass_Operation(void);
void Motor_Operation(void);
void Bool_Operation(void);
void ShotDown_Operation(void);

void Second_image(void);
void Third_image(void);
void Fourth_image(void);
void Fifth_image(void);
void Sixth_image(void);


extern rt_mailbox_t key_value_mb;
extern uint8_t run_flag;
//extern rt_mutex_t run_mutex;

extern struct rt_thread operation_thread;
extern struct rt_thread key_thread;
#endif
