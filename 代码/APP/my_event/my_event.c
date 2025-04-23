#include "my_event.h"
#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "event_groups.h"              // FreeRTOS队列API
#include "config.h"

EventGroupHandle_t MyEventGroup = NULL;

void MyEventGroup_Init(void)
{
	MyEventGroup = xEventGroupCreate();
	if(MyEventGroup == NULL) printf("事件创建失败\r\n");	
}

void My_Set_Event(uint32_t event_flags)
{
	if(MyEventGroup != NULL)
		xEventGroupSetBits(MyEventGroup, event_flags);
}

void My_Clear_Event(uint32_t event_flags)
{
	if(MyEventGroup != NULL)
		xEventGroupClearBits(MyEventGroup, event_flags);	
}

BaseType_t My_Check_Event(uint32_t event_flags, u8 MS)
{
	BaseType_t result = pdFAIL;
	if(MyEventGroup != NULL)
	{
		EventBits_t event_state = xEventGroupWaitBits(MyEventGroup, event_flags, pdFAIL, pdTRUE, pdMS_TO_TICKS(MS));
		if((event_state & event_flags) == event_flags) result = pdTRUE;
	}
	return result;
}


void My_Printf_Event_Status(void)
{
	#if DEBUG_MyEventGroup
		if(MyEventGroup != NULL)
		{
			EventBits_t uxBits = xEventGroupGetBits(MyEventGroup);
			printf("MyEventGroup Status:Ox%X\r\n",uxBits);
		}		
	#endif
}

void printfEventGroupBinary(void)
{
	#if DEBUG_MyEventGroup_BY_BITS
	
		EventBits_t eventBits = xEventGroupGetBits(MyEventGroup);
		int i;
		for(i = (sizeof(EventBits_t) * 8 - 1); i >= 0; i--)
		{
			if(eventBits & (1 << i)) putchar('1');
			else putchar('0');
			if(i == 4) putchar(' ');
			if(i == 8) putchar(' ');
			if(i == 12) putchar(' ');
		}
		putchar('\n');
		
	#endif
}
	

