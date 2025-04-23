#include "usart.h"		 
#include "config.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "screen.h"




int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{	
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}



/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������		   : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

	//USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���





/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ch;
    ScreenCommandType cmd; // ������ʱ����
//		u8 r;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART1);
        
        switch(ch) 
        {
					case 'A': 
							cmd = SCREEN_CMD_ANGLE_ADD; // ��ֵ������
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
							
					case 'B': 
							cmd = SCREEN_CMD_ANGLE_SUB;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
							
					case 'a': 
							cmd = SCREEN_CMD_SAVE_POS1;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
							
					case 'b': 
							cmd = SCREEN_CMD_SAVE_POS2;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					case 'C': 
							cmd = SCREEN_CMD_ANGLE_TEST;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					case 'c': 
						cmd = SCREEN_CMD_SPEAK1;
						xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
						break;
					
					case 'D': 
							cmd = SCREEN_CMD_SPEAK2;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					case 'd': 
							cmd = SCREEN_CMD_SPEAK3;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					case 'E': 
							cmd = SCREEN_CMD_SPEAK4;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					case 'e': 
							cmd = SCREEN_CMD_SPEAK5;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
							
					case 'X': 
							cmd = SCREEN_CMD_MOTOR_START;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
							
					case 'Z': 
							cmd = SCREEN_CMD_JUICE_CHANGE;
							xQueueSendFromISR(xScreenCmdQueue, &cmd, &xHigherPriorityTaskWoken);
							break;
					
					default: 
							// ���������ַ�
							break; // �޸�����6
			}
			
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
		 /* ������п��ܹ�����жϱ�־ */
    USART_ClearITPendingBit(USART1, USART_IT_RXNE | USART_IT_ORE);
	
} 


void uart1_send_byte(unsigned char data)
{
	USART_SendData(USART1, data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
//����1�����ַ��� �ַ�����""������  ��Ƭ��->����
void uart1_send_str(unsigned char *str)
{
	while(*str != '\0')
	{
		uart1_send_byte(*str);
		str++;
	}
}

//void uart1_send_byte(unsigned char data)
//{
//	USART_SendData(USART1, data);
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//}


////����1�����ַ��� �ַ�����""������  ��Ƭ��->����
//void uart1_send_str(unsigned char *str)
//{
//	while(*str != '\0')
//	{
//		uart1_send_byte(*str);
//		str++;
//	}
//}
