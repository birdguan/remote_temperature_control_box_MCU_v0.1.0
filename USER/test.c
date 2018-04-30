#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"  
#include "adc.h"
#include "key.h"
#include "lsens.h"
#include "usart3.h"
#include "timer.h"
#include "oled.h"
#include "ds18b20.h"
#include "gizwits_protocol.h"
#include "gizwits_product.h"
#include "peripheral.h"
#include "pid.h"

//ALIENTEKս��STM32F103����������Ʋ���ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

//�Զ����÷�Χ
#define AUTOSET_RANGE	80	  		//���÷�ΧΪRGBLEDԭ���ȵİٷֱ�	

//�û�����ǰ�豸״̬�ṹ�� 
dataPoint_t currentDataPoint; 

 
 
 
//��ʼ�������Ƽ��������
void Gizwits_Init(void)			 
{
	TIM3_Int_Init(9,7199);			//1ms������ϵͳ��ʱ
  usart3_init(36,9600);			//WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();					//������Э���ʼ��
}

//������������
//ɨ�谴������ɿ��ء��Զ�������WIFI ��λ/AirLink�ȿ���
void KEY_Task(void)
{
	u8 key;
	key=KEY_Scan(0);
	switch(key)
	{
		case KEY1_PRES: 	//����WIFI AirLinkģʽ
			printf("WIFI����AirLink����ģʽ\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);	//Air-linkģʽ����
			OLED_ShowString(13*6,0,"AirLink",12);
			OLED_Refresh_Gram();
			break;
		case WKUP_PRES: 	//��λWIFI
			printf("WIFI��λ����������������\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);	//��λWIFI
			OLED_ShowString(13*6,0,"RESET  ",12);
			OLED_Refresh_Gram();
			break;
	}
}


int main(void)
{	 
	u8 t;
////  Stm32_Clock_Init(9);	//ϵͳʱ������
	uart_init(72,115200);	//���ڳ�ʼ��Ϊ115200
	delay_init(72);	   	 	//��ʱ��ʼ��  			 	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//��ʼ������ 
	OLED_Init();
	Gizwits_Init();			//��ʼ�������Ƽ�������� 
	peripheral_init();
	pid_IT_Init(20);
	
	OLED_ShowString(0,0,"WIFI_MODE:",12);
	OLED_ShowString(0,12,"DeviceOnOff:",12);
	OLED_ShowString(0,24,"TempSet:",12);
	OLED_ShowString(0,36,"TempNow:",12);
	OLED_ShowString(14*6,48,"@NJFU",12);
	OLED_Refresh_Gram();
	PAout(1) = 0;
	while(DS18B20_Init())
	{
		OLED_ShowString(0,48,"DS18B20 Err",12);
		OLED_Refresh_Gram();
		delay_ms(200);
	}
	OLED_ShowString(0,48,"DS18B20 OK ",12);
	OLED_Refresh_Gram();
	printf("************************\r\n");
	printf("*    �����¿�ϵͳ      *\r\n");
	printf("*         NJFU         *\r\n");
	printf("*         TEST         *\r\n");
	printf("************************\r\n");
	ControlData.CtrlInitTemp = DS18B20_Get_Temp();
	delay_ms(1000);
	ControlData.CtrlInitTemp = DS18B20_Get_Temp();
 	while(1) 
	{		 														     				   
		KEY_Task();
		
		ControlData.CtrlTempNow = DS18B20_Get_Temp();
		if(ControlData.CtrlTempNow > 0)
			OLED_ShowChar(9*6,36,'+',12,1);
		else if(ControlData.CtrlTempNow == 0)
			OLED_ShowChar(9*6,36,' ',12,1);
		else
			OLED_ShowChar(9*6,36,'-',12,1);
		OLED_ShowNum(10*6,36,abs(ControlData.CtrlTempNow/10),3,12);
		OLED_ShowChar(13*6,36,'.',12,1);
		OLED_ShowNum(14*6,36,ControlData.CtrlTempNow%10,1,12);
		OLED_Refresh_Gram();
		
		currentDataPoint.valueTempNow = ControlData.CtrlTempNow/10 + 20;
    gizwitsHandle((dataPoint_t *)&currentDataPoint);//ѭ�����������Э������
		
		
		
		delay_ms(1);
		t++;
		if((t%50)==0)		//500msѭ��һ�Σ�DS0��˸
		{ 
			if(t==150)t=0;
			LED0=!LED0;		//DS0��˸��ʾ��������
		}
	}
}


