#include "dc_motor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "my_event.h"
#include "semphr.h"
#include "my_queue.h"
#include "config.h"
#include "sg90.h"

static DcMotorData data = {0};
static SemaphoreHandle_t xMotorMutex = NULL;  // 在初始化中创建
static MotorState_t motorState = MOTOR_STATE_IDLE; //DcMotor的状态机
QueueHandle_t xDcMotorQueue = NULL; 

void DC_Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义GPIO结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //GPIO结构体指定输出模式：推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DC_Motor_PIN;
	
	RCC_APB2PeriphClockCmd(DC_Motor_PORT_RCC, ENABLE); //使能外部APB2总线时钟RCC
	GPIO_Init(DC_Motor_PORT, &GPIO_InitStructure);  //初始化GPIO
	GPIO_ResetBits(DC_Motor_PORT, DC_Motor_PIN); 
	
	xMotorMutex = xSemaphoreCreateMutex();  // 创建互斥锁
	
	xDcMotorQueue = xQueueCreate(4, sizeof(Sg90Data));  // 存储3个角度值
		if (xDcMotorQueue == NULL) {
        debug_SR04("ERROR: xDcMotorQueue create failed!\r\n");
    } else {
        debug_SR04("xDcMotorQueue created successfully!\r\n");
    }
}

void DcMotorOn(void) 
{
    if (xSemaphoreTake(xMotorMutex, pdMS_TO_TICKS(50)) == pdTRUE)
		{
        GPIO_SetBits(DC_Motor_PORT, DC_Motor_PIN);  // 假设PB0控制电机电源
        xSemaphoreGive(xMotorMutex);
    }
}

void DcMotorOff(void) 
{
    if (xSemaphoreTake(xMotorMutex, pdMS_TO_TICKS(50)) == pdTRUE) 
		{
        GPIO_ResetBits(DC_Motor_PORT, DC_Motor_PIN);
        xSemaphoreGive(xMotorMutex);
    }
}

static void DcMotorCheckEventFlag(u8 MS)
{
	data.start_flag  = My_Check_Event(EVENT_FLAG_START, MS);
	data.stop_flag	 = My_Check_Event(EVENT_FLAG_STOP, MS);
	data.run_flag    = My_Check_Event(EVENT_FLAG_RUN, MS);
	data.ready_flag1 = My_Check_Event(EVENT_FLAG_READY_1, MS);
	data.ready_flag2 = My_Check_Event(EVENT_FLAG_READY_2, MS);
	data.finish_flag1= My_Check_Event(EVENT_FLAG_FINISH_1, MS);
	data.finish_flag2= My_Check_Event(EVENT_FLAG_FINISH_2, MS);
	data.levle_flag1 = My_Check_Event(EVENT_FLAG_LEVEL_1, MS);
	data.levle_flag2 = My_Check_Event(EVENT_FLAG_LEVEL_2, MS);
	data.levle_flag3 = My_Check_Event(EVENT_FLAG_LEVEL_3, MS);
}

void dc_motor_task(void *pvParameters)
{
	Sg90Data angle_data;	
	My_Set_Event(EVENT_FLAG_RUN);
	u8 set_angle = 0;
	while (1) 
	{
		DcMotorCheckEventFlag(10);
		xQueueReceive(xDcMotorQueue, &angle_data, 10);
		if (data.stop_flag)
		{
				motorState = MOTOR_STATE_STOPPED;
		} 
		else if (data.start_flag)
		{
				if (data.ready_flag1 && !data.finish_flag1) 
				{
						motorState = MOTOR_STATE_RUNNING1;
				} 
				else if (data.ready_flag2 && !data.finish_flag2) 
				{
						motorState = MOTOR_STATE_RUNNING2;
				} 
				else 
				{
						motorState = MOTOR_STATE_IDLE;
				}
		} 
		else
		{
				motorState = MOTOR_STATE_IDLE;
		}

		switch (motorState) {
				case MOTOR_STATE_RUNNING1:
						SG90Run(angle_data.saved_angle1);
						debug_SR04("angle1:%d\r\n",angle_data.saved_angle1);
						DcMotorOn();
						break;
				case MOTOR_STATE_RUNNING2:
						SG90Run(angle_data.saved_angle2);
						debug_SR04("angle2:%d\r\n",angle_data.saved_angle2);
						if((set_angle = xQueueReceive(xDcMotorQueue, &angle_data, 10) == 1)) DcMotorOn();
						break;
				default:
						DcMotorOff();
						break;
		}

		vTaskDelay(pdMS_TO_TICKS(10));
  }
}


