#include "screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "dc_motor.h"
#include "liquid_level.h"
#include "my_queue.h"
#include "config.h"
#include "my_event.h"
#include "sg90.h"
#include "sr04.h"
#include "queue.h"
#include <stdio.h>
#include "usart3.h"

/* ��̬ȫ�ֱ�����֤���ݰ�ȫ */
static Sg90Data sg90data = {0};

QueueHandle_t xScreenCmdQueue = NULL;

void Screen_Init(void)
{
	//�������У�������USART3����䴫�ݲ���
    xScreenCmdQueue = xQueueCreate(5, sizeof(ScreenCommandType));
}


static void SendAngleToDisplay(int32_t angle)
{
	// ���д�����ʾ����
	printf_to_pinmu("wset angle_txt.txt \"%d\"\r\n", angle);
	printf_to_pinmu("wset set_angle_txt1.txt \"%d\"\r\n", sg90data.saved_angle1);
	printf_to_pinmu("wset set_angle_txt2.txt \"%d\"\r\n", sg90data.saved_angle2);
	
}

/* ˽�к���ʵ�� */
static void UpdateSG90Angle(int32_t angle)
{
    // Ӳ��������
    angle = (angle < 0) ? 0 : (angle > 180) ? 180 : angle;
    
    // ������ʾ
    SendAngleToDisplay(angle);
    
    // �������
    SG90_SetAngle((float)angle);
    
    // �������
    debug_sg90("[SG90] Current Angle: %d��\r\n", angle);
}

static void Screen_Cmd_Switch(ScreenCommandType received_cmd)
{
	switch(received_cmd)
	{
		case SCREEN_CMD_ANGLE_ADD:
				sg90data.current_angle = (sg90data.current_angle >= 170) ? 
						180 : sg90data.current_angle + 10;
				break;
				
		case SCREEN_CMD_ANGLE_SUB:
				sg90data.current_angle = (sg90data.current_angle <= 10) ? 
						0 : sg90data.current_angle - 10;
				break;
				
		case SCREEN_CMD_SAVE_POS1:
				sg90data.saved_angle1 = sg90data.current_angle;
				break;
				
		case SCREEN_CMD_SAVE_POS2:
				sg90data.saved_angle2 = sg90data.current_angle;
				break;
			
		case SCREEN_CMD_ANGLE_TEST:
				sg90data.test = 1;
				xQueueSend(xAngleQueue, &sg90data, 0); // �������ͣ�������
				break;
				
		case SCREEN_CMD_SPEAK1:
				USART3_SendString("speak1");
				break;
		
		case SCREEN_CMD_SPEAK2:
				USART3_SendString("speak2");
				break;
		
		case SCREEN_CMD_SPEAK3:
				USART3_SendString("speak3");
				break;
		
		case SCREEN_CMD_SPEAK4:
				USART3_SendString("speak4");
				break;
		
		case SCREEN_CMD_SPEAK5:
				USART3_SendString("speak5");
				break;
		
		case SCREEN_CMD_SPEAK_GET:
				Check_level();
				break;
				
		case SCREEN_CMD_MOTOR_START:
				My_Set_Event(EVENT_FLAG_START);
				xQueueSend(xDcMotorQueue, &sg90data, 0); // �������ͣ�������
				break;
		default:
			
			break;
	}
	// ͳһ���½Ƕ�
	UpdateSG90Angle(sg90data.current_angle);
}


void screen_task(void *pvParameters)
{
	ScreenCommandType received_cmd;
	
	while(1)
	{
		// ��������н���ָ�����ʽ��
		if(xQueueReceive(xScreenCmdQueue, &received_cmd, portMAX_DELAY) == pdPASS)
		{
			Screen_Cmd_Switch(received_cmd);
			
		}		
		vTaskDelay(pdMS_TO_TICKS(1)); // ��ʱ 10ms
	}
		
}


