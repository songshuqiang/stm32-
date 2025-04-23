/*--------------------------------- 头文件包含 ---------------------------------*/
#include "ec11.h"
#include "stm32f10x.h"          // STM32标准外设库
#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "task.h"               // FreeRTOS任务API
#include "queue.h"              // FreeRTOS队列API
#include "pwm.h"

/*--------------------------------- 宏定义 -------------------------------------*/
#define ENCODER_A_PIN GPIO_Pin_0  // 编码器A相连接的GPIO引脚（PA0）
#define ENCODER_B_PIN GPIO_Pin_1  // 编码器B相连接的GPIO引脚（PA1）
#define ENCODER_GPIO GPIOA        // 编码器使用的GPIO端口

/*--------------------------------- 全局变量 -----------------------------------*/
QueueHandle_t xEncoderQueue;     // FreeRTOS队列句柄，用于传递方向信息
volatile int32_t encoderCount = 0; // 编码器累计计数值（volatile确保可见性）


/*--------------------------------- 函数声明 -----------------------------------*/
void Encoder_Init(void);         // 编码器硬件初始化函数
void Encoder_Task(void *pvParameters); // 编码器数据处理任务
int32_t Encoder_GetCount(void);  // 安全获取计数值的接口函数

/*==============================================================================
  硬件初始化函数：Encoder_Init
  功能：配置编码器GPIO、外部中断、NVIC和FreeRTOS队列
==============================================================================*/
void Encoder_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /*---------------- GPIO配置 ----------------*/
    // 使能GPIOA和AFIO时钟（AFIO用于外部中断线路映射）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // 配置PA0和PA1为上拉输入模式（编码器输出通常需要上拉）
    GPIO_InitStruct.GPIO_Pin = ENCODER_A_PIN | ENCODER_B_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Input Pull-Up
    GPIO_Init(ENCODER_GPIO, &GPIO_InitStruct);

    /*---------------- 外部中断配置 ----------------*/
    // 将PA0映射到EXTI0线路，PA1映射到EXTI1线路
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

    // 配置EXTI参数：双沿触发、使能中断
    EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;   // 中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 双沿触发
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;            // 使能线路
    EXTI_Init(&EXTI_InitStruct);

    /*---------------- NVIC配置 ----------------*/
    // 配置EXTI0中断（编码器A相）
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    // 关键配置：设置抢占优先级不高于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
    // 确保可以在中断中安全调用FreeRTOS的FromISR函数
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;   // 子优先级
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // 使能中断通道
    NVIC_Init(&NVIC_InitStruct);

    // 配置EXTI1中断（编码器B相），参数与EXTI0相同
    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    /*---------------- FreeRTOS队列创建 ----------------*/
    // 创建长度为20的队列，每个元素为int8_t类型（存放-1/0/+1）
    xEncoderQueue = xQueueCreate(20, sizeof(int8_t));
    configASSERT(xEncoderQueue != NULL); // 建议添加队列创建校验
}

// 预定义的四步编码器状态转换表（格雷码解码）
static const int8_t encoder_states[16] = {
/* 旧状态 -> 新状态 */ 
/* 00 -> */ 0, -1, +1, 0,    // 00, 01, 10, 11
/* 01 -> */ +1, 0, 0, -1,    // 00, 01, 10, 11
/* 10 -> */ -1, 0, 0, +1,    // 00, 01, 10, 11
/* 11 -> */ 0, +1, -1, 0     // 00, 01, 10, 11
};


/*==============================================================================
  编码器中断公共处理函数：Encoder_ISR_Handler
  功能：读取当前引脚状态，通过状态机判断旋转方向，发送队列消息
  说明：该函数被EXTI0_IRQHandler和EXTI1_IRQHandler共同调用
==============================================================================*/
static void Encoder_ISR_Handler(void) {
    static uint8_t lastAB = 0; // 保存上一次AB相状态（静态变量保持状态）
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*---------------- 状态采集 ----------------*/
    // 读取当前A相和B相电平（返回值为Bit_SET或Bit_RESET）
    uint8_t currentA = GPIO_ReadInputDataBit(ENCODER_GPIO, ENCODER_A_PIN);
    uint8_t currentB = GPIO_ReadInputDataBit(ENCODER_GPIO, ENCODER_B_PIN);
    
    // 将两个引脚状态组合为2bit数值（00,01,10,11）
    uint8_t currentAB = (currentA << 1) | currentB;

    /*---------------- 状态机处理 ----------------*/
    // 组合新旧状态形成4bit索引（前2bit是上一次状态，后2bit是当前状态）
    uint8_t stateIndex = (lastAB << 2) | currentAB;
    
    // 使用预定义的状态转换表获取方向（四步编码器标准解码表）
    // 状态表设计原理：基于格雷码变化序列检测有效步进
    int8_t direction = encoder_states[stateIndex];

    /*---------------- 队列发送 ----------------*/
    if (direction != 0) { // 仅当检测到有效方向时发送
        // 使用FromISR版本发送到队列，可能唤醒阻塞的任务
        xQueueSendFromISR(xEncoderQueue, &direction, &xHigherPriorityTaskWoken);
    }

    lastAB = currentAB; // 更新状态记录

    /*---------------- 上下文切换判断 ----------------*/
    // 如果发送操作唤醒了更高优先级任务，请求上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/*==============================================================================
  EXTI0中断服务函数：处理A相引脚变化
==============================================================================*/
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) { // 确认中断标志
        Encoder_ISR_Handler();                   // 调用公共处理函数
        EXTI_ClearITPendingBit(EXTI_Line0);      // 清除中断标志
    }
}

