#ifndef _usart_H
#define _usart_H

#include "system.h" 
#include "stdio.h" 

// ��ͷ�ļ��ж��建������
#define USART1_REC_LEN        128    // �������ֽ���
#define FRAME_END_FLAG        0x0D0A // ֡������"\r\n"

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���


void USART1_Init(u32 bound);

void uart1_send_str(unsigned char *str);
void uart1_send_byte(unsigned char data);



#endif


