#ifndef _screen_H
#define _screen_H

#include "FreeRTOS.h"
#include "queue.h"



/* 私有函数声明 */
static void ProcessUARTCommand(uint8_t cmd);
static void SendAngleToDisplay(int32_t angle);
static void UpdateSG90Angle(int32_t angle);
static void Sr04DistenceDisplay(float distence);
void print_hex_buffer(const unsigned char* buf, size_t len);



/* 外部接口 */
extern QueueHandle_t xScreenCmdQueue;
void Screen_Init(void);

void screen_task(void *pvParameters);
#endif
