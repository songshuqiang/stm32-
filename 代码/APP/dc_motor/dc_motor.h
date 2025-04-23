#ifndef _DC_Motor_H
#define _DC_Motor_H

#include "system.h"
#include "FreeRTOS.h"
#include "queue.h"

#define DC_Motor_PORT  			GPIOB
#define DC_Motor_PIN 				GPIO_Pin_13
#define DC_Motor_PORT_RCC 	RCC_APB2Periph_GPIOB

#define DC_Motor	PBout(13)

extern QueueHandle_t xDcMotorQueue;

void DC_Motor_Init(void);
void dc_motor_task(void *pvParameters);

#endif
