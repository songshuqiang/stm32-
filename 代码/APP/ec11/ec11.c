/*--------------------------------- ͷ�ļ����� ---------------------------------*/
#include "ec11.h"
#include "stm32f10x.h"          // STM32��׼�����
#include "FreeRTOS.h"           // FreeRTOS����ͷ�ļ�
#include "task.h"               // FreeRTOS����API
#include "queue.h"              // FreeRTOS����API
#include "pwm.h"

/*--------------------------------- �궨�� -------------------------------------*/
#define ENCODER_A_PIN GPIO_Pin_0  // ������A�����ӵ�GPIO���ţ�PA0��
#define ENCODER_B_PIN GPIO_Pin_1  // ������B�����ӵ�GPIO���ţ�PA1��
#define ENCODER_GPIO GPIOA        // ������ʹ�õ�GPIO�˿�

/*--------------------------------- ȫ�ֱ��� -----------------------------------*/
QueueHandle_t xEncoderQueue;     // FreeRTOS���о�������ڴ��ݷ�����Ϣ
volatile int32_t encoderCount = 0; // �������ۼƼ���ֵ��volatileȷ���ɼ��ԣ�


/*--------------------------------- �������� -----------------------------------*/
void Encoder_Init(void);         // ������Ӳ����ʼ������
void Encoder_Task(void *pvParameters); // ���������ݴ�������
int32_t Encoder_GetCount(void);  // ��ȫ��ȡ����ֵ�Ľӿں���

/*==============================================================================
  Ӳ����ʼ��������Encoder_Init
  ���ܣ����ñ�����GPIO���ⲿ�жϡ�NVIC��FreeRTOS����
==============================================================================*/
void Encoder_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /*---------------- GPIO���� ----------------*/
    // ʹ��GPIOA��AFIOʱ�ӣ�AFIO�����ⲿ�ж���·ӳ�䣩
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // ����PA0��PA1Ϊ��������ģʽ�����������ͨ����Ҫ������
    GPIO_InitStruct.GPIO_Pin = ENCODER_A_PIN | ENCODER_B_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Input Pull-Up
    GPIO_Init(ENCODER_GPIO, &GPIO_InitStruct);

    /*---------------- �ⲿ�ж����� ----------------*/
    // ��PA0ӳ�䵽EXTI0��·��PA1ӳ�䵽EXTI1��·
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

    // ����EXTI������˫�ش�����ʹ���ж�
    EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;   // �ж�ģʽ
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ˫�ش���
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;            // ʹ����·
    EXTI_Init(&EXTI_InitStruct);

    /*---------------- NVIC���� ----------------*/
    // ����EXTI0�жϣ�������A�ࣩ
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    // �ؼ����ã�������ռ���ȼ�������configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
    // ȷ���������ж��а�ȫ����FreeRTOS��FromISR����
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;   // �����ȼ�
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // ʹ���ж�ͨ��
    NVIC_Init(&NVIC_InitStruct);

    // ����EXTI1�жϣ�������B�ࣩ��������EXTI0��ͬ
    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    /*---------------- FreeRTOS���д��� ----------------*/
    // ��������Ϊ20�Ķ��У�ÿ��Ԫ��Ϊint8_t���ͣ����-1/0/+1��
    xEncoderQueue = xQueueCreate(20, sizeof(int8_t));
    configASSERT(xEncoderQueue != NULL); // ������Ӷ��д���У��
}

// Ԥ������Ĳ�������״̬ת������������룩
static const int8_t encoder_states[16] = {
/* ��״̬ -> ��״̬ */ 
/* 00 -> */ 0, -1, +1, 0,    // 00, 01, 10, 11
/* 01 -> */ +1, 0, 0, -1,    // 00, 01, 10, 11
/* 10 -> */ -1, 0, 0, +1,    // 00, 01, 10, 11
/* 11 -> */ 0, +1, -1, 0     // 00, 01, 10, 11
};


