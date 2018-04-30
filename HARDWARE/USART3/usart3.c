#include "sys.h"
#include "usart3.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h"  
#include "gizwits_protocol.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������ 
//����3��ʼ������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/3/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 	

//����3���������жϷ����������յ�������ͨ��gizPutDataд�뻺����
void USART3_IRQHandler(void)
{
	u8 res;	      
	if(USART3->SR&(1<<5))//���յ�����
	{	 
		res=USART3->DR; 
		gizPutData(&res,1);//����д�뵽������
	}  				 											 
}   
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void usart3_init(u32 pclk1,u32 bound)
{  	 
	RCC->APB2ENR|=1<<3;   	//ʹ��PORTB��ʱ��  
 	GPIOB->CRH&=0XFFFF00FF;	//IO״̬����
	GPIOB->CRH|=0X00008B00;	//IO״̬���� 
	
	RCC->APB1ENR|=1<<18;  	//ʹ�ܴ���ʱ�� 	 
	RCC->APB1RSTR|=1<<18;   //��λ����3
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	
	//����������
 	USART3->BRR=(pclk1*1000000)/(bound);// ����������	 
	USART3->CR1|=0X200C;  	//1λֹͣ,��У��λ.
	//ʹ�ܽ����ж� 
	USART3->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(1,2,USART3_IRQn,2);//��2  
}
 

































