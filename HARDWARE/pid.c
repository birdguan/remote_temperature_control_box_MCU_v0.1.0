#include "pid.h"
PIDConstStructDef warming_PIDConstStruct;
PIDConstStructDef cooling_PIDConstStruct;
diffStructDef warming_diffStruct;
diffStructDef cooling_diffStruct;
float warming_frequency;
extern dataPoint_t currentDataPoint;

/************************
*ÊäÈë£ºPIDÖÜÆÚ(ms)
************************/
void pid_IT_Init(u16 period)
{
	u16 arr = period*10 - 1;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM7,ENABLE);
	
	MY_NVIC_Init(2,0,TIM7_IRQn,2);
	
	warming_PIDConstStruct.const_P = 0.02;
	warming_PIDConstStruct.const_I = 0;
	warming_PIDConstStruct.const_D = 0;
	cooling_PIDConstStruct.const_P = 0;
	cooling_PIDConstStruct.const_I = 0;
	cooling_PIDConstStruct.const_D = 0;
	
	warming_frequency = 0.0;
}	



void calcFrequency(void)
{
	warming_diffStruct.error = (float)ControlData.CtrlTempSet - (float)ControlData.CtrlTempNow;
	warming_diffStruct.dError = warming_diffStruct.error - warming_diffStruct.last_error;
	warming_diffStruct.last_error = warming_diffStruct.error;
	warming_frequency = warming_PIDConstStruct.const_P*(float)warming_diffStruct.error
										+ warming_PIDConstStruct.const_D*(float)warming_diffStruct.dError;
	if(warming_frequency > 1.0)
		warming_frequency = 1.0;
	if(warming_frequency <= 0.0)
		warming_frequency =  0.0;
}

void calcVolt(void)
{
	
}

void TIM7_IRQHandler()
{
	
	if(TIM_GetITStatus(TIM7,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
//		printf("PID_Handler\n");
		calcFrequency();
		calcVolt();
		setFrequency();
		setVolt();
	}
		
}
