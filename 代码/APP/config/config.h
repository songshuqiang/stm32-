#ifndef config_H
#define config_H


#include "FreeRTOS.h"
#include "queue.h"


#define PINMU_BUF_SIZE          128    // ��������С

#define PRINTF_TO_PINMU_ENABLED 		  1 //��Ļ��ʾ��Ϣ
#define DEBUG_MyEventGroup					  0 //���¼�����Ϣ
#define DEBUG_MyEventGroup_BY_BITS		1 //���¼�����Ϣ
#define DEBUG_DISTENCE								0 //��ӡ����ֵ��Ϣ
#define PRINTF_LEVEL   								0 //�򿪴���ֵ
#define DEBUG_IN_SG90   							1 //��SG90����
#define DEBUG_IN_DC_MOTOR							1 //��DC_MOTOR����
#define DEBUG_IN_SR04									0 //��SG04����
#define DEBUG_IN_ADC									0 //��ADC����


#define EVENT_FLAG_START				(1 << 0)	//��ʼ���������¼�λ
#define EVENT_FLAG_STOP 				(1 << 1)	//ֹͣ�����¼�λ
#define EVENT_FLAG_RUN 					(1 << 2)	//�����¼�λ
#define EVENT_FLAG_READY_1 			(1 << 3)	//ȡ�ϱ�1�����¼�λ
#define EVENT_FLAG_READY_2 			(1 << 4)	//ȡ�ϱ�2�����¼�λ
#define EVENT_FLAG_FINISH_1 		(1 << 5)	//ȡ�ϱ�1����¼�λ
#define EVENT_FLAG_FINISH_2 		(1 << 6)	//ȡ�ϱ�2����¼�λ
#define EVENT_FLAG_LEVEL_1 			(1 << 7)	//Һλ80%�����¼�λ
#define EVENT_FLAG_LEVEL_2 			(1 << 8)	//Һλ50%�����¼�λ
#define EVENT_FLAG_LEVEL_3 			(1 << 9)	//Һλ20%�����¼�λ
                                          
extern QueueHandle_t xScreenCmdQueue;

/* ��Ļָ������ */
typedef enum {
	SCREEN_CMD_ANGLE_ADD,    			// �Ƕ�����
	SCREEN_CMD_ANGLE_SUB,    			// �Ƕȼ���
	SCREEN_CMD_SAVE_POS1,   		  // ����λ��1
	SCREEN_CMD_SAVE_POS2,    		  // ����λ��2
	SCREEN_CMD_ANGLE_TEST,   			// �ǶȲ���
	SCREEN_CMD_MOTOR_START,  			// �������
	SCREEN_CMD_SPEAK1,   		 			// ��������1
	SCREEN_CMD_SPEAK2,   		 			// ��������2
	SCREEN_CMD_SPEAK3,   		 			// ��������3
	SCREEN_CMD_SPEAK4,   		 			// ��������4
	SCREEN_CMD_SPEAK5,   		 			// ��������5
	SCREEN_CMD_SPEAK_GET,					// ����ʶ��
	SCREEN_CMD_JUICE_CHANGE   		// ���ϸı�
	
} ScreenCommandType;


/* Sg90���ݽṹ��,ʹ��GCC/ARMCC����ָ������*/
typedef struct __attribute__((aligned(4))) {
	int32_t current_angle;   // ��ǰ�Ƕ�
	int32_t saved_angle1;    // Ԥ��Ƕ�1
	int32_t saved_angle2;    // Ԥ��Ƕ�2
	int32_t test;            // ���Ա�־
} Sg90Data;

/* dc_motor���ݽṹ��,ʹ��GCC/ARMCC����ָ������*/
typedef struct __attribute__((aligned(4))) {
	BaseType_t start_flag;
	BaseType_t run_flag;
	BaseType_t ready_flag1;
	BaseType_t ready_flag2;
	BaseType_t stop_flag;
	BaseType_t finish_flag1;
	BaseType_t finish_flag2; 
	BaseType_t levle_flag1; 
	BaseType_t levle_flag2; 
	BaseType_t levle_flag3; 
} DcMotorData;

typedef struct
{
	u16 distence;
	int angle_1;
	int angle_2;
	u32 angle;
}MyData;

//DcMotor��״̬��
typedef enum {
    MOTOR_STATE_IDLE,
    MOTOR_STATE_RUNNING1,
    MOTOR_STATE_RUNNING2,
    MOTOR_STATE_STOPPED
} MotorState_t;


void Config_init(void);
void debug_printf(const char *format, ...);
void printf_to_pinmu(const char *format, ...);
void printf_distence(const char *format, ...);
void debug_printf_level(const char *format, ...);
void debug_sg90(const char *format, ...);
void debug_dc_motor(const char *format, ...);
void debug_SR04(const char *format, ...);
void debug_ADC(const char *format, ...);


#endif
