#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//// 定义协议数据（小端序）
//const uint8_t protocol_data[] = {
//    0xAA, 0x00,  // 帧头
//    0x00, 0x00, 0x00, 0x00, 0x01  // 数据段
//      
//};

//// 推荐使用结构体封装协议
//typedef struct {
//    uint8_t header;     // 帧头 0xAA
//    uint8_t data[5];    // 数据段
//    uint8_t checksum;   // 校验位
//} UartProtocol;


// 初始化USART3
void USART3_Init(uint32_t baudrate);

// 发送单个字符
void USART3_SendChar(char ch);

//// 发送字符串
//void USART3_SendString(const char *str);

// 接收字符（阻塞模式）
char USART3_ReceiveChar(void);

// 中断服务函数声明（在stm32f10x_it.c中实现）
void USART3_IRQHandler(void);

extern void USART3_SendHexData(const uint8_t *data);
extern void USART3_SendString(const char *str);

#endif

