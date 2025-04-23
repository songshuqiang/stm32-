#ifndef _usart_H
#define _usart_H

#include "system.h" 
#include "stdio.h" 

// 在头文件中定义缓冲区宏
#define USART1_REC_LEN        128    // 最大接收字节数
#define FRAME_END_FLAG        0x0D0A // 帧结束符"\r\n"

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记


void USART1_Init(u32 bound);

void uart1_send_str(unsigned char *str);
void uart1_send_byte(unsigned char data);



#endif