/*==============================================================================
  EXTI1中断服务函数：处理B相引脚变化
==============================================================================*/
void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        Encoder_ISR_Handler();
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/*==============================================================================
  安全获取计数值函数：Encoder_GetCount
  返回值：当前编码器的累计计数值（int32_t）
  说明：使用临界区保护确保读取时的原子性
==============================================================================*/
int32_t Encoder_GetCount(void) {
    int32_t count;
    
    taskENTER_CRITICAL(); // 进入临界区
    count = encoderCount; // 安全读取
    taskEXIT_CRITICAL();
    
    return count;
}

/*==============================================================================
  编码器处理任务：Encoder_Task
  功能：从队列接收方向数据，安全更新累计计数值
==============================================================================*/
void Encoder_Task(void *pvParameters) {
    int8_t direction; // 接收到的方向值（-1/+1）
    
    while (1) {
        // 阻塞等待队列数据（portMAX_DELAY表示无限等待）
        if (xQueueReceive(xEncoderQueue, &direction, portMAX_DELAY) == pdPASS) {
            /*---------------- 临界区保护 ----------------*/
            // 进入临界区（关闭中断）保证对encoderCount的原子操作
            taskENTER_CRITICAL();
            encoderCount += direction*10;
            taskEXIT_CRITICAL();
						printf("wset text2.txt \"Count: %ld\"\r\n", (long)encoderCount);
            //printf("Count: %ld\r\n", (long)encoderCount);
						TIM_SetCompare4(TIM4,encoderCount*10);//2.5ms	// 更新 CCR1 寄存器
						vTaskDelay(pdMS_TO_TICKS(10)); // 延时 10ms
					
            /* 此处可添加其他处理，例如：
               - 通过串口输出计数值
               - 触发其他任务处理
               - 限制计数范围等 */
        }
    }
}



///*==============================================================================
//  主函数：系统初始化与任务启动
//==============================================================================*/
//int main(void) {
//    SystemInit(); // STM32系统时钟初始化（需根据实际时钟配置）
//    
//    Encoder_Init(); // 初始化编码器硬件和队列
//    
//    // 创建编码器处理任务（建议检查任务创建返回值）
//    xTaskCreate(Encoder_Task,          // 任务函数
//                "EncoderTask",         // 任务名称（调试用）
//                configMINIMAL_STACK_SIZE, // 任务堆栈大小（需根据实际调整）
//                NULL,                  // 任务参数
//                2,                     // 任务优先级（中等优先级）
//                NULL);                 // 任务句柄
//    
//    vTaskStartScheduler(); // 启动FreeRTOS调度器
//    
//    // 调度器启动后不会返回，此处while用于防止编译器警告
//    while (1);
//}











//#include "ec11.h"
//#include "stm32f10x.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"


//#include "ec11.h"
//#include "usart.h"
//#include "FreeRTOS.h"
//#include "task.h"

//u32 count = 2100;

//void EC11_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure; //定义GPIO结构体
//	NVIC_InitTypeDef NVIC_InitStruct;
//	EXTI_InitTypeDef EXTI_InitStruct;	
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE); 
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入模式
//	GPIO_InitStructure.GPIO_Pin = EC11_PORT_PIN; //GPIO结构体指定引脚
//	GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化GPIO	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);	
//	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn; //中断源：外部中断0通道
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	EXTI_InitStruct.EXTI_Line =  EXTI_Line10; //外部中断线路0
//	EXTI_InitStruct.EXTI_Mode =  EXTI_Mode_Interrupt;
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStruct);
//}

//void EXTI15_10_IRQHandler(void)
//{
//	if (GPIO_ReadInputDataBit(GPIOB, EC11_CLK))
//	{
//		delay_ms(1);
//		if (GPIO_ReadInputDataBit(GPIOB, EC11_DT)) 
//		{
//			count +=100;
//			if(count > 2500) count =2500;
//		}
//		else 
//		{
//			count -=100;
//			if(count<=200) count = 200;
//		}
//	}
//	else 
//	{
//		delay_ms(1);
//		if (GPIO_ReadInputDataBit(GPIOB, EC11_DT)) 
//		{
//			count -=100;
//			if(count<=200) count = 200;
//		}
//		else 
//		{
//			count +=100;
//			if(count > 2500) count =2500;
//		}
//	}
//	if(count< 1) count = 200;
//	printf("count:%d\r\n",count);
//	// 清除中断标志
//	TIM_SetCompare4(TIM4,count);//2.5ms	
//	EXTI_ClearFlag(EXTI_Line10);
//	flag = 1;
//}



//void ec11_task(void *pvParameters)
//{
//	while(1)
//	{
//		if (GPIO_ReadInputDataBit(GPIOB, EC11_CLK))
//		{
//			vTaskDelay(1);
//			if (GPIO_ReadInputDataBit(GPIOB, EC11_DT)) 
//			{
//				count +=100;
//				if(count > 2500) count =2500;
//			}
//			else 
//			{
//				count -=100;
//				if(count<=200) count = 200;
//			}
//		}
//		else 
//		{
//			vTaskDelay(1);
//			if (GPIO_ReadInputDataBit(GPIOB, EC11_DT)) 
//			{
//				count -=100;
//				if(count<=200) count = 200;
//			}
//			else 
//			{
//				count +=100;
//				if(count > 2500) count =2500;
//			}
//		}
//		if(count< 1) count = 200;
//		printf("wset text2.txt \"%d\"\r\n",count);
//		vTaskDelay(200);
//	}
//	
//}
