/**************************************************
 * SG90舵机控制驱动
 * 硬件：STM32F10x + 标准外设库
 * 平台：Keil + FreeRTOS
 * 版本：v1.0
 * 说明：提供角度控制接口
 **************************************************/
 
#ifndef __SG90_H
#define __SG90_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "FreeRTOS.h"
#include "queue.h"

/* 硬件配置宏 -----------------------------------------------------------*/
// PWM定时器选择（TIM1-TIM4，注意时钟总线区别）
#define SG90_TIM               TIM4
#define SG90_TIM_CLK           RCC_APB1Periph_TIM4

// PWM输出通道配置
#define SG90_TIM_CHANNEL       TIM_Channel_4  // 通道号
#define SG90_CCR               CCR4          // 捕获比较寄存器

// GPIO配置
#define SG90_GPIO_CLK          RCC_APB2Periph_GPIOB
#define SG90_GPIO_PORT         GPIOB
#define SG90_GPIO_PIN          GPIO_Pin_9

// PWM周期参数（20ms周期）
#define SG90_PWM_PERIOD        20000   // 20ms @1MHz时钟
#define SG90_PWM_PRESCALER     71      // 72MHz/(71+1)=1MHz

/* 舵机参数 ------------------------------------------------------------*/
#define SG90_MIN_PULSE         500     // 0度对应0.5ms脉冲（500us）
#define SG90_MAX_PULSE         2500    // 180度对应2.5ms脉冲
#define SG90_ANGLE_RANGE       180.0f  // 有效角度范围



extern QueueHandle_t xAngleQueue;

/* 函数原型 ------------------------------------------------------------*/
void SG90_Init(void);                      // 舵机初始化
void SG90_SetAngle(float angle);          // 角度设置函数
void vSG90ControlTask(void *pvParameters);
void SG90Run(int pra);
#ifdef __cplusplus
}
#endif

#endif /* __SG90_H */
