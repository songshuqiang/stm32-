#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "usart3.h"
#include "key.h"
#include "my_task.h"
#include "dc_motor.h"
#include "ec11.h"
#include "pwm.h"
#include "Infrared.h"
#include "sr04.h"
#include "sg90.h"
#include "liquid_level.h"
#include "my_queue.h"
#include "my_event.h"
#include "adc.h"
#include "screen.h"
#include "config.h"


/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	// �� main() �������ж����ȼ�,�� USART1 �ж����ȼ����� FreeRTOS ������жϣ������������ȳ�ͻ��
	NVIC_SetPriority(USART1_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	Config_init();
	Screen_Init();
	USART1_Init(115200);
	USART3_Init(9600);
	
	MyQueue_Init();
	MyEventGroup_Init();
	
	DC_Motor_Init();
	LED_Init();
	KEY_Init();
	
//	ADCx_Init();
	
//	Encoder_Init();
//	EC11_Init();
	
	Infrared_Init();
	sr04_init();
	SG90_Init();
	liquid_level_Init();

//	TIM4_CH4_PWM_Init(10000-1, 72-1); // PWM ���� = 1000 / 1MHz = 1ms �� 1kHz   ,  72MHz / 72 = 1MHz
	create_task();
	
	
	while(1);
	
}




