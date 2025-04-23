#include "key.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"               // FreeRTOS任务API
#include "my_event.h"
#include "config.h"


/*******************************************************************************
* 函 数 名         : KEY_Init
* 函数功能		   : 按键初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能外部APB2总线时钟RCC
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//上拉输入 
	
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN | KEY2_PIN;	   //选择你要设置的IO口
	GPIO_Init(KEY_PORT,&GPIO_InitStructure);		  /* 初始化GPIO */
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU ;//上拉输入 
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;	   //选择你要设置的IO口  GPIO_Mode_IN_FLOATING
	GPIO_Init(KEY_PORT,&GPIO_InitStructure);		  /* 初始化GPIO */
}



void key_task(void *pvParameters)
{
	while(1)
	{
		if(KEY1 == 0)	My_Set_Event(EVENT_FLAG_READY_1);
		else My_Clear_Event(EVENT_FLAG_READY_1);
		
		if(KEY2 == 0) My_Set_Event(EVENT_FLAG_READY_2);
		else My_Clear_Event(EVENT_FLAG_READY_2);
		
		vTaskDelay(pdMS_TO_TICKS(1)); // 延时 10ms
	}
	
}
