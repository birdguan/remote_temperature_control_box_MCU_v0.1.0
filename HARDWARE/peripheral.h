#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H
#include "sys.h"
#include "pid.h"
#include "delay.h"
#define warming_arrMAX  999
#define warming_arrMIN  0

void peripheral_init(void);
void setFrequency(void);
void setVolt(void);
#endif
