#ifndef _key_H
#define _key_H

#include "system.h"
 
#define KEY2_PIN   			GPIO_Pin_13    	//����KEY0�ܽ�
#define KEY1_PIN    		GPIO_Pin_12    	//����KEY1�ܽ�


#define KEY_PORT 			GPIOA 		//����˿�


//ʹ��λ��������
#define KEY2 	PAin(13)
#define KEY1 	PAin(12)

//�����������ֵ  
#define KEY1_PRESS 		1
#define KEY2_PRESS		2
 
void KEY_Init(void);
void key_task(void *pvParameters);

#endif
