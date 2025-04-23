#ifndef _key_H
#define _key_H

#include "system.h"
 
#define KEY2_PIN   			GPIO_Pin_13    	//定义KEY0管脚
#define KEY1_PIN    		GPIO_Pin_12    	//定义KEY1管脚


#define KEY_PORT 			GPIOA 		//定义端口


//使用位操作定义
#define KEY2 	PAin(13)
#define KEY1 	PAin(12)

//定义各个按键值  
#define KEY1_PRESS 		1
#define KEY2_PRESS		2
 
void KEY_Init(void);
void key_task(void *pvParameters);

#endif
