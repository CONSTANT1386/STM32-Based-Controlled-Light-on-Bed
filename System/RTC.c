#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "Key.h"
extern uint8_t Alarm_Setted;

uint16_t RTC_Time[] = {2023, 11, 22, 16, 40, 0}; //����ȫ�ֵ�ʱ�����飬�������ݷֱ�Ϊ�ꡢ�¡��ա�ʱ���֡���
uint16_t Alarm_Time[] = {4,30,0};
uint16_t Total_Seconds;
uint32_t Alarm_Start_Time;


void RTC_SetTime(void);				//��������
static void RTC_NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

/**
  * ��    ����RTC��ʼ��
  * ��    ������
  * �� �� ֵ����
  */


void RTC_Init(void)
{
	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//����PWR��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);		//����BKP��ʱ��
	
	/*���ݼĴ�������ʹ��*/
	PWR_BackupAccessCmd(ENABLE);							//ʹ��PWR�����Ա��ݼĴ����ķ���
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)			//ͨ��д�뱸�ݼĴ����ı�־λ���ж�RTC�Ƿ��ǵ�һ������
															//if������ִ�е�һ�ε�RTC����
	{
		RCC_LSEConfig(RCC_LSE_ON);							//����LSEʱ��
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);	//�ȴ�LSE׼������
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//ѡ��RTCCLK��ԴΪLSE
		RCC_RTCCLKCmd(ENABLE);								//RTCCLKʹ��
		
		RTC_WaitForSynchro();								//�ȴ�ͬ��
		RTC_WaitForLastTask();								//�ȴ���һ�β������
		//RTC_ITConfig(RTC_IT_ALR, ENABLE);        //ʹ�����ӶϺ������ж�
		RTC_WaitForLastTask(); //�ȴ���RTC�Ĵ�������д�������

		RTC_EnterConfigMode();
		RTC_SetPrescaler(32768 - 1);						//����RTCԤ��Ƶ����Ԥ��Ƶ��ļ���Ƶ��Ϊ1Hz
		RTC_WaitForLastTask();								//�ȴ���һ�β������
		RTC_SetTime();									//����ʱ�䣬���ô˺�����ȫ��������ʱ��ֵˢ�µ�RTCӲ����·
		RTC_ExitConfigMode();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);			//�ڱ��ݼĴ���д���Լ��涨�ı�־λ�������ж�RTC�ǲ��ǵ�һ��ִ������
	}
	else													//RTC���ǵ�һ������
	{
		RTC_WaitForSynchro();								//�ȴ�ͬ��
		//RTC_ITConfig(RTC_IT_ALR, ENABLE);        //ʹ�����ӶϺ������ж�
		RTC_WaitForLastTask();								//�ȴ���һ�β������
	}
	//RTC_NVIC_Config();
}


/**
  * ��    ����RTC����ʱ��
  * ��    ������
  * �� �� ֵ����
  * ˵    �������ô˺�����ȫ��������ʱ��ֵ��ˢ�µ�RTCӲ����·
  */
void RTC_SetTime(void)
{
	time_t time_cnt;		//�������������������
	struct tm time_date;	//��������ʱ����������
	
	time_date.tm_year = RTC_Time[0] - 1900;		//�������ʱ�丳ֵ������ʱ��ṹ��
	time_date.tm_mon = RTC_Time[1] - 1;
	time_date.tm_mday = RTC_Time[2];
	time_date.tm_hour = RTC_Time[3];
	time_date.tm_min = RTC_Time[4];
	time_date.tm_sec = RTC_Time[5];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//����mktime������������ʱ��ת��Ϊ���������ʽ
													//- 8 * 60 * 60Ϊ��������ʱ������
	
	RTC_SetCounter(time_cnt);						//���������д�뵽RTC��CNT��
	RTC_WaitForLastTask();							//�ȴ���һ�β������
}

/**
  * ��    ����RTC��ȡʱ��
  * ��    ������
  * �� �� ֵ����
  * ˵    �������ô˺�����RTCӲ����·��ʱ��ֵ��ˢ�µ�ȫ������
  */
void RTC_ReadTime(void)
{
	time_t time_cnt;		//�������������������
	struct tm time_date;	//��������ʱ����������
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;		//��ȡRTC��CNT����ȡ��ǰ���������
													//+ 8 * 60 * 60Ϊ��������ʱ������
	
	time_date = *localtime(&time_cnt);				//ʹ��localtime���������������ת��Ϊ����ʱ���ʽ
	
	RTC_Time[0] = time_date.tm_year + 1900;		//������ʱ��ṹ�帳ֵ�������ʱ��
	RTC_Time[1] = time_date.tm_mon + 1;
	RTC_Time[2] = time_date.tm_mday;
	RTC_Time[3] = time_date.tm_hour;
	RTC_Time[4] = time_date.tm_min;
	RTC_Time[5] = time_date.tm_sec;
}

void RTC_SetAlarmTime(void)
{
	Total_Seconds = Alarm_Time[0] * 3600 + Alarm_Time[1] * 60 + Alarm_Time[2] ;
	//Total_Seconds = 12;       ������
	uint32_t Alarm = RTC_GetCounter() + Total_Seconds;//����Ϊ���Ѻ�ǰʱ��ĺ�xʱx��
	//BKP_WriteBackupRegister(BKP_DR2, Total_Seconds);
	Alarm_Start_Time = RTC_GetCounter();
	//BKP_WriteBackupRegister(BKP_DR3, Alarm_Start_Time>>16);
	//RTC_SetAlarm(RTC_GetCounter() + 10);
	//RTC_SetAlarm(Alarm);							//д������ֵ��RTC��ALR�Ĵ���
	RTC_WaitForLastTask(); 
}

void RTC_WriteInit(void)
{
	BKP_WriteBackupRegister(BKP_DR1, 0x0000);  //���DR1�Ĵ�������ֹ��λ��ʱ��д����ȥ
	
}

/*void RTC_IRQHandler(void)
{
	if(Alarm_Setted == 1){
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET) //�����ж�
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



