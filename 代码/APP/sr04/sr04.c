#include "sr04.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"              // FreeRTOS����API
#include "my_queue.h"
#include "config.h"
#include "usart3.h"

/*--------------------------------- ȫ�ֱ��� -----------------------------------*/
volatile u16 TIM3_cnt = 0; // ��ʱ��4����ֵ��volatileȷ���ɼ��ԣ�
QueueHandle_t xSG04Queue = NULL;

/*******************************************************************************
* �� �� ��         : TIM3_Init
* ��������		   : TIM4��ʼ������
* ��    ��         : per:��װ��ֵ
					 psc:��Ƶϵ��
* ��    ��         : ��
*******************************************************************************/

void TIM3_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM3,DISABLE); //�رն�ʱ��
}

//SR04�˿ڳ�ʼ��
void sr04_init(void)
{
	GPIO_InitTypeDef  GPIO_InitSture;
	EXTI_InitTypeDef EXTI_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	
//	RCC_APB2PeriphClockCmd(TRIG_PORT_RCC|ECHO_PORT_RCC,ENABLE);
	/*---------------- GPIO���� ----------------*/
  // ʹ��GPIOB��AFIOʱ�ӣ�AFIO�����ⲿ�ж���·ӳ�䣩
  RCC_APB2PeriphClockCmd(TRIG_PORT_RCC | ECHO_PORT_RCC | RCC_APB2Periph_AFIO, ENABLE);
	
	//����IO�˿�
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitSture.GPIO_Pin=TRIG_PIN;
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(TRIG_PORT,&GPIO_InitSture);
	
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitSture.GPIO_Pin=ECHO_PIN;
	GPIO_Init(ECHO_PORT,&GPIO_InitSture);
	
	/*---------------- �ⲿ�ж����� ----------------*/
	// ��PA1->ECHOӳ�䵽EXTI1��·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	// ����EXTI������˫�ش�����ʹ���ж�
	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;   // �ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ˫�ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;            // ʹ����·
	EXTI_Init(&EXTI_InitStruct);
	
	/*---------------- NVIC���� ----------------*/
	// ����EXTI1�ж�
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	// �ؼ����ã�������ռ���ȼ�������configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
	// ȷ���������ж��а�ȫ����FreeRTOS��FromISR����
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;   // �����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
	TIM3_Init(50000,71);
	
	
	xSG04Queue = xQueueCreate(1, sizeof(float));  // �洢��������ֵ
	if (xSG04Queue == NULL) {
			debug_SR04("ERROR: xAngleQueue create failed!\r\n");
	} else {
			debug_SR04("xAngleQueue created successfully!\r\n");
	}
}



/*==============================================================================
  EXTI1�жϷ�����������PA1�����ű仯
==============================================================================*/
void EXTI1_IRQHandler(void)
{
	float distence = 0.0; 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		if(ECHO==1)
		{
			TIM_SetCounter(TIM3,0);//���������ֵ
			TIM_Cmd(TIM3,ENABLE);//������ʱ������ʼ����
		}
		if(ECHO==0)
		{
			TIM_Cmd(TIM3,DISABLE);//�رն�ʱ��
			TIM3_cnt=TIM_GetCounter(TIM3);
			distence = (float)TIM3_cnt*340/20000.0;
			xQueueSendFromISR(xSG04Queue, &distence, &xHigherPriorityTaskWoken);
			
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
		/*---------------- �������л��ж� ----------------*/
    // ������Ͳ��������˸������ȼ����������������л�
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}


void sr04_task(void *pvParameters)
{
	float distence = 0.0; 
//	unsigned char data[] = {0xAA,0x00,0x00,0x00,0x00,0x00,0x01};

	while(1)
	{	
		TRIG=1;	//����
		vTaskDelay(pdMS_TO_TICKS(20)); // ��ʱ 20ms 
		TRIG=0;	//����
		vTaskDelay(pdMS_TO_TICKS(200)); // ��ʱ 20ms 
//		printf("wset sg04_text.txt \"%d\"\r\n",TIM3_cnt*340/20000);
		if(xQueueReceive(xSG04Queue, &distence,0)== pdPASS) debug_SR04("wset sg04_txt.txt \"%.2f\"\r\n",distence);
		

		
	}
	
	
}


