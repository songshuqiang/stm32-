#ifndef my_queue_H
#define my_queue_H

#include "FreeRTOS.h"           // FreeRTOS����ͷ�ļ�
#include "queue.h"              // FreeRTOS����API

extern QueueHandle_t MyQueue;


#define  QUEUE_LEN    10   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  QUEUE_SIZE   sizeof(void*)   /* ������ÿ����Ϣ��С���ֽڣ� */



void MyQueue_Init(void);
BaseType_t xMyQueueReceiveData(void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xMyQueueSendData(void *pvItemToQueue, TickType_t xTicksToWait);



#endif
