#include "usart3.h"
#include <stdarg.h>
#include "FreeRTOS.h"  // ���ʹ��RTOS
#include "semphr.h"
#include <stdio.h>
#include "config.h"

// �����������ڶ����񱣻���
static SemaphoreHandle_t xUsart3Mutex = NULL;

// USART3��GPIO���ж�����
void USART3_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ʹ��USART3��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // ����PB10��TX��Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ����PB11��RX��Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART��������
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    // ʹ��USART3�����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // ����USART3�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;  // ���ȼ�������FreeRTOS
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ʹ��USART3
    USART_Cmd(USART3, ENABLE);
		
		// ���������������ʹ��RTOS��
    xUsart3Mutex = xSemaphoreCreateMutex();
		if(xUsart3Mutex == NULL) printf("xUsart3Mutex ����ʧ��\r\n");

}



//// ��ȫ���ͺ���
//void USART3_SendHexData(const uint8_t *data)
//{
//	uint16_t i;
//	
//	if(xUsart3Mutex != NULL) 
//	{
//			// ��ȡ�����������ȴ�20ms��
//			if(xSemaphoreTake(xUsart3Mutex, pdMS_TO_TICKS(20)))
//			{
//				 for(i=0; i<7; i++)
//				 {
//						// �ȴ����ͼĴ�����
//						while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//						// д�����ݼĴ���
//						USART_SendData(USART3, data[i]);
//				 }
//				// �ȴ����һ�δ������
//				while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
//				xSemaphoreGive(xUsart3Mutex);
//			}
//	}
//}

// ��ȫ�����ַ����������Զ��жϳ��ȣ�
void USART3_SendString(const char *str)
{
	uint16_t i = 0;
	if(xUsart3Mutex != NULL) 
	{
		// ��ȡ�����������ȴ�20ms��
		if(xSemaphoreTake(xUsart3Mutex, pdMS_TO_TICKS(20)))
		{
			// ����ֱ���ַ���������
			while(str[i] != '\0') 
			{
					// �ȴ����ͼĴ�����
					while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
					// д�����ݼĴ���
					USART_SendData(USART3, (uint8_t)str[i]);
					i++;
			}
			// �ȴ����һ�δ������
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			xSemaphoreGive(xUsart3Mutex);
		}
		else printf("��ȡ������ʧ��\n");
	}
}





// �����ַ�
void USART3_SendChar(char ch) {
    USART_SendData(USART3, (uint16_t)ch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

//// �����ַ���
//void USART3_SendString(const char *str) {
//    while (*str) {
//        USART3_SendChar(*str++);
//    }
//}

// ����ʽ�����ַ�
char USART3_ReceiveChar(void) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART3);
}

//// ���USART3�жϷ�����
//void USART3_IRQHandler(void) {
//    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
//        // ������յ������ݣ�ʾ�������ԣ�
//        char received_char = USART_ReceiveData(USART3);
//        USART3_SendChar(received_char);  // ����
//        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//    }
//}

void USART3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ch;
    ScreenCommandType cmd; // ������ʱ����
//		u8 r;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART3);
        
        switch(ch) 
        {
					case 'P': 
							cmd = SCREEN_CMD_SPEAK_GET; // ��ֵ������
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
			
					
					default: 
							// ���������ַ�
							break; // �޸�����6
			}
			
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
		 /* ������п��ܹ�����жϱ�־ */
    USART_ClearITPendingBit(USART3, USART_IT_RXNE | USART_IT_ORE);
	
} 
