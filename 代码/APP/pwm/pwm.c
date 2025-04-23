#include "pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ec11.h"
#include "screen.h"

void TIM4_CH4_PWM_Init(u16 per,u16 psc)
{
	//����ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	//�ҽ�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��GPIO�����AFIO���ù���ģ��ʱ��ʹ��
	//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��ʱ����������
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_InitStructure.TIM_Period = per;                                         //�Զ���װֵ
	TIM_InitStructure.TIM_Prescaler = psc;                                        //ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInit(TIM4,&TIM_InitStructure);
	
	
	//PWM����
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;  // ��ʼռ�ձ� 0% (0/1000)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable); //ʹ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
}

void sg90_task(void *pvParameters)
{
	int last_data =0;
	while(1)
	{
		if(last_data != data) 
		{
			TIM_SetCompare4(TIM4,data*10);//2.5ms	// ���� CCR1 �Ĵ���
			last_data = data;
		}
		vTaskDelay(pdMS_TO_TICKS(10)); // ��ʱ 10ms
	}
}
