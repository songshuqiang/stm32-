#include "led.h"

/*******************************************************************************
* �� �� ��         : LED_Init
* ��������		   : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	RCC_APB2PeriphClockCmd(LED13_PORT_RCC | LED3_PORT_RCC | LED4_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  	= LED13_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed =	GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED13_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(LED13_PORT,LED13_PIN);   //��LED�˿����ߣ�Ϩ������LED
	
	GPIO_InitStructure.GPIO_Pin  	= LED3_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed =	GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED3_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(LED3_PORT,LED3_PIN);   //��LED�˿����ߣ�Ϩ������LED
	
	GPIO_InitStructure.GPIO_Pin  	= LED4_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed =	GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED4_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(LED4_PORT,LED4_PIN);   //��LED�˿����ߣ�Ϩ������LED
}




