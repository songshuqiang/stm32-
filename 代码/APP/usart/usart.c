#include "usart.h"		 
#include "config.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "screen.h"




int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{	
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}



/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能		   : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
}


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记





/*******************************************************************************
* 函 数 名         : USART1_IRQHandler
* 函数功能		   : USART1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ch;
    ScreenCommandType cmd; // 新增临时变量
//		u8 r;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART1);
        
        switch(ch) 
        {
					case 'A': 
							cmd = SCREEN_CMD_ANGLE_ADD; // 赋值给变量
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
							// 处理其他字符
							break; // 修复错误6
			}
			
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
		 /* 清除所有可能挂起的中断标志 */
    USART_ClearITPendingBit(USART1, USART_IT_RXNE | USART_IT_ORE);
	
} 


void uart1_send_byte(unsigned char data)
{
	USART_SendData(USART1, data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
//串口1发送字符串 字符串用""框起来  单片机->电脑
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


////串口1发送字符串 字符串用""框起来  单片机->电脑
//void uart1_send_str(unsigned char *str)
//{
//	while(*str != '\0')
//	{
//		uart1_send_byte(*str);
//		str++;
//	}
//}
