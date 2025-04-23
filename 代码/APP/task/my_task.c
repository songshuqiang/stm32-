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


//�������ȼ�
#define   START_TASK_PRIO								1
#define 	LED1_TASK_PRIO								2
#define 	KEY_TASK_PRIO									2
#define 	SCREEN_TASK_PRIO							4
#define		DC_MOTOR_TASK_PRIO						4
#define		SG90_TASK_PRIO								2
#define		SR04_TASK_PRIO								2
#define		INFRATED_TASK_PRIO						2
#define		LIQUID_LEVEL_TASK_PRIO				2

//�����ջ��С	
#define 	START_STK_SIZE 								128 
#define 	LED1_STK_SIZE 								50 
#define 	KEY_STK_SIZE 									50 
#define  	SCREEN_STK_SIZE 							256 
#define  	DC_MOTOR_STK_SIZE 						128 
#define  	SG90_STK_SIZE 								128 
#define  	SR04_STK_SIZE 								128 
#define  	INFRATED_STK_SIZE 						50
#define  	LIQUID_LEVEL_STK_SIZE 				100

//������
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
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
	
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

		//����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
		
		//����KEY����
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler); 
								
		//����SCREEN����
    xTaskCreate((TaskFunction_t )screen_task,     
                (const char*    )"screen_task",   
                (uint16_t       )SCREEN_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SCREEN_TASK_PRIO,
                (TaskHandle_t*  )&SCREENTask_Handler); 
								
		//����DC_MOTOR����
    xTaskCreate((TaskFunction_t )dc_motor_task,     
                (const char*    )"dc_motor_task",   
                (uint16_t       )DC_MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DC_MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&DC_MOTORTask_Handler);
								
		//����SG90�������
		xTaskCreate(vSG90ControlTask, "SG90 Ctrl", configMINIMAL_STACK_SIZE, NULL, 5, NULL);	
//    xTaskCreate((TaskFunction_t )sg90_task,     
//                (const char*    )"sg90_task",   
//                (uint16_t       )configMINIMAL_STACK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )SG90_TASK_PRIO,
//                (TaskHandle_t*  )&SG90Task_Handler);
								
		//����SR04�����������
    xTaskCreate((TaskFunction_t )sr04_task,     
                (const char*    )"sr04_task",   
                (uint16_t       )SR04_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SR04_TASK_PRIO,
                (TaskHandle_t*  )&SR04Task_Handler);
								
		//����INFRARED����������
    xTaskCreate((TaskFunction_t )infrared_task,     
                (const char*    )"infrared_task",   
                (uint16_t       )INFRATED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRATED_TASK_PRIO,
                (TaskHandle_t*  )&INFRATEDTask_Handler);
								
		//����Iliquid_levelҺλ�������
    xTaskCreate((TaskFunction_t )liquid_level_task,     
                (const char*    )"liquid_level_task",   
                (uint16_t       )LIQUID_LEVEL_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LIQUID_LEVEL_TASK_PRIO,
                (TaskHandle_t*  )&LIQUID_LEVELTask_Handler);
								
								
    
								
//    xTaskCreate(ADCTask,        // ������
//                "ADC Task",     // ��������
//                256,            // ��ջ��С���֣�
//                NULL, 
//                3,              // ���ȼ�������Խ�����ȼ�Խ�ߣ�
//                NULL);

//    xTaskCreate(DistanceTask, 
//                "Distance Task", 
//                256, 
//                NULL, 
//                2,              // �ϵ����ȼ�
//                NULL);
		
		
//								
//    // ����EC11�������������񣨽��������񴴽�����ֵ��
//    xTaskCreate(Encoder_Task,          // ������
//                "EncoderTask",         // �������ƣ������ã�
//                configMINIMAL_STACK_SIZE, // �����ջ��С�������ʵ�ʵ�����
//                NULL,                  // �������
//                2,                     // �������ȼ����е����ȼ���
//                NULL);                 // ������
//								
	
		vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
} 

//LED1������
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


