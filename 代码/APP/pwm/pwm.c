#include "pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ec11.h"
#include "screen.h"

void TIM4_CH4_PWM_Init(u16 per,u16 psc)
{
	//定义结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	//挂接时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能GPIO外设和AFIO复用功能模块时钟使能
	//配置引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//定时器基础配置
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_InitStructure.TIM_Period = per;                                         //自动重装值
	TIM_InitStructure.TIM_Prescaler = psc;                                        //时钟预分频数
	TIM_TimeBaseInit(TIM4,&TIM_InitStructure);
	
	
	//PWM配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比 0% (0/1000)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable); //使能TIMx在CCR2上的预装载寄存器
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
}

void sg90_task(void *pvParameters)
{
	int last_data =0;
	while(1)
	{
		if(last_data != data) 
		{
			TIM_SetCompare4(TIM4,data*10);//2.5ms	// 更新 CCR1 寄存器
			last_data = data;
		}
		vTaskDelay(pdMS_TO_TICKS(10)); // 延时 10ms
	}
}
