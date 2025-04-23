#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "config.h"
#include "dc_motor.h"

// 添加互斥锁保护（FreeRTOS环境）
static SemaphoreHandle_t xPinmuMutex = NULL;

void Config_init(void) {
    xPinmuMutex = xSemaphoreCreateMutex(); // 初始化互斥锁
}

static void debug_vprintf(const char *format, va_list args) 
{
	char buf[PINMU_BUF_SIZE];
	int len;
	int i;
	if (xPinmuMutex == NULL) return;
	len = vsnprintf(buf, sizeof(buf), format, args);
	if (len > 0) {
			if (xSemaphoreTake(xPinmuMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
					for (i = 0; i < len; i++) {
							while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
							USART_SendData(USART1, buf[i]);
					}
					xSemaphoreGive(xPinmuMutex);
			}
	}

}


void printf_to_pinmu(const char *format, ...)
{
	#if PRINTF_TO_PINMU_ENABLED
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif	
}


void printf_distence(const char *format, ...)
{
	#if DEBUG_DISTENCE
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);	
	#endif	
}

void debug_printf_level(const char *format, ...)
{
	#if PRINTF_LEVEL
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif
}

void debug_sg90(const char *format, ...)
{
	#if DEBUG_IN_SG90
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif
	
}
void debug_dc_motor(const char *format, ...)
{
	#if DEBUG_IN_DC_MOTOR
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif
	
}
void debug_SR04(const char *format, ...)
{
	#if DEBUG_IN_SR04
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif
	
}
void debug_ADC(const char *format, ...)
{
	#if DEBUG_IN_ADC
			va_list args;
			va_start(args, format);
			debug_vprintf(format, args);
			va_end(args);
	#endif
	
}
