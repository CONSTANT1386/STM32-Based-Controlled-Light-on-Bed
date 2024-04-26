#include "stm32f10x.h"
#include "Delay.h"
#include "IO.h"
#include "EXTI.h"
#include "RTC.h"
#include "TIM4.h"


uint8_t Key_Mode = 0;
uint8_t Alarm_Set_Flag = 0;
uint8_t Alarm_SetHour_Flag = 0;
uint8_t Alarm_SetMin_Flag = 0;
uint8_t Alarm_Setted = 0;
uint8_t Sleep_Flag = 0;
extern uint16_t Alarm_Time[];
extern uint16_t num;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ���ⲿ�жϣ�����AFIOʱ��

	IO_InInit(GPIOA, GPIO_Pin_5);
	IO_InInit(GPIOB, GPIO_Pin_0);
	IO_InInit(GPIOA, GPIO_Pin_3);  //��Դ����
	IO_InInit(GPIOB, GPIO_Pin_12); //������������ֵ
	IO_InInit(GPIOA, GPIO_Pin_8);  //������������ֵ
	
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
	EXTI_Key_Init(EXTI_Line3,ENABLE);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_Key_Init(EXTI_Line0,ENABLE);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	EXTI_Key_Init(EXTI_Line12,DISABLE);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	EXTI_Key_Init(EXTI_Line8,DISABLE);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	

}

uint8_t Key_GetMode(void)
{
	return Key_Mode;
}


void Key_ResetMode(void)
{
	Key_Mode = 0;
}


void Key_Loop(void)
{
	static uint8_t Key_Now, Key_Last;
	Key_Last = Key_Now;
	Key_Now = 0;
	if(IO_InGet(GPIOA, GPIO_Pin_5)==0)
	{
		Delay_ms(20);
		Key_Now=2;
		Delay_ms(20);
	}
	if(Key_Last==2 && Key_Now==0)
	{
		Key_Mode++;
		if(Key_Mode > 3)
			Key_Mode %= 4;
	}
	return ;
}
	
//PB0�жϺ��������Ը��Ƶ�ʹ�����ĵط�
void EXTI0_IRQHandler(void)
{
	//Delay_ms(10);
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
		{
			num = 0;
		if(Alarm_Set_Flag == 0){
			Alarm_Set_Flag = 1;   //��ζ������������ģʽ
			Alarm_SetHour_Flag = 1;
			EXTI_Key_Init(EXTI_Line12,ENABLE);
			EXTI_Key_Init(EXTI_Line8,ENABLE);      //ʹ���ж����ð���
		}
		else if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 0)
		{
			Alarm_SetMin_Flag = 1;   //���Сʱ����,�����������
			
		}
		else if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 1)
		{
			Alarm_Set_Flag = 0;   
			Alarm_SetHour_Flag = 0;
			Alarm_SetMin_Flag = 0;    //������ɣ���־λ����
			Alarm_Setted = 1;
			RTC_SetAlarmTime();       //�趨����
			EXTI_Key_Init(EXTI_Line12,DISABLE);
			EXTI_Key_Init(EXTI_Line8,DISABLE);
			
		}
		
		EXTI_ClearITPendingBit(EXTI_Line0);
		}
	}
}

void EXTI9_5_IRQHandler(void) //PA8�������¼�������ʱ��
{
	//Delay_ms(10);
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
		{	
	if (EXTI_GetITStatus(EXTI_Line8) == SET)
	{
		num = 0;
		if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 0){
			Alarm_Time[0] --;
		}
		else if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 1){
			Alarm_Time[1] --;
		}
		EXTI_ClearITPendingBit(EXTI_Line8); //�ֶ�����жϱ�־λ
	}
	}
}

void EXTI15_10_IRQHandler(void) //PB12����������������ʱ��
{
	//Delay_ms(10);
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
	if (EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		num = 0;
		if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 0){
			Alarm_Time[0] ++;
		}
		else if(Alarm_Set_Flag == 1 && Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 1){
			Alarm_Time[1] ++;
		}
		EXTI_ClearITPendingBit(EXTI_Line12); //�ֶ�����жϱ�־λ
	}
	}
}

void EXTI3_IRQHandler(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)
	{
		if (EXTI_GetITStatus(EXTI_Line3) == SET){
			num = 0;
			if(Sleep_Flag == 1){
				Sleep_Flag = 0;
			}
			else if(Sleep_Flag == 0){
				Sleep_Flag = 1;
			}
			EXTI_ClearITPendingBit(EXTI_Line3);
		}
	}
}

