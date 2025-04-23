#ifndef config_H
#define config_H


#include "FreeRTOS.h"
#include "queue.h"


#define PINMU_BUF_SIZE          128    // 缓冲区大小

#define PRINTF_TO_PINMU_ENABLED 		  1 //屏幕显示信息
#define DEBUG_MyEventGroup					  0 //打开事件组信息
#define DEBUG_MyEventGroup_BY_BITS		1 //打开事件组信息
#define DEBUG_DISTENCE								0 //打印测量值信息
#define PRINTF_LEVEL   								0 //打开储料值
#define DEBUG_IN_SG90   							1 //打开SG90调试
#define DEBUG_IN_DC_MOTOR							1 //打开DC_MOTOR调试
#define DEBUG_IN_SR04									0 //打开SG04调试
#define DEBUG_IN_ADC									0 //打开ADC调试


#define EVENT_FLAG_START				(1 << 0)	//开始启动出料事件位
#define EVENT_FLAG_STOP 				(1 << 1)	//停止出料事件位
#define EVENT_FLAG_RUN 					(1 << 2)	//出料事件位
#define EVENT_FLAG_READY_1 			(1 << 3)	//取料杯1就绪事件位
#define EVENT_FLAG_READY_2 			(1 << 4)	//取料杯2就绪事件位
#define EVENT_FLAG_FINISH_1 		(1 << 5)	//取料杯1完成事件位
#define EVENT_FLAG_FINISH_2 		(1 << 6)	//取料杯2完成事件位
#define EVENT_FLAG_LEVEL_1 			(1 << 7)	//液位80%以上事件位
#define EVENT_FLAG_LEVEL_2 			(1 << 8)	//液位50%以上事件位
#define EVENT_FLAG_LEVEL_3 			(1 << 9)	//液位20%以下事件位
                                          
extern QueueHandle_t xScreenCmdQueue;

/* 屏幕指令类型 */
typedef enum {
	SCREEN_CMD_ANGLE_ADD,    			// 角度增加
	SCREEN_CMD_ANGLE_SUB,    			// 角度减少
	SCREEN_CMD_SAVE_POS1,   		  // 保存位置1
	SCREEN_CMD_SAVE_POS2,    		  // 保存位置2
	SCREEN_CMD_ANGLE_TEST,   			// 角度测试
	SCREEN_CMD_MOTOR_START,  			// 启动电机
	SCREEN_CMD_SPEAK1,   		 			// 语音播报1
	SCREEN_CMD_SPEAK2,   		 			// 语音播报2
	SCREEN_CMD_SPEAK3,   		 			// 语音播报3
	SCREEN_CMD_SPEAK4,   		 			// 语音播报4
	SCREEN_CMD_SPEAK5,   		 			// 语音播报5
	SCREEN_CMD_SPEAK_GET,					// 语音识别
	SCREEN_CMD_JUICE_CHANGE   		// 饮料改变
	
} ScreenCommandType;


/* Sg90数据结构体,使用GCC/ARMCC特性指定对齐*/
typedef struct __attribute__((aligned(4))) {
	int32_t current_angle;   // 当前角度
	int32_t saved_angle1;    // 预设角度1
	int32_t saved_angle2;    // 预设角度2
	int32_t test;            // 测试标志
} Sg90Data;

/* dc_motor数据结构体,使用GCC/ARMCC特性指定对齐*/
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

//DcMotor的状态机
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
