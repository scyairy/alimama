#ifndef __LED_H
#define __LED_H

#include "sys.h"
#include <rtthread.h>

#define LED0 PGout(14)

void LED_init(void);

#endif


