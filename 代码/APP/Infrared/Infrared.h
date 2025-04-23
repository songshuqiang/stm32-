#ifndef _Infrared_H
#define _Infrared_H

#include "system.h"
#include "SysTick.h"

//外部传感器模块管脚定义
#define DATA_PORT 			GPIOB  
#define DATA_PIN 			GPIO_Pin_15
#define DATA_PORT_RCC		RCC_APB2Periph_GPIOB

#define DATA_PIN_READ 		PBin(15) 

//外部传感器模块初始化
void Infrared_Init(void);
void infrared_task(void *pvParameters);

#endif
