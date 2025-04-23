#ifndef liquid_level_H
#define liquid_level_H

#include "system.h"
 
#define liquid_level_4_PIN   			GPIO_Pin_2    	//����KEY0�ܽ�
#define liquid_level_3_PIN    		GPIO_Pin_3    	//����KEY1�ܽ�
#define liquid_level_2_PIN    		GPIO_Pin_4    	//����KEY2�ܽ�

#define liquid_level_PORT 			GPIOA 		//����˿�


//ʹ��λ��������
#define liquid_level_4 	PAin(4)
#define liquid_level_3 	PAin(3)
#define liquid_level_2 	PAin(2)

void Check_level(void);
void liquid_level_Init(void);
void liquid_level_task(void *pvParameters);
void liquid_level_task(void *pvParameters);


#endif
