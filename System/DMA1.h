#ifndef __DMA1_H
#define __DMA1_H
#include "stm32f10x.h"

void DMA1_Init(uint32_t MemoryBaseAddr);
void DMA1_SetIRQHandler(void (*IRQHandler)(void));
void DMA1_Start(uint16_t DataNumber);

#endif
