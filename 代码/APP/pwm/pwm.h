#ifndef _PWM_H
#define _PWM_H

#include "system.h"
#include "stm32f10x_tim.h"

void TIM4_CH4_PWM_Init(u16 per,u16 psc);
void sg90_task(void *pvParameters);

#endif

