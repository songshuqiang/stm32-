#include "sr04.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"              // FreeRTOS队列API
#include "my_queue.h"
#include "config.h"
#include "usart3.h"

/*--------------------------------- 全局变量 -----------------------------------*/
volatile u16 TIM3_cnt = 0; // 定时器4计数值（volatile确保可见性）
QueueHandle_t xSG04Queue = NULL;

/*******************************************************************************
* 函 数 名         : TIM3_Init
* 函数功能		   : TIM4初始化函数
* 输    入         : per:重装载值
					 psc:分频系数
* 输    出         : 无
*******************************************************************************/

void TIM3_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM3,DISABLE); //关闭定时器
}

//SR04端口初始化
void sr04_init(void)
{
	GPIO_InitTypeDef  GPIO_InitSture;
	EXTI_InitTypeDef EXTI_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	
//	RCC_APB2PeriphClockCmd(TRIG_PORT_RCC|ECHO_PORT_RCC,ENABLE);
	/*---------------- GPIO配置 ----------------*/
  // 使能GPIOB和AFIO时钟（AFIO用于外部中断线路映射）
  RCC_APB2PeriphClockCmd(TRIG_PORT_RCC | ECHO_PORT_RCC | RCC_APB2Periph_AFIO, ENABLE);
	
	//配置IO端口
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitSture.GPIO_Pin=TRIG_PIN;
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(TRIG_PORT,&GPIO_InitSture);
	
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitSture.GPIO_Pin=ECHO_PIN;
	GPIO_Init(ECHO_PORT,&GPIO_InitSture);
	
	/*---------------- 外部中断配置 ----------------*/
	// 将PA1->ECHO映射到EXTI1线路
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	// 配置EXTI参数：双沿触发、使能中断
	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;   // 中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 双沿触发
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;            // 使能线路
	EXTI_Init(&EXTI_InitStruct);
	
	/*---------------- NVIC配置 ----------------*/
	// 配置EXTI1中断
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	// 关键配置：设置抢占优先级不高于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
	// 确保可以在中断中安全调用FreeRTOS的FromISR函数
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;   // 子优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // 使能中断通道
	NVIC_Init(&NVIC_InitStruct);
	
	TIM3_Init(50000,71);
	
	
	xSG04Queue = xQueueCreate(1, sizeof(float));  // 存储测量距离值
	if (xSG04Queue == NULL) {
			debug_SR04("ERROR: xAngleQueue create failed!\r\n");
	} else {
			debug_SR04("xAngleQueue created successfully!\r\n");
	}
}



/*==============================================================================
  EXTI1中断服务函数：处理PA1相引脚变化
==============================================================================*/
void EXTI1_IRQHandler(void)
{
	float distence = 0.0; 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		if(ECHO==1)
		{
			TIM_SetCounter(TIM3,0);//清零计数器值
			TIM_Cmd(TIM3,ENABLE);//开启定时器，开始计数
		}
		if(ECHO==0)
		{
			TIM_Cmd(TIM3,DISABLE);//关闭定时器
			TIM3_cnt=TIM_GetCounter(TIM3);
			distence = (float)TIM3_cnt*340/20000.0;
			xQueueSendFromISR(xSG04Queue, &distence, &xHigherPriorityTaskWoken);
			
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
		/*---------------- 上下文切换判断 ----------------*/
    // 如果发送操作唤醒了更高优先级任务，请求上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}


void sr04_task(void *pvParameters)
{
	float distence = 0.0; 
//	unsigned char data[] = {0xAA,0x00,0x00,0x00,0x00,0x00,0x01};

	while(1)
	{	
		TRIG=1;	//拉高
		vTaskDelay(pdMS_TO_TICKS(20)); // 延时 20ms 
		TRIG=0;	//拉低
		vTaskDelay(pdMS_TO_TICKS(200)); // 延时 20ms 
//		printf("wset sg04_text.txt \"%d\"\r\n",TIM3_cnt*340/20000);
		if(xQueueReceive(xSG04Queue, &distence,0)== pdPASS) debug_SR04("wset sg04_txt.txt \"%.2f\"\r\n",distence);
		

		
	}
	
	
}


