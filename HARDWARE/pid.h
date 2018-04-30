#ifndef __PID_H
#define __PID_H
#include "sys.h"
#include "gizwits_protocol.h"
#include "peripheral.h"
typedef struct
{
	float const_P;
	float const_I;
	float const_D;
}PIDConstStructDef;
typedef struct
{
	float error;
	float last_error;
	float dError;
}diffStructDef;



void pid_IT_Init(u16 period);
void calcFrequency(void);
void calcVolt(void);
#endif
