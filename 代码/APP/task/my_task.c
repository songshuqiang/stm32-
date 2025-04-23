#include "my_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "screen.h"
#include "dc_motor.h"
#include "ec11.h"
#include "pwm.h"
#include "sr04.h"
#include "sg90.h"
#include "Infrared.h"
#include "liquid_level.h"
#include "adc.h"


//任务优先级
#define   START_TASK_PRIO								1
#define 	LED1_TASK_PRIO								2
#define 	KEY_TASK_PRIO									2
#define 	SCREEN_TASK_PRIO							4
#define		DC_MOTOR_TASK_PRIO						4
#define		SG90_TASK_PRIO								2
#define		SR04_TASK_PRIO								2
#define		INFRATED_TASK_PRIO						2
#define		LIQUID_LEVEL_TASK_PRIO				2

//任务堆栈大小	
#define 	START_STK_SIZE 								128 
#define 	LED1_STK_SIZE 								50 
#define 	KEY_STK_SIZE 									50 
#define  	SCREEN_STK_SIZE 							256 
#define  	DC_MOTOR_STK_SIZE 						128 
#define  	SG90_STK_SIZE 								128 
#define  	SR04_STK_SIZE 								128 
#define  	INFRATED_STK_SIZE 						50
#define  	LIQUID_LEVEL_STK_SIZE 				100

//任务句柄
TaskHandle_t StartTask_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t KEYTask_Handler;
TaskHandle_t SCREENTask_Handler;
TaskHandle_t DC_MOTORTask_Handler;
TaskHandle_t SG90Task_Handler;
TaskHandle_t SR04Task_Handler;
TaskHandle_t INFRATEDTask_Handler;
TaskHandle_t LIQUID_LEVELTask_Handler;

u8 cnt = 0;

void create_task(void)
{
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
	
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

		//创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
		
		//创建KEY任务
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler); 
								
		//创建SCREEN任务
    xTaskCreate((TaskFunction_t )screen_task,     
                (const char*    )"screen_task",   
                (uint16_t       )SCREEN_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SCREEN_TASK_PRIO,
                (TaskHandle_t*  )&SCREENTask_Handler); 
								
		//创建DC_MOTOR任务
    xTaskCreate((TaskFunction_t )dc_motor_task,     
                (const char*    )"dc_motor_task",   
                (uint16_t       )DC_MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DC_MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&DC_MOTORTask_Handler);
								
		//创建SG90舵机任务
		xTaskCreate(vSG90ControlTask, "SG90 Ctrl", configMINIMAL_STACK_SIZE, NULL, 5, NULL);	
//    xTaskCreate((TaskFunction_t )sg90_task,     
//                (const char*    )"sg90_task",   
//                (uint16_t       )configMINIMAL_STACK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )SG90_TASK_PRIO,
//                (TaskHandle_t*  )&SG90Task_Handler);
								
		//创建SR04超声测距任务
    xTaskCreate((TaskFunction_t )sr04_task,     
                (const char*    )"sr04_task",   
                (uint16_t       )SR04_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SR04_TASK_PRIO,
                (TaskHandle_t*  )&SR04Task_Handler);
								
		//创建INFRARED红外测距任务
    xTaskCreate((TaskFunction_t )infrared_task,     
                (const char*    )"infrared_task",   
                (uint16_t       )INFRATED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRATED_TASK_PRIO,
                (TaskHandle_t*  )&INFRATEDTask_Handler);
								
		//创建Iliquid_level液位检测任务
    xTaskCreate((TaskFunction_t )liquid_level_task,     
                (const char*    )"liquid_level_task",   
                (uint16_t       )LIQUID_LEVEL_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LIQUID_LEVEL_TASK_PRIO,
                (TaskHandle_t*  )&LIQUID_LEVELTask_Handler);
								
								
    
								
//    xTaskCreate(ADCTask,        // 任务函数
//                "ADC Task",     // 任务名称
//                256,            // 堆栈大小（字）
//                NULL, 
//                3,              // 优先级（数字越大优先级越高）
//                NULL);

//    xTaskCreate(DistanceTask, 
//                "Distance Task", 
//                256, 
//                NULL, 
//                2,              // 较低优先级
//                NULL);
		
		
//								
//    // 创建EC11编码器处理任务（建议检查任务创建返回值）
//    xTaskCreate(Encoder_Task,          // 任务函数
//                "EncoderTask",         // 任务名称（调试用）
//                configMINIMAL_STACK_SIZE, // 任务堆栈大小（需根据实际调整）
//                NULL,                  // 任务参数
//                2,                     // 任务优先级（中等优先级）
//                NULL);                 // 任务句柄
//								
	
		vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
} 

//LED1任务函数
void led1_task(void *pvParameters)
{
	while(1)
	{
		LED13=0;
		vTaskDelay(200);
		LED13=1;
		vTaskDelay(800);
//		printf("wset text1.txt \"%d\"\r\n",cnt++);
//		 printf("wset text1.txt \"%f\"\r\n",(float)TIM3_cnt*340/20000.0);
	}
}


