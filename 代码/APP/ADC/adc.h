#ifndef __ADC_H
#define __ADC_H

#include "FreeRTOS.h"           // FreeRTOS核心头文件
#include "queue.h"

/* 硬件配置 */
#define SAMPLE_COUNT   10    // DMA单次传输的采样次数（提高抗干扰性）
#define FILTER_SIZE     5    // 移动平均滤波窗口大小（平衡实时性与稳定性）

/* 全局队列定义 */
extern QueueHandle_t xDistanceQueue;  // 用于传递ADC值的FreeRTOS队列

/* 函数声明 */
void ADCx_Init(void);                     // ADC初始化函数
void ADCTask(void *pvParameters);
void DistanceTask(void *pvParameters);
uint16_t Get_Filtered_ADC(void);          // 获取滤波后的ADC值

#endif
