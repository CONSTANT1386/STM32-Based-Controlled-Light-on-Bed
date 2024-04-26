#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "Key.h"
extern uint8_t Alarm_Setted;

uint16_t RTC_Time[] = {2023, 11, 22, 16, 40, 0}; //定义全局的时间数组，数组内容分别为年、月、日、时、分、秒
uint16_t Alarm_Time[] = {4,30,0};
uint16_t Total_Seconds;
uint32_t Alarm_Start_Time;


void RTC_SetTime(void);				//函数声明
static void RTC_NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/**
  * 函    数：RTC初始化
  * 参    数：无
  * 返 回 值：无
  */


void RTC_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);		//开启BKP的时钟
	
	/*备份寄存器访问使能*/
	PWR_BackupAccessCmd(ENABLE);							//使用PWR开启对备份寄存器的访问
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)			//通过写入备份寄存器的标志位，判断RTC是否是第一次配置
															//if成立则执行第一次的RTC配置
	{
		RCC_LSEConfig(RCC_LSE_ON);							//开启LSE时钟
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);	//等待LSE准备就绪
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//选择RTCCLK来源为LSE
		RCC_RTCCLKCmd(ENABLE);								//RTCCLK使能
		
		RTC_WaitForSynchro();								//等待同步
		RTC_WaitForLastTask();								//等待上一次操作完成
		//RTC_ITConfig(RTC_IT_ALR, ENABLE);        //使能秒钟断和闹钟中断
		RTC_WaitForLastTask(); //等待对RTC寄存器最后的写操作完成

		RTC_EnterConfigMode();
		RTC_SetPrescaler(32768 - 1);						//设置RTC预分频器，预分频后的计数频率为1Hz
		RTC_WaitForLastTask();								//等待上一次操作完成
		RTC_SetTime();									//设置时间，调用此函数，全局数组里时间值刷新到RTC硬件电路
		RTC_ExitConfigMode();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);			//在备份寄存器写入自己规定的标志位，用于判断RTC是不是第一次执行配置
	}
	else													//RTC不是第一次配置
	{
		RTC_WaitForSynchro();								//等待同步
		//RTC_ITConfig(RTC_IT_ALR, ENABLE);        //使能秒钟断和闹钟中断
		RTC_WaitForLastTask();								//等待上一次操作完成
	}
	//RTC_NVIC_Config();
}


/**
  * 函    数：RTC设置时间
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，全局数组里时间值将刷新到RTC硬件电路
  */
void RTC_SetTime(void)
{
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_date.tm_year = RTC_Time[0] - 1900;		//将数组的时间赋值给日期时间结构体
	time_date.tm_mon = RTC_Time[1] - 1;
	time_date.tm_mday = RTC_Time[2];
	time_date.tm_hour = RTC_Time[3];
	time_date.tm_min = RTC_Time[4];
	time_date.tm_sec = RTC_Time[5];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//调用mktime函数，将日期时间转换为秒计数器格式
													//- 8 * 60 * 60为东八区的时区调整
	
	RTC_SetCounter(time_cnt);						//将秒计数器写入到RTC的CNT中
	RTC_WaitForLastTask();							//等待上一次操作完成
}

/**
  * 函    数：RTC读取时间
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，RTC硬件电路里时间值将刷新到全局数组
  */
void RTC_ReadTime(void)
{
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;		//读取RTC的CNT，获取当前的秒计数器
													//+ 8 * 60 * 60为东八区的时区调整
	
	time_date = *localtime(&time_cnt);				//使用localtime函数，将秒计数器转换为日期时间格式
	
	RTC_Time[0] = time_date.tm_year + 1900;		//将日期时间结构体赋值给数组的时间
	RTC_Time[1] = time_date.tm_mon + 1;
	RTC_Time[2] = time_date.tm_mday;
	RTC_Time[3] = time_date.tm_hour;
	RTC_Time[4] = time_date.tm_min;
	RTC_Time[5] = time_date.tm_sec;
}

void RTC_SetAlarmTime(void)
{
	Total_Seconds = Alarm_Time[0] * 3600 + Alarm_Time[1] * 60 + Alarm_Time[2] ;
	//Total_Seconds = 12;       调试用
	uint32_t Alarm = RTC_GetCounter() + Total_Seconds;//闹钟为唤醒后当前时间的后x时x秒
	//BKP_WriteBackupRegister(BKP_DR2, Total_Seconds);
	Alarm_Start_Time = RTC_GetCounter();
	//BKP_WriteBackupRegister(BKP_DR3, Alarm_Start_Time>>16);
	//RTC_SetAlarm(RTC_GetCounter() + 10);
	//RTC_SetAlarm(Alarm);							//写入闹钟值到RTC的ALR寄存器
	RTC_WaitForLastTask(); 
}

void RTC_WriteInit(void)
{
	BKP_WriteBackupRegister(BKP_DR1, 0x0000);  //清除DR1寄存器，防止复位后时间写不进去
	
}

/*void RTC_IRQHandler(void)
{
	if(Alarm_Setted == 1){
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET) //秒钟中断
		{
			Total_Seconds--;
			Alarm_Time[0] = Total_Seconds / 3600;
			Alarm_Time[1] = Total_Seconds % 3600 / 60;
			Alarm_Time[2] = Total_Seconds % 60;
			RTC_ClearITPendingBit(RTC_IT_SEC);
		}
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);
}*/



