/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits 控制协议处理,及平台相关的硬件初始化 
* @author       Gizwits
* @date         2016-09-05
* @version      V03010101
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/

#include <stdio.h>
#include <string.h>
#include "gizwits_protocol.h"
#include "gizwits_product.h"
#include "usart3.h"
#include "oled.h"
#include "math.h"


/**@} */
/**@name Gizwits 用户API接口
* @{
*/
ControlDataTypeDef ControlData;
extern dataPoint_t currentDataPoint;

/**
* @brief 事件处理接口

* 说明：

* 1.用户可以对WiFi模组状态的变化进行自定义的处理

* 2.用户可以在该函数内添加数据点事件处理逻辑，如调用相关硬件外设的操作接口

* @param[in] info : 事件队列
* @param[in] data : 协议数据
* @param[in] len : 协议数据长度
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;
  protocolTime_t *ptime = (protocolTime_t *)data;

  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_DeviceOnOff:
        currentDataPoint.valueDeviceOnOff = dataPointPtr->valueDeviceOnOff;
				ControlData.CtrlOnOff = currentDataPoint.valueDeviceOnOff;
        GIZWITS_LOG("Evt: EVENT_DeviceOnOff %d \n", currentDataPoint.valueDeviceOnOff);
        if(0x01 == currentDataPoint.valueDeviceOnOff)
        {
          //user handle
					PFout(0) = 1;
					OLED_ShowString(13*6,12,"ON ",12);
					OLED_Refresh_Gram();
        }
        else
        {
          //user handle
					PFout(0) = 0;
					OLED_ShowString(13*6,12,"OFF",12);
					OLED_Refresh_Gram();					
        }
        break;
      case EVENT_TempSet:
        currentDataPoint.valueTempSet = dataPointPtr->valueTempSet;
				ControlData.CtrlTempSet = (currentDataPoint.valueTempSet - 20)*10;
        GIZWITS_LOG("Evt:EVENT_TempSet %d\n",currentDataPoint.valueTempSet);
        //user handle
				if(ControlData.CtrlTempSet > 0)
					OLED_ShowChar(9*6,24,'+',12,1);
				else if(ControlData.CtrlTempSet == 0)
					OLED_ShowChar(9*6,24,' ',12,1);
				else
					OLED_ShowChar(9*6,24,'-',12,1);
				OLED_ShowNum(10*6,24,abs(ControlData.CtrlTempSet)/10,3,12);
				OLED_ShowChar(13*6,24,'.',12,1);
				OLED_ShowNum(14*6,24,abs(ControlData.CtrlTempSet)%10,1,12);				
				OLED_Refresh_Gram();
        break;


      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
        break;
      case WIFI_DISCON_ROUTER:
        break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      case WIFI_NTP:
        GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
        break;
      default:
        break;
    }
  }

  return 0;
}


/**@} */


//MCU软复位函数
void mcuRestart(void)
{
	INTX_DISABLE();		//禁止所有中断
	Sys_Soft_Reset();	//软复位STM32
}

/**@} */
/**
* @brief 系统毫秒计时维护函数,毫秒自增,溢出归零

* @param none
* @return none
*/
static uint32_t timerMsCount;
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief 读取系统时间毫秒计时数

* @param none
* @return 系统时间毫秒数
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief 定时器TIM3中断处理函数

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief USART2串口中断函数

* 接收功能，用于接收与WiFi模组间的串口协议数据
* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
  uint8_t value = 0;
  //value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}


//串口写操作，发送数据到WiFi模组 
//buf: 数据地址；len: 数据长度；
//返回值：-1，错误；其他，有效数据长度; 
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    if(NULL == buf)return -1; 
    for(i=0; i<len; i++)
    {   
		USART3->DR=(u8)buf[i];    		//循环发送buf[i]到WIFI模组 
        while((USART3->SR&0X40)==0);	//等待发送完毕
        if(i >=2 && buf[i] == 0xFF)
        { 
			USART3->DR=0x55;			//发送0x55到WIFI模组
			while((USART3->SR&0X40)==0);//等待发送完毕
        }
    }     
    return len;
}


