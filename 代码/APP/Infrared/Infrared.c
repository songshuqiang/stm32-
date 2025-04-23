#include "Infrared.h"
#include "FreeRTOS.h"
#include "task.h"
#include "my_queue.h"
#include "config.h"

//�ⲿ������ģ���ʼ��
void Infrared_Init(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;	

	RCC_APB2PeriphClockCmd(DATA_PORT_RCC,ENABLE);   

	GPIO_InitStructure.GPIO_Pin=DATA_PIN;		
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;	
	GPIO_Init(DATA_PORT,&GPIO_InitStructure); 	
}

void infrared_task(void *pvParameters)
{
	MyData data;
	while(1)
	{
		xMyQueueReceiveData((void*)&data.distence, portMAX_DELAY);
//		if(DATA_PIN_READ==0) printf("test ok!\r\n");
		vTaskDelay(pdMS_TO_TICKS(10)); // ��ʱ 10ms
	}
}
