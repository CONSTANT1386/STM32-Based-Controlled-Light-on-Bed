#include "stm32f10x.h"                  // Device header
#include "RTC.h"
#include "Key.h"
#define Sleep_Time 600

extern uint16_t Total_Seconds;
extern uint16_t Alarm_Time[];
extern uint8_t Alarm_Setted;
extern uint8_t Sleep_Flag;


uint16_t num = 0;
uint8_t Alarm_up;
/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void TIM4_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启TIM4的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM4);		//选择TIM4为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM4的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					//开启TIM4的更新中断
	
	
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				//选择配置NVIC的TIM4线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM4, ENABLE);			//使能TIM4，定时器开始运行
}

 //定时器中断函数，可以复制到使用它的地方
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		num++;
		if(num >= Sleep_Time)
		{
			Sleep_Flag = 1;
			num = 0;
		}
		if(Alarm_Setted == 1){
			Total_Seconds--;
			Alarm_Time[0] = Total_Seconds / 3600;
			Alarm_Time[1] = Total_Seconds % 3600 / 60;
			Alarm_Time[2] = Total_Seconds % 60;
			if(Total_Seconds == 0) 
			{
				Alarm_Setted = 0;
				Sleep_Flag = 0;
				Alarm_up = 1;
			}
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

