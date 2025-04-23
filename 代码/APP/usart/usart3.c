#include "usart3.h"
#include <stdarg.h>
#include "FreeRTOS.h"  // 如果使用RTOS
#include "semphr.h"
#include <stdio.h>
#include "config.h"

// 互斥锁（用于多任务保护）
static SemaphoreHandle_t xUsart3Mutex = NULL;

// USART3的GPIO和中断配置
void USART3_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能USART3和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // 配置PB10（TX）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置PB11（RX）为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART参数配置
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    // 使能USART3接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 配置USART3中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;  // 优先级需适配FreeRTOS
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能USART3
    USART_Cmd(USART3, ENABLE);
		
		// 创建互斥锁（如果使用RTOS）
    xUsart3Mutex = xSemaphoreCreateMutex();
		if(xUsart3Mutex == NULL) printf("xUsart3Mutex 创建失败\r\n");

}



//// 安全发送函数
//void USART3_SendHexData(const uint8_t *data)
//{
//	uint16_t i;
//	
//	if(xUsart3Mutex != NULL) 
//	{
//			// 获取互斥锁（最多等待20ms）
//			if(xSemaphoreTake(xUsart3Mutex, pdMS_TO_TICKS(20)))
//			{
//				 for(i=0; i<7; i++)
//				 {
//						// 等待发送寄存器空
//						while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//						// 写入数据寄存器
//						USART_SendData(USART3, data[i]);
//				 }
//				// 等待最后一次传输完成
//				while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
//				xSemaphoreGive(xUsart3Mutex);
//			}
//	}
//}

// 安全发送字符串函数（自动判断长度）
void USART3_SendString(const char *str)
{
	uint16_t i = 0;
	if(xUsart3Mutex != NULL) 
	{
		// 获取互斥锁（最多等待20ms）
		if(xSemaphoreTake(xUsart3Mutex, pdMS_TO_TICKS(20)))
		{
			// 遍历直到字符串结束符
			while(str[i] != '\0') 
			{
					// 等待发送寄存器空
					while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
					// 写入数据寄存器
					USART_SendData(USART3, (uint8_t)str[i]);
					i++;
			}
			// 等待最后一次传输完成
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			xSemaphoreGive(xUsart3Mutex);
		}
		else printf("获取互斥锁失败\n");
	}
}





// 发送字符
void USART3_SendChar(char ch) {
    USART_SendData(USART3, (uint16_t)ch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

//// 发送字符串
//void USART3_SendString(const char *str) {
//    while (*str) {
//        USART3_SendChar(*str++);
//    }
//}

// 阻塞式接收字符
char USART3_ReceiveChar(void) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART3);
}

//// 添加USART3中断服务函数
//void USART3_IRQHandler(void) {
//    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
//        // 处理接收到的数据（示例：回显）
//        char received_char = USART_ReceiveData(USART3);
//        USART3_SendChar(received_char);  // 回显
//        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//    }
//}

void USART3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ch;
    ScreenCommandType cmd; // 新增临时变量
//		u8 r;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART3);
        
        switch(ch) 
        {
					case 'P': 
							cmd = SCREEN_CMD_SPEAK_GET; // 赋值给变量
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
			
					
					default: 
							// 处理其他字符
							break; // 修复错误6
			}
			
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
		 /* 清除所有可能挂起的中断标志 */
    USART_ClearITPendingBit(USART3, USART_IT_RXNE | USART_IT_ORE);
	
} 
