#include "key.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"               // FreeRTOS����API
#include "my_event.h"
#include "config.h"


/*******************************************************************************
* �� �� ��         : KEY_Init
* ��������		   : ������ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ���ⲿAPB2����ʱ��RCC
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//�������� 
	
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN | KEY2_PIN;	   //ѡ����Ҫ���õ�IO��
	GPIO_Init(KEY_PORT,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//�������� 
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;	   //ѡ����Ҫ���õ�IO��  GPIO_Mode_IN_FLOATING
	GPIO_Init(KEY_PORT,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
}



void key_task(void *pvParameters)
{
	while(1)
	{
		if(KEY1 == 0)	My_Set_Event(EVENT_FLAG_READY_1);
		else My_Clear_Event(EVENT_FLAG_READY_1);
		
		if(KEY2 == 0) My_Set_Event(EVENT_FLAG_READY_2);
		else My_Clear_Event(EVENT_FLAG_READY_2);
		
		vTaskDelay(pdMS_TO_TICKS(1)); // ��ʱ 10ms
	}
	
}
