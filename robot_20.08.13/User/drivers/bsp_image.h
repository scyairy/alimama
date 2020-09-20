#ifndef __BSP_IMAGE_H
#define __BSP_IMAGE_H

#include <rtthread.h>
#include "sys.h"

extern uint16_t image[3];
void Image_Init(void);
uint16_t Send_Receive(uint16_t data);
uint16_t Second_Plat_Image(void);
uint16_t Third_Plat_Image(void);
uint16_t Fourth_Plat_Image(void);
uint16_t Fifth_Plat_Image(void);
uint16_t Sixth_Plat_Image(void);

#endif
