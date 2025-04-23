#include "adc.h"
#include "stm32f10x_adc.h"  // ADC�⺯������
#include "stm32f10x_dma.h"  // DMA�⺯������
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "math.h"
#include "config.h"

/* ȫ�ֱ������� */
volatile uint16_t adc_buffer[SAMPLE_COUNT];  // DMAĿ�껺������volatile��ֹ�������Ż���
uint16_t filter_buffer[FILTER_SIZE];         // ����˲�������
uint8_t filter_index = 0;                    // �˲���������ǰ����

QueueHandle_t xDistanceQueue = NULL;


/**
  * @brief  ADC��ʼ����������DMA���ã�
  * @param  ��
  * @retval ��
  * @note   ����PB1ΪADC���룬����DMA1ͨ��1ʵ���Զ����ݴ���
  */
void ADCx_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;

    /* ʱ��ʹ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE); // GPIOB��ADC1ʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                         // DMA1ʱ��

    /* GPIO���� */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;       // PB1���ţ���ӦADC1ͨ��9��
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;   // ģ������ģʽ
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* DMA���� */
    DMA_DeInit(DMA1_Channel1);                   // ��λDMA1ͨ��1
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  // �����ַ��ADC���ݼĴ���
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;      // �ڴ��ַ���Զ��建����
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                // ���䷽��������ڴ�
    DMA_InitStruct.DMA_BufferSize = SAMPLE_COUNT;                  // ����������
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // �����ַ������
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;           // �ڴ��ַ����
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16λ����
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;                   // ѭ��ģʽ
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;               // �����ȼ�
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;                      // �����ڴ浽�ڴ�
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);                      // ��ʼ��DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);                                // ����DMAͨ��

    /* ADC���� */
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                // ����ģʽ
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                     // ����ɨ��ģʽ����ͨ����
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;                // ����ת��ģʽ
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// �������
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;            // �����Ҷ���
    ADC_InitStruct.ADC_NbrOfChannel = 1;                           // ת��ͨ����
    ADC_Init(ADC1, &ADC_InitStruct);                               // ��ʼ��ADC1

    /* ͨ������ */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5); // ͨ��9������ʱ��239.5����

    /* ����DMA */
    ADC_DMACmd(ADC1, ENABLE);       // ʹ��ADC DMA����
    ADC_Cmd(ADC1, ENABLE);          // ʹ��ADC1

    /* ADCУ׼���� */
    ADC_ResetCalibration(ADC1);     // ��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1)); // �ȴ���λ���
    ADC_StartCalibration(ADC1);     // ��ʼУ׼
    while(ADC_GetCalibrationStatus(ADC1));      // �ȴ�У׼���

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ����ADC����ת��
		
		/* ����FreeRTOS���� */
		xDistanceQueue = xQueueCreate(5, sizeof(uint16_t)); // ���г���5�����ݴ�С2�ֽ�
}

/**
  * @brief  �ƶ�ƽ���˲�����
  * @param  new_val: �²�����ADCԭʼֵ
  * @retval �˲����ADCֵ
  * @note   ʹ�û��λ�����ʵ�ֻ�������ƽ��
  */
uint16_t Moving_Average_Filter(uint16_t new_val) {
	uint8_t i;
	uint32_t sum = 0;
  filter_buffer[filter_index] = new_val;                // ������ֵ
	filter_index = (filter_index + 1) % FILTER_SIZE;      // �������������λ��壩
	
	for(i=0; i<FILTER_SIZE; i++) {
			sum += filter_buffer[i];                          // �ۼӴ���������ֵ
	}
	return (uint16_t)(sum / FILTER_SIZE);                 // ����ƽ��ֵ
}

//��ȡ�˲����ADCֵ��˫���˲��������ADCֵ���ȶ�DMA�������Ķ�β���ȡƽ�����ٽ����ƶ�ƽ���˲�
uint16_t Get_Filtered_ADC(void) {
	uint8_t i;
	uint32_t sum = 0;
	// ����DMA��������ƽ��ֵ��Ӳ�����˲���
	for(i=0; i<SAMPLE_COUNT; i++) {
			sum += adc_buffer[i];
	}
	// ��������˲�
	return Moving_Average_Filter((uint16_t)(sum / SAMPLE_COUNT));
}

//����������񣺴Ӷ��л�ȡADCֵ�������ѹ��ת��Ϊ���룬ͨ���������
void DistanceTask(void *pvParameters) {
    float voltage, distance;
    uint16_t adc_value;
    
    while(1) {
        // �ȴ��������ݣ�����ʽ��
        if(xQueueReceive(xDistanceQueue, &adc_value, portMAX_DELAY) == pdTRUE) {
            /* ADCֵת��ѹ������VREF=3.3V�� */
            voltage = (adc_value * 3.3f) / 4095.0f;  // 12λADCֵת��
            
            /* ���������㣨ʾ����ʽ����У׼�� */
            if(voltage < 0.5) {
                distance = 80.0f;  // ��������
            } else {
                // ���鹫ʽ��distance = A * voltage^B
                distance = 27.726f * pow(voltage, -1.2045f);
            }
            
            /* ͨ����������������ʵ��USARTx_Init�� */
            debug_ADC("ADC: %d, Distance: %.2f cm\r\n", adc_value, distance);
        }
    }
}

//ADC�ɼ��������ڻ�ȡ�˲����ADCֵ�����͵�����
void ADCTask(void *pvParameters) {
    uint16_t filtered_adc;
    
    while(1) {
        filtered_adc = Get_Filtered_ADC();      // ��ȡ˫���˲����ֵ
        xQueueSend(xDistanceQueue, &filtered_adc, 0);  // ���͵����У���������
        vTaskDelay(pdMS_TO_TICKS(20));         // 20ms�������ڣ�50Hz��
    }
}


