#include "stm32f10x.h"                  // Device header

void EXTI_Key_Init(uint32_t EXTI_Line, FunctionalState Able)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line;
	EXTI_InitStructure.EXTI_LineCmd = Able;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	
	
}
