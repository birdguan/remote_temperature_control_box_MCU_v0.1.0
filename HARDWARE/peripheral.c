#include "peripheral.h"
extern float warming_frequency;
extern dataPoint_t currentDataPoint;
u16 warming_ccr;
GPIO_InitTypeDef GPIO_InitStructure;
void peripheral_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	TIM_InternalClockConfig(TIM2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//heating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//cooling
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PBout(6) = 0;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	PFout(0) = 0;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	PEout(5) = 0;
	
	//1kKz
	TIM_TimeBaseInitStructure.TIM_Period = 999;				//1000000/1000 = 1kHz
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;				//72000000/72 = 1000000
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	//TIM2-CH2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
//	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//	TIM_CtrlPWMOutputs(TIM2, ENABLE);
//	MY_NVIC_Init(2,1,TIM2_IRQn,2);
	TIM_Cmd(TIM2, ENABLE);
}

void setFrequency(void)
{
	//加热模式
	if((ControlData.CtrlTempSet > ControlData.CtrlInitTemp) && (ControlData.CtrlOnOff == 1))
	{
		if(ControlData.CtrlTempSet > ControlData.CtrlTempNow){
			
			warming_ccr = warming_frequency*(float)warming_arrMAX;
			
			TIM_Cmd(TIM2, ENABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			TIM_SetCompare2(TIM2, warming_ccr);
			PBout(6) = 0;
			
			if(ControlData.CtrlTempSet != ControlData.CtrlLastTempSet){
				ControlData.CtrlLastTempSet = ControlData.CtrlTempSet;
				printf("\n加热模式\n");
				printf("warming frequency %f \n", warming_frequency);
				printf("warming ccr %d \n", warming_ccr);
			}
			
		}
	
	}
	//制冷模式
	if((ControlData.CtrlTempSet < ControlData.CtrlInitTemp) && (ControlData.CtrlOnOff == 1))
	{
		if(ControlData.CtrlTempSet < ControlData.CtrlTempNow){
			PEout(5) = 0;
			TIM_Cmd(TIM2, DISABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			PBout(6) = 1;
			
			if(ControlData.CtrlTempSet != ControlData.CtrlLastTempSet){
				ControlData.CtrlLastTempSet = ControlData.CtrlTempSet;
				printf("\n制冷模式\n");		
			}
		}
		

	}
	else
	{
		TIM_Cmd(TIM2, DISABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		PBout(6) = 0;
		PEout(5) = 0;
	}
}
void setVolt(void)
{
	
}
//void TIM2_IRQHandler()
//{
//	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
//	{
//		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
//		printf("TIM2_IRQHandler\n");
//		PEout(5) = !PEout(5);
////		if(TIM2->CCR2 > warming_ccr)
////			PAout(1) = 0;
////		else
////			PAout(1) = 1;
//	}
//}
