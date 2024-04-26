#ifndef __IO_H
#define __IO_H
#include "stm32f10x.h"

void IO_OutInit(GPIO_TypeDef* GPIOx,uint16_t Pin);
void IO_OutPut(GPIO_TypeDef* GPIOx,uint16_t Pin,uint8_t PinState);
void IO_OutTurn(GPIO_TypeDef* GPIOx,uint16_t Pin);

void IO_InInit(GPIO_TypeDef* GPIOx,uint16_t Pin);
uint8_t IO_InGet(GPIO_TypeDef* GPIOx,uint16_t Pin);

#endif
