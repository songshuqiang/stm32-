#ifndef _led_H
#define _led_H

#include "system.h"

/*  LED时钟端口、引脚定义 */
#define LED13_PORT 			GPIOC  
#define LED13_PIN 			GPIO_Pin_13
#define LED13_PORT_RCC		RCC_APB2Periph_GPIOC

#define LED3_PORT 			GPIOA  
#define LED3_PIN 			GPIO_Pin_3
#define LED3_PORT_RCC		RCC_APB2Periph_GPIOA

#define LED4_PORT 			GPIOA 
#define LED4_PIN 			GPIO_Pin_4
#define LED4_PORT_RCC		RCC_APB2Periph_GPIOA


//位带定义
#define LED13 	PCout(13)
#define LED3 	PAout(3)
#define LED4 	PAout(4)


//函数声明
void LED_Init(void);

#endif
