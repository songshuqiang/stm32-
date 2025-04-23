#include "my_queue.h"
#include "stm32f10x.h"          // STM32��׼�����
#include "FreeRTOS.h"           // FreeRTOS����ͷ�ļ�
#include "queue.h"              // FreeRTOS����API

QueueHandle_t MyQueue = NULL;

void MyQueue_Init(void)
{
	/* ����Test_Queue */
	MyQueue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
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
