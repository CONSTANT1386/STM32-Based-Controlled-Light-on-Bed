#include "stm32f10x.h"
#include "TIM2.h"
#include "DMA1.h"

//#define WS2812B_LED_QUANTITY	32  //灯珠数量
#define WS2812B_LED_QUANTITY	120  //灯珠数量


uint32_t WS2812B_Buf[WS2812B_LED_QUANTITY];	//0xGGRRBB
uint16_t WS2812B_Bit[24*WS2812B_LED_QUANTITY+1];//灯珠数量*24bit+1
uint8_t WS2812B_Flag;

void WS2812B_IRQHandler(void);

void WS2812B_Init(void)
{
	DMA1_SetIRQHandler(WS2812B_IRQHandler);//把WS2812B_IRQHandler赋给DMA1_Handler
	DMA1_Init((uint32_t)(&WS2812B_Bit));//DMA
	TIM2_Init();//定时器
}

void WS2812B_ClearBuf(void)//全部灯珠亮度归0
{
	uint8_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=0x000000;
	}
}

void WS2812B_SetBuf(uint32_t Color)//颜色
{
	uint8_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=Color;
	}
}

void WS2812B_UpdateBuf(void)
{
	uint8_t i,j;
	for(j=0;j<WS2812B_LED_QUANTITY;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=60;}//0x800000为二进制的1加23个0，>>:右移
			else{WS2812B_Bit[j*24+i+1]=30;}
		}
	}
	DMA1_Start(24*WS2812B_LED_QUANTITY+1);
	TIM2_Cmd(ENABLE);
	while(WS2812B_Flag==0);
	WS2812B_Flag=0;
}

void WS2812B_IRQHandler(void)
{
	TIM2_SetCompare2(0);//pwm设置
	TIM2_Cmd(DISABLE);//关闭定时器
	WS2812B_Flag=1;
}
