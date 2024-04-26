#ifndef __TIM2_H
#define __TIM2_H
#include "stm32f10x.h"

void TIM2_Init(void);
void TIM2_Cmd(FunctionalState NewState);
void TIM2_SetCompare2(uint16_t Value);

#endif
