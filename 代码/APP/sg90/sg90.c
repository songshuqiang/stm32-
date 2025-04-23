#include "sg90.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config.h"
#include "my_queue.h"
#include "dc_motor.h"

QueueHandle_t xAngleQueue = NULL;

void SG90_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    /* 1. ʹ��ʱ�� */
    RCC_APB2PeriphClockCmd(SG90_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(SG90_TIM_CLK, ENABLE);
    
    /* 2. ����GPIOΪ����������� */
    GPIO_InitStructure.GPIO_Pin = SG90_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SG90_GPIO_PORT, &GPIO_InitStructure);
    
    /* 3. ���ö�ʱ��ʱ����Ԫ */
    TIM_TimeBaseStructure.TIM_Period = SG90_PWM_PERIOD - 1;       // �Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = SG90_PWM_PRESCALER - 1; // Ԥ��Ƶϵ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(SG90_TIM, &TIM_TimeBaseStructure);
    
    /* 4. ����PWMģʽ */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // PWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = SG90_MIN_PULSE;       // ��ʼλ��0��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC4Init(SG90_TIM, &TIM_OCInitStructure);
    
    /* 5. ������ʱ�� */
    TIM_Cmd(SG90_TIM, ENABLE);
    TIM_CtrlPWMOutputs(SG90_TIM, ENABLE);  // �߼���ʱ����Ҫ�˾�
		
		xAngleQueue = xQueueCreate(4, sizeof(Sg90Data));  // �洢3���Ƕ�ֵ
		if (xAngleQueue == NULL) {
        debug_sg90("ERROR: xAngleQueue create failed!\r\n");
    } else {
        debug_sg90("xAngleQueue created successfully!\r\n");
    }
		
}

/**
  * @brief  ���ö���Ƕ�
  * @param  angle: Ŀ��Ƕȣ�0-180�ȣ�
  * @retval ��
  */
void SG90_SetAngle(float angle)
{	
	u8 set_ok = 1;
	uint16_t pulse_width;
	angle = (angle < 0) ? 0 : (angle > 180) ? 180 : angle; // �޷�
	pulse_width = SG90_MIN_PULSE + (angle / 180.0) * (SG90_MAX_PULSE - SG90_MIN_PULSE);
	TIM_SetCompare4(SG90_TIM, pulse_width); // ʹ�ÿ⺯������ CCR4
	
	xQueueSend(xDcMotorQueue, &set_ok, 0); // �������ͣ�������
}

void SG90Run(int pra)
{
	float target_angle = 1.0;
	/* �ǶȰ�ȫת�� */
	target_angle = (float)pra;
	
	/* Ӳ��������������0-180��֮�� */
	target_angle = (target_angle < 0) ? 0 : 
							 (target_angle > 180) ? 180 : target_angle;

	/* ִ�нǶȿ��� */
	SG90_SetAngle(target_angle);
	
	/* ������� */
	debug_sg90("Set Angle: %.1f��\r\n", target_angle);
	
}


/* ��������������� */
void vSG90ControlTask(void *pvParameters)
{ 
	Sg90Data data = {0};
	int i = 0;
	
	while(1)
	{
		if(xQueueReceive(xAngleQueue, &data, 10))
		{
			debug_sg90("test:%d\r\n",data.test);
			debug_sg90("angle1:%d\r\n",data.saved_angle1);
			debug_sg90("angle2:%d\r\n",data.saved_angle2);
			if(data.test == 1)
			{
				for(;i<2;i++)
				{
					SG90Run(data.saved_angle1);
					vTaskDelay(pdMS_TO_TICKS(100));
					SG90Run(data.saved_angle2);
					vTaskDelay(pdMS_TO_TICKS(100));
				}
				i = 0;
				data.test = 0;
			}
		}
		
		
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	
}
