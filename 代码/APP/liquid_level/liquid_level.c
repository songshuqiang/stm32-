#include "liquid_level.h"
#include "SysTick.h"
#include "usart.h"
#include "usart3.h"
#include "FreeRTOS.h"
#include "task.h"
#include "my_event.h"
#include "config.h"


void liquid_level_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ���ⲿAPB2����ʱ��RCC
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//�������� 
	GPIO_InitStructure.GPIO_Pin=liquid_level_2_PIN | liquid_level_3_PIN | liquid_level_4_PIN;	   //ѡ����Ҫ���õ�IO��
	GPIO_Init(liquid_level_PORT,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
}

void Check_level(void)
{
	if(My_Check_Event(EVENT_FLAG_LEVEL_1, 10))
	{
		USART3_SendString("speak5");
	}
	else if(My_Check_Event(EVENT_FLAG_LEVEL_2, 10))
	{
		USART3_SendString("speak4");
	}
	else if(My_Check_Event(EVENT_FLAG_LEVEL_3, 10))
	{
		USART3_SendString("speak3");
	}
	
}

void liquid_level_task(void *pvParameters)
{
	while(1)
	{
		if(liquid_level_4 == 0)
		{
			My_Set_Event(EVENT_FLAG_LEVEL_1);
			My_Clear_Event(EVENT_FLAG_STOP);
			My_Clear_Event(EVENT_FLAG_FINISH_1);
			My_Clear_Event(EVENT_FLAG_RUN);
			debug_printf_level("wset level_text.txt \"����80%%����\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_1);
		
		if(liquid_level_3 == 0)
		{
			My_Set_Event(EVENT_FLAG_LEVEL_2);
			My_Clear_Event(EVENT_FLAG_STOP);
			debug_printf_level("wset level_text.txt \"����50%%����\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_2);

		if(liquid_level_2 == 0) 
		{
			My_Set_Event(EVENT_FLAG_LEVEL_3);
			My_Set_Event(EVENT_FLAG_STOP);
			debug_printf_level("wset level_text.txt \"����20%%����\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_3);
		
		My_Printf_Event_Status();
		printfEventGroupBinary();
		vTaskDelay(200);
	}
	
	
}
