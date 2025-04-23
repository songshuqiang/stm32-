#include "my_queue.h"
#include "stm32f10x.h"          // STM32标准外设库
#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "queue.h"              // FreeRTOS队列API

QueueHandle_t MyQueue = NULL;

void MyQueue_Init(void)
{
	/* 创建Test_Queue */
	MyQueue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
                            (UBaseType_t ) QUEUE_SIZE);/* 消息的大小 */
}

BaseType_t xMyQueueSendData(void *pvItemToQueue, TickType_t xTicksToWait)
{
	if(MyQueue != NULL)
		return xQueueSend(MyQueue, &pvItemToQueue, xTicksToWait);
	
	return pdFALSE;
	
}

BaseType_t xMyQueueReceiveData(void *pvBuffer, TickType_t xTicksToWait)
{
	if(MyQueue != NULL)
	{
		void *pvReceivedItem;
		if(xQueueReceive(MyQueue, &pvReceivedItem, xTicksToWait) == pdPASS)
		{
			*(void**)pvBuffer = pvReceivedItem;
			return pdPASS;
		}
	}
	
	return pdFALSE;
}
