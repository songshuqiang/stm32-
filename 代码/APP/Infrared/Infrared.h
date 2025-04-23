#ifndef _Infrared_H
#define _Infrared_H

#include "system.h"
#include "SysTick.h"

//�ⲿ������ģ��ܽŶ���
#define DATA_PORT 			GPIOB  
#define DATA_PIN 			GPIO_Pin_15
#define DATA_PORT_RCC		RCC_APB2Periph_GPIOB

#define DATA_PIN_READ 		PBin(15) 

//�ⲿ������ģ���ʼ��
void Infrared_Init(void);
void infrared_task(void *pvParameters);

#endif
