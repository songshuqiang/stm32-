#ifndef my_event_H
#define my_event_H

#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "event_groups.h"              // FreeRTOS队列API

extern EventGroupHandle_t MyEventGroup;


void MyEventGroup_Init(void);
void My_Set_Event(uint32_t event_flags);
void My_Clear_Event(uint32_t event_flags);
void My_Printf_Event_Status(void);
void printfEventGroupBinary(void);
BaseType_t My_Check_Event(uint32_t event_flags, u8 MS);

#endif
