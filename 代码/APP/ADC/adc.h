#ifndef __ADC_H
#define __ADC_H

#include "FreeRTOS.h"           // FreeRTOS����ͷ�ļ�
#include "queue.h"

/* Ӳ������ */
#define SAMPLE_COUNT   10    // DMA���δ���Ĳ�����������߿������ԣ�
#define FILTER_SIZE     5    // �ƶ�ƽ���˲����ڴ�С��ƽ��ʵʱ�����ȶ��ԣ�

/* ȫ�ֶ��ж��� */
extern QueueHandle_t xDistanceQueue;  // ���ڴ���ADCֵ��FreeRTOS����

/* �������� */
void ADCx_Init(void);                     // ADC��ʼ������
void ADCTask(void *pvParameters);
void DistanceTask(void *pvParameters);
uint16_t Get_Filtered_ADC(void);          // ��ȡ�˲����ADCֵ

#endif
