/**************************************************
 * SG90�����������
 * Ӳ����STM32F10x + ��׼�����
 * ƽ̨��Keil + FreeRTOS
 * �汾��v1.0
 * ˵�����ṩ�Ƕȿ��ƽӿ�
 **************************************************/
 
#ifndef __SG90_H
#define __SG90_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "FreeRTOS.h"
#include "queue.h"

/* Ӳ�����ú� -----------------------------------------------------------*/
// PWM��ʱ��ѡ��TIM1-TIM4��ע��ʱ����������
#define SG90_TIM               TIM4
#define SG90_TIM_CLK           RCC_APB1Periph_TIM4

// PWM���ͨ������
#define SG90_TIM_CHANNEL       TIM_Channel_4  // ͨ����
#define SG90_CCR               CCR4          // ����ȽϼĴ���

// GPIO����
#define SG90_GPIO_CLK          RCC_APB2Periph_GPIOB
#define SG90_GPIO_PORT         GPIOB
#define SG90_GPIO_PIN          GPIO_Pin_9

// PWM���ڲ�����20ms���ڣ�
#define SG90_PWM_PERIOD        20000   // 20ms @1MHzʱ��
#define SG90_PWM_PRESCALER     71      // 72MHz/(71+1)=1MHz

/* ������� ------------------------------------------------------------*/
#define SG90_MIN_PULSE         500     // 0�ȶ�Ӧ0.5ms���壨500us��
#define SG90_MAX_PULSE         2500    // 180�ȶ�Ӧ2.5ms����
#define SG90_ANGLE_RANGE       180.0f  // ��Ч�Ƕȷ�Χ



extern QueueHandle_t xAngleQueue;

/* ����ԭ�� ------------------------------------------------------------*/
void SG90_Init(void);                      // �����ʼ��
void SG90_SetAngle(float angle);          // �Ƕ����ú���
void vSG90ControlTask(void *pvParameters);
void SG90Run(int pra);
#ifdef __cplusplus
}
#endif

#endif /* __SG90_H */
