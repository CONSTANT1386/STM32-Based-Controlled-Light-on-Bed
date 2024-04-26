#ifndef __RTC_H
#define __RTC_H

extern uint16_t RTC_Time[];

void RTC_Init(void);
void RTC_SetTime(void);
void RTC_ReadTime(void);
void RTC_SetAlarmTime(void);
void RTC_WriteInit(void);


#endif
