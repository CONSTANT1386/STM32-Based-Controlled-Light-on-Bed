#include "stm32f10x.h"

/**
  * @brief  IO口初始化为输出模式
  * @param  GPIOx,Pin 目标IO口
  * @retval 无
  */
void IO_OutInit(GPIO_TypeDef* GPIOx,uint16_t Pin)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin= Pin;
    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/**
  * @brief  IO口输出状态
  * @param  GPIOx,Pin 目标IO口
  * @param  PinState IO口状态，0为低电平，非0为高电平
  * @retval 无
  */
void IO_OutPut(GPIO_TypeDef* GPIOx,uint16_t Pin,uint8_t PinState)
{
	if(PinState)
	{
		GPIO_SetBits(GPIOx,Pin);
	}
	else
	{
		GPIO_ResetBits(GPIOx,Pin);
	}
}

/**
  * @brief  IO口状态翻转
  * @param  GPIOx,Pin 目标IO口
  * @retval 无
  */
void IO_OutTurn(GPIO_TypeDef* GPIOx,uint16_t Pin)
{
	if(GPIO_ReadOutputDataBit(GPIOx,Pin))
	{
		GPIO_ResetBits(GPIOx,Pin);
	}
	else
	{
		GPIO_SetBits(GPIOx,Pin);
	}
}

/**
  * @brief  IO口初始化为输入模式
  * @param  GPIOx,Pin 目标IO口
  * @retval 无
  */
void IO_InInit(GPIO_TypeDef* GPIOx,uint16_t Pin)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin= Pin;
    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/**
  * @brief  IO口状态获取
  * @param  GPIOx,Pin 目标IO口
  * @retval IO口的状态，低电平为0，高电平为1
  */
uint8_t IO_InGet(GPIO_TypeDef* GPIOx,uint16_t Pin)
{
	return GPIO_ReadInputDataBit(GPIOx,Pin);
}