/*==============================================================================
  �������жϹ�����������Encoder_ISR_Handler
  ���ܣ���ȡ��ǰ����״̬��ͨ��״̬���ж���ת���򣬷��Ͷ�����Ϣ
  ˵�����ú�����EXTI0_IRQHandler��EXTI1_IRQHandler��ͬ����
==============================================================================*/
static void Encoder_ISR_Handler(void) {
    static uint8_t lastAB = 0; // ������һ��AB��״̬����̬��������״̬��
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*---------------- ״̬�ɼ� ----------------*/
    // ��ȡ��ǰA���B���ƽ������ֵΪBit_SET��Bit_RESET��
    uint8_t currentA = GPIO_ReadInputDataBit(ENCODER_GPIO, ENCODER_A_PIN);
    uint8_t currentB = GPIO_ReadInputDataBit(ENCODER_GPIO, ENCODER_B_PIN);
    
    // ����������״̬���Ϊ2bit��ֵ��00,01,10,11��
    uint8_t currentAB = (currentA << 1) | currentB;

    /*---------------- ״̬������ ----------------*/
    // ����¾�״̬�γ�4bit������ǰ2bit����һ��״̬����2bit�ǵ�ǰ״̬��
    uint8_t stateIndex = (lastAB << 2) | currentAB;
    
    // ʹ��Ԥ�����״̬ת�����ȡ�����Ĳ���������׼�����
    // ״̬�����ԭ�����ڸ�����仯���м����Ч����
    int8_t direction = encoder_states[stateIndex];

    /*---------------- ���з��� ----------------*/
    if (direction != 0) { // ������⵽��Ч����ʱ����
        // ʹ��FromISR�汾���͵����У����ܻ�������������
        xQueueSendFromISR(xEncoderQueue, &direction, &xHigherPriorityTaskWoken);
    }

    lastAB = currentAB; // ����״̬��¼

    /*---------------- �������л��ж� ----------------*/
    // ������Ͳ��������˸������ȼ����������������л�
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/*==============================================================================
  EXTI0�жϷ�����������A�����ű仯
==============================================================================*/
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) { // ȷ���жϱ�־
        Encoder_ISR_Handler();                   // ���ù���������
        EXTI_ClearITPendingBit(EXTI_Line0);      // ����жϱ�־
    }
}

/*==============================================================================
  EXTI1�жϷ�����������B�����ű仯
==============================================================================*/
void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        Encoder_ISR_Handler();
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/*==============================================================================
  ��ȫ��ȡ����ֵ������Encoder_GetCount
  ����ֵ����ǰ���������ۼƼ���ֵ��int32_t��
  ˵����ʹ���ٽ�������ȷ����ȡʱ��ԭ����
==============================================================================*/
int32_t Encoder_GetCount(void) {
    int32_t count;
    
    taskENTER_CRITICAL(); // �����ٽ���
    count = encoderCount; // ��ȫ��ȡ
    taskEXIT_CRITICAL();
    
    return count;
}

/*==============================================================================
  ��������������Encoder_Task
  ���ܣ��Ӷ��н��շ������ݣ���ȫ�����ۼƼ���ֵ
==============================================================================*/
void Encoder_Task(void *pvParameters) {
    int8_t direction; // ���յ��ķ���ֵ��-1/+1��
    
    while (1) {
        // �����ȴ��������ݣ�portMAX_DELAY��ʾ���޵ȴ���
        if (xQueueReceive(xEncoderQueue, &direction, portMAX_DELAY) == pdPASS) {
            /*---------------- �ٽ������� ----------------*/
            // �����ٽ������ر��жϣ���֤��encoderCount��ԭ�Ӳ���
            taskENTER_CRITICAL();
            encoderCount += direction*10;
            taskEXIT_CRITICAL();
						printf("wset text2.txt \"Count: %ld\"\r\n", (long)encoderCount);
            //printf("Count: %ld\r\n", (long)encoderCount);
						TIM_SetCompare4(TIM4,encoderCount*10);//2.5ms	// ���� CCR1 �Ĵ���
						vTaskDelay(pdMS_TO_TICKS(10)); // ��ʱ 10ms
					
            /* �˴�����������������磺
               - ͨ�������������ֵ
               - ��������������
               - ���Ƽ�����Χ�� */
        }
    }
}



///*==============================================================================
//  ��������ϵͳ��ʼ������������
//==============================================================================*/
//int main(void) {
//    SystemInit(); // STM32ϵͳʱ�ӳ�ʼ���������ʵ��ʱ�����ã�
//    
//    Encoder_Init(); // ��ʼ��������Ӳ���Ͷ���
//    
//    // �����������������񣨽��������񴴽�����ֵ��
//    xTaskCreate(Encoder_Task,          // ������
//                "EncoderTask",         // �������ƣ������ã�
//                configMINIMAL_STACK_SIZE, // �����ջ��С�������ʵ�ʵ�����
//                NULL,                  // �������
//                2,                     // �������ȼ����е����ȼ���
//                NULL);                 // ������
//    
//    vTaskStartScheduler(); // ����FreeRTOS������
//    
//    // �����������󲻻᷵�أ��˴�while���ڷ�ֹ����������
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
//	GPIO_InitTypeDef GPIO_InitStructure; //����GPIO�ṹ��
//	NVIC_InitTypeDef NVIC_InitStruct;
//	EXTI_InitTypeDef EXTI_InitStruct;	
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE); 
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������ģʽ
//	GPIO_InitStructure.GPIO_Pin = EC11_PORT_PIN; //GPIO�ṹ��ָ������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��GPIO	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);	
//	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn; //�ж�Դ���ⲿ�ж�0ͨ��
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	EXTI_InitStruct.EXTI_Line =  EXTI_Line10; //�ⲿ�ж���·0
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
//	// ����жϱ�־
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
