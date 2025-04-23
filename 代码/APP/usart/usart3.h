#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//// ����Э�����ݣ�С����
//const uint8_t protocol_data[] = {
//    0xAA, 0x00,  // ֡ͷ
//    0x00, 0x00, 0x00, 0x00, 0x01  // ���ݶ�
//      
//};

//// �Ƽ�ʹ�ýṹ���װЭ��
//typedef struct {
//    uint8_t header;     // ֡ͷ 0xAA
//    uint8_t data[5];    // ���ݶ�
//    uint8_t checksum;   // У��λ
//} UartProtocol;


// ��ʼ��USART3
void USART3_Init(uint32_t baudrate);

// ���͵����ַ�
void USART3_SendChar(char ch);

//// �����ַ���
//void USART3_SendString(const char *str);

// �����ַ�������ģʽ��
char USART3_ReceiveChar(void);

// �жϷ�������������stm32f10x_it.c��ʵ�֣�
void USART3_IRQHandler(void);

extern void USART3_SendHexData(const uint8_t *data);
extern void USART3_SendString(const char *str);

#endif

