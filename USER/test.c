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

//ALIENTEK战舰STM32F103开发板机智云测试实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

//自动设置范围
#define AUTOSET_RANGE	80	  		//设置范围为RGBLED原亮度的百分比	

//用户区当前设备状态结构体 
dataPoint_t currentDataPoint; 

 
 
 
//初始化机智云及相关外设
void Gizwits_Init(void)			 
{
	TIM3_Int_Init(9,7199);			//1ms机智云系统计时
  usart3_init(36,9600);			//WIFI初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();					//机智云协议初始化
}

//按键处理任务
//扫描按键，完成开关、自动调整、WIFI 复位/AirLink等控制
void KEY_Task(void)
{
	u8 key;
	key=KEY_Scan(0);
	switch(key)
	{
		case KEY1_PRES: 	//进入WIFI AirLink模式
			printf("WIFI进入AirLink连接模式\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);	//Air-link模式接入
			OLED_ShowString(13*6,0,"AirLink",12);
			OLED_Refresh_Gram();
			break;
		case WKUP_PRES: 	//复位WIFI
			printf("WIFI复位，请重新配置连接\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);	//复位WIFI
			OLED_ShowString(13*6,0,"RESET  ",12);
			OLED_Refresh_Gram();
			break;
	}
}


int main(void)
{	 
	u8 t;
////  Stm32_Clock_Init(9);	//系统时钟设置
	uart_init(72,115200);	//串口初始化为115200
	delay_init(72);	   	 	//延时初始化  			 	
	LED_Init();		  		//初始化与LED连接的硬件接口
	KEY_Init();				//初始化按键 
	OLED_Init();
	Gizwits_Init();			//初始化机智云及相关外设 
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
	printf("*    智能温控系统      *\r\n");
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
    gizwitsHandle((dataPoint_t *)&currentDataPoint);//循环处理机智云协议数据
		
		
		
		delay_ms(1);
		t++;
		if((t%50)==0)		//500ms循环一次，DS0闪烁
		{ 
			if(t==150)t=0;
			LED0=!LED0;		//DS0闪烁提示代码运行
		}
	}
}


