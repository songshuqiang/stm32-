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
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能外部APB2总线时钟RCC
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//上拉输入 
	GPIO_InitStructure.GPIO_Pin=liquid_level_2_PIN | liquid_level_3_PIN | liquid_level_4_PIN;	   //选择你要设置的IO口
	GPIO_Init(liquid_level_PORT,&GPIO_InitStructure);		  /* 初始化GPIO */
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
			debug_printf_level("wset level_text.txt \"储料80%%以上\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_1);
		
		if(liquid_level_3 == 0)
		{
			My_Set_Event(EVENT_FLAG_LEVEL_2);
			My_Clear_Event(EVENT_FLAG_STOP);
			debug_printf_level("wset level_text.txt \"储料50%%以上\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_2);

		if(liquid_level_2 == 0) 
		{
			My_Set_Event(EVENT_FLAG_LEVEL_3);
			My_Set_Event(EVENT_FLAG_STOP);
			debug_printf_level("wset level_text.txt \"储料20%%以下\"\r\n");
		}else My_Clear_Event(EVENT_FLAG_LEVEL_3);
		
		My_Printf_Event_Status();
		printfEventGroupBinary();
		vTaskDelay(200);
	}
	
	
}
