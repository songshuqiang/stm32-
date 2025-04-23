#ifndef liquid_level_H
#define liquid_level_H

#include "system.h"
 
#define liquid_level_4_PIN   			GPIO_Pin_2    	//定义KEY0管脚
#define liquid_level_3_PIN    		GPIO_Pin_3    	//定义KEY1管脚
#define liquid_level_2_PIN    		GPIO_Pin_4    	//定义KEY2管脚

#define liquid_level_PORT 			GPIOA 		//定义端口


//使用位操作定义
#define liquid_level_4 	PAin(4)
#define liquid_level_3 	PAin(3)
#define liquid_level_2 	PAin(2)

void Check_level(void);
void liquid_level_Init(void);
void liquid_level_task(void *pvParameters);
void liquid_level_task(void *pvParameters);


#endif
