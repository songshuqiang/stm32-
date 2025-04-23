#ifndef my_queue_H
#define my_queue_H

#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "queue.h"              // FreeRTOS队列API

extern QueueHandle_t MyQueue;


#define  QUEUE_LEN    10   /* 队列的长度，最大可包含多少个消息 */
#define  QUEUE_SIZE   sizeof(void*)   /* 队列中每个消息大小（字节） */



void MyQueue_Init(void);
BaseType_t xMyQueueReceiveData(void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xMyQueueSendData(void *pvItemToQueue, TickType_t xTicksToWait);



#endif
