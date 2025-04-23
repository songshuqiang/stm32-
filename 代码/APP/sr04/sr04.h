#ifndef _sr04_H
#define _sr04_H

#include "FreeRTOS.h"
#include "queue.h" 

//管脚端口定义
#define TRIG_PORT 			GPIOA   
#define TRIG_PIN 			GPIO_Pin_0
#define TRIG_PORT_RCC		RCC_APB2Periph_GPIOA

#define ECHO_PORT 			GPIOA   
#define ECHO_PIN 			GPIO_Pin_1
#define ECHO_PORT_RCC		RCC_APB2Periph_GPIOA

//IO操作函数	 
#define TRIG    PAout(0) 
#define ECHO    PAin(1) 

extern QueueHandle_t xSG04Queue;

//函数声明
void sr04_init(void);
float sr04_read_distance(void);
void sr04_task(void *pvParameters);


#endif
