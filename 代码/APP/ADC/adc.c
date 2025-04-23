#include "adc.h"
#include "stm32f10x_adc.h"  // ADC库函数声明
#include "stm32f10x_dma.h"  // DMA库函数声明
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "math.h"
#include "config.h"

/* 全局变量定义 */
volatile uint16_t adc_buffer[SAMPLE_COUNT];  // DMA目标缓冲区（volatile防止编译器优化）
uint16_t filter_buffer[FILTER_SIZE];         // 软件滤波缓冲区
uint8_t filter_index = 0;                    // 滤波缓冲区当前索引

QueueHandle_t xDistanceQueue = NULL;


/**
  * @brief  ADC初始化函数（含DMA配置）
  * @param  无
  * @retval 无
  * @note   配置PB1为ADC输入，启用DMA1通道1实现自动数据传输
  */
void ADCx_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;

    /* 时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE); // GPIOB和ADC1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                         // DMA1时钟

    /* GPIO配置 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;       // PB1引脚（对应ADC1通道9）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;   // 模拟输入模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* DMA配置 */
    DMA_DeInit(DMA1_Channel1);                   // 复位DMA1通道1
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  // 外设地址：ADC数据寄存器
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;      // 内存地址：自定义缓冲区
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                // 传输方向：外设→内存
    DMA_InitStruct.DMA_BufferSize = SAMPLE_COUNT;                  // 传输数据量
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;           // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位数据
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;                   // 循环模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;               // 高优先级
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;                      // 禁用内存到内存
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);                      // 初始化DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);                                // 启用DMA通道

    /* ADC配置 */
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                // 独立模式
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                     // 禁用扫描模式（单通道）
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;                // 连续转换模式
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// 软件触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;            // 数据右对齐
    ADC_InitStruct.ADC_NbrOfChannel = 1;                           // 转换通道数
    ADC_Init(ADC1, &ADC_InitStruct);                               // 初始化ADC1

    /* 通道配置 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5); // 通道9，采样时间239.5周期

    /* 启用DMA */
    ADC_DMACmd(ADC1, ENABLE);       // 使能ADC DMA请求
    ADC_Cmd(ADC1, ENABLE);          // 使能ADC1

    /* ADC校准流程 */
    ADC_ResetCalibration(ADC1);     // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1)); // 等待复位完成
    ADC_StartCalibration(ADC1);     // 开始校准
    while(ADC_GetCalibrationStatus(ADC1));      // 等待校准完成

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC连续转换
		
		/* 创建FreeRTOS队列 */
		xDistanceQueue = xQueueCreate(5, sizeof(uint16_t)); // 队列长度5，数据大小2字节
}

/**
  * @brief  移动平均滤波函数
  * @param  new_val: 新采样的ADC原始值
  * @retval 滤波后的ADC值
  * @note   使用环形缓冲区实现滑动窗口平均
  */
uint16_t Moving_Average_Filter(uint16_t new_val) {
	uint8_t i;
	uint32_t sum = 0;
  filter_buffer[filter_index] = new_val;                // 存入新值
	filter_index = (filter_index + 1) % FILTER_SIZE;      // 更新索引（环形缓冲）
	
	for(i=0; i<FILTER_SIZE; i++) {
			sum += filter_buffer[i];                          // 累加窗口内所有值
	}
	return (uint16_t)(sum / FILTER_SIZE);                 // 返回平均值
}

//获取滤波后的ADC值（双重滤波后的最终ADC值）先对DMA缓冲区的多次采样取平均，再进行移动平均滤波
uint16_t Get_Filtered_ADC(void) {
	uint8_t i;
	uint32_t sum = 0;
	// 计算DMA缓冲区的平均值（硬件级滤波）
	for(i=0; i<SAMPLE_COUNT; i++) {
			sum += adc_buffer[i];
	}
	// 进行软件滤波
	return Moving_Average_Filter((uint16_t)(sum / SAMPLE_COUNT));
}

//距离计算任务：从队列获取ADC值，计算电压并转换为距离，通过串口输出
void DistanceTask(void *pvParameters) {
    float voltage, distance;
    uint16_t adc_value;
    
    while(1) {
        // 等待队列数据（阻塞式）
        if(xQueueReceive(xDistanceQueue, &adc_value, portMAX_DELAY) == pdTRUE) {
            /* ADC值转电压（假设VREF=3.3V） */
            voltage = (adc_value * 3.3f) / 4095.0f;  // 12位ADC值转换
            
            /* 红外距离计算（示例公式，需校准） */
            if(voltage < 0.5) {
                distance = 80.0f;  // 超出量程
            } else {
                // 经验公式：distance = A * voltage^B
                distance = 27.726f * pow(voltage, -1.2045f);
            }
            
            /* 通过串口输出结果（需实现USARTx_Init） */
            debug_ADC("ADC: %d, Distance: %.2f cm\r\n", adc_value, distance);
        }
    }
}

//ADC采集任务：周期获取滤波后的ADC值并发送到队列
void ADCTask(void *pvParameters) {
    uint16_t filtered_adc;
    
    while(1) {
        filtered_adc = Get_Filtered_ADC();      // 获取双重滤波后的值
        xQueueSend(xDistanceQueue, &filtered_adc, 0);  // 发送到队列（非阻塞）
        vTaskDelay(pdMS_TO_TICKS(20));         // 20ms任务周期（50Hz）
    }
}


