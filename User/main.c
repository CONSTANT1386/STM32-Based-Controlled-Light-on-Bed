#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "IO.h"
#include "TIM3.h"
#include "OLED.h"
#include "WS2812B.h"
#include "Key.h"
#include "RCC.h"
#include "stdlib.h"
#include "RTC.h"
#include "TIM4.h"
#include "NVIC.h"
#include "EXTI.h"

//初始化变量
uint8_t mode = 0;
uint8_t UpdateFlag=1;
uint8_t ColorLight_Mode,ColorLight_Flag;
uint16_t ColorLight_Time;
#define WS2812B_LED_QUANTITY	121  //灯珠数量
char* Mode[5] = {"Closed ", "Reading", "RGB1   ", "RGB2   ", "Time Up!"};
extern uint8_t Sleep_Flag;
extern uint32_t WS2812B_Buf[];
extern uint16_t num;
extern uint16_t Alarm_Time[];
extern uint8_t Alarm_Set_Flag;
extern uint8_t Alarm_SetHour_Flag;
extern uint8_t Alarm_SetMin_Flag;
extern uint16_t Total_Seconds;
extern uint8_t Alarm_Setted;
extern uint8_t Alarm_up;


//函数声明
void Close(void);
void WhiteLight(void);
void ColorLight(void);
void ColorLight_Mode0(void);
void ColorLight_Mode1(void);
void ColorLight_Mode2(void);
void ColorLight_Mode3(void);
void MainLoop(void)             //每20ms检查一次按键状态                                 
{                                     //控制灯带颜色模式按设定的周期(ColorLight_Time)自动切换
	static uint16_t LoopCount[2];
	LoopCount[0]++;
	if(LoopCount[0]>=20)
	{
		LoopCount[0]=0;
		Key_Loop();
	}
	LoopCount[1]++;
	if(LoopCount[1]>=ColorLight_Time)
	{
		LoopCount[1]=0;
		ColorLight_Flag=1;
	}
}void Init(void)
{
	NVIC_Priority_Init();
	RCC_Init();
	OLED_Init();
	Key_Init();
	WS2812B_Init();
	TIM3_SetIRQHandler(MainLoop);
	TIM3_Init();
	RTC_WriteInit();
	RTC_Init();
	TIM4_Init();
	PWR_WakeUpPinCmd(ENABLE);
}




int main(void)
{
	Init();


	while (1)
	{
		uint8_t Alarm_Current_Flag = 0;  //用来判断是否是退出设置模式的最后一次循环
		uint8_t temp = mode;
		mode = Key_GetMode();
		if(temp != mode) num = 0;
		//OLED_ShowString(1,9, Mode[mode]);
		//OLED_ShowNum(1,9, (uint16_t)Sleep_Flag, 2);
		OLED_ShowString(1, 3, "Mode:");
		OLED_ShowString(2, 7, "-");
		OLED_ShowString(2, 10, "-");
		OLED_ShowString(3, 3, "Time:");
		OLED_ShowString(3, 10, ":");
		OLED_ShowString(3, 13, ":");
		OLED_ShowString(4, 3, "Alarm:");
		OLED_ShowString(4, 11, ":");
		OLED_ShowString(4, 14, ":");
		RTC_ReadTime();
		OLED_ShowNum(2, 3, RTC_Time[0], 4);		//显示MyRTC_Time数组中的时间值，年
		OLED_ShowNum(2, 8, RTC_Time[1], 2);		//月
		OLED_ShowNum(2, 11, RTC_Time[2], 2);	//日
		OLED_ShowNum(3, 8, RTC_Time[3], 2);		    //时
		OLED_ShowNum(3, 11, RTC_Time[4], 2);		//分
		OLED_ShowNum(3, 14, RTC_Time[5], 2);		//秒
		OLED_ShowNum(4, 9, Alarm_Time[0], 2);		//闹钟：时
		OLED_ShowNum(4, 12, Alarm_Time[1], 2);      //闹钟：分
		OLED_ShowNum(4, 15, Alarm_Time[2], 2);      //闹钟：秒  
		if(Alarm_Setted == 0 && Total_Seconds == 0)
			Total_Seconds = Alarm_Time[0] * 3600 + Alarm_Time[1] * 60 + Alarm_Time[2] ;
		if(Alarm_up == 1){
			mode = 4;
			num = 0;
		}

		
		switch(mode)
		{
			case 0:
			{				
				Close();
				OLED_ShowString(1,9, Mode[mode]);
				//OLED_ShowString(2,9, "Closed");
				break;
			}
			case 1: 
			{
				WhiteLight();
				OLED_ShowString(1,9, Mode[mode]);
				//OLED_ShowString(2,9, "Reading");
				break;
			}
			case 2:
			{
				ColorLight_Mode = 1;
				ColorLight();
				OLED_ShowString(1,9, Mode[mode]);
				//OLED_ShowString(2,9, "RGB1");
				break;
			}
			case 3:
			{
				ColorLight_Mode = 2 ;
				ColorLight();
				OLED_ShowString(1,9, Mode[mode]);
				//OLED_ShowString(2,9, "RGB2");
				break;
			}
			case 4:
				ColorLight_Mode = 0 ;
				ColorLight();
				OLED_ShowString(1,9, Mode[mode]);
				//OLED_ShowString(2,9, "RGB3");
				break;
		}
		//判断是否开启闹钟模式--无效的，待机唤醒一切清零
		/*if(Alarm_Setted == 1)
		{
			OLED_ShowString(4, 8, " ");
			Delay_ms(500);
			OLED_ShowString(4, 8, ":");       //本来想做闹钟那种显示跳变，但是加上后500ms对主函数其他部分的执行影响太大
			Delay_ms(500);
			
		}*/
		

		
		if(Sleep_Flag == 1)
		{
			Close();
			OLED_Clear();
			EXTI_Key_Init(EXTI_Line12,DISABLE);
			EXTI_Key_Init(EXTI_Line8,DISABLE);
			EXTI_Key_Init(EXTI_Line0,DISABLE);
			while(Sleep_Flag == 1);//关机模式，等待唤醒
			num = 0;
			EXTI_Key_Init(EXTI_Line0,ENABLE);
			Key_ResetMode();
		}
		
		//闹钟设置模式
		while(Alarm_Set_Flag == 1)
		{
			Alarm_Current_Flag = Alarm_Set_Flag;
			if(Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 0)
			{
				OLED_ShowString(4, 9, "  ");
				Delay_ms(500);
				OLED_ShowNum(4, 9, Alarm_Time[0], 2);
				Delay_ms(500);
			}
			if(Alarm_SetHour_Flag == 1 && Alarm_SetMin_Flag == 1)
			{
				OLED_ShowString(4, 12, "  ");
				Delay_ms(500);
				OLED_ShowNum(4, 12, Alarm_Time[1], 2);
				Delay_ms(500);
			}
			if(Alarm_Current_Flag == 1 && Alarm_Set_Flag == 0)
			{
				OLED_ShowString(4, 9, "SET!         ");
				Delay_ms(750);
				OLED_ShowString(4, 3, "Alarm:XX:XX:XX");
			}
			
		}
		
	}
}

void Close(void)
{
	UpdateFlag=1;
	if(UpdateFlag)
	{
		UpdateFlag=0;
		WS2812B_SetBuf(0x000000);
		WS2812B_UpdateBuf();
	}
}

void WhiteLight(void)	//MODE=2
{
	UpdateFlag=1;
	if(UpdateFlag)
	{
		UpdateFlag=0;
		WS2812B_SetBuf(0x2F2F2F);//中档
		WS2812B_UpdateBuf();
	}
}
void ColorLight_Mode0(void)
{
	static uint8_t i,Color;
	ColorLight_Time=6;

	if(i==0)WS2812B_SetBuf((Color));
	if(i==1)WS2812B_SetBuf((255-Color));
	if(i==2)WS2812B_SetBuf((Color)<<8);
	if(i==3)WS2812B_SetBuf((255-Color)<<8);
	if(i==4)WS2812B_SetBuf((Color)<<16);
	if(i==5)WS2812B_SetBuf((255-Color)<<16);
	if(i==6)WS2812B_SetBuf((Color)|((Color)<<8));
	if(i==7)WS2812B_SetBuf((255-Color)|((255-Color)<<8));
	if(i==8)WS2812B_SetBuf((Color)|((Color)<<16));
	if(i==9)WS2812B_SetBuf((255-Color)|((255-Color)<<16));
	if(i==10)WS2812B_SetBuf(((Color)<<8)|((Color)<<16));
	if(i==11)WS2812B_SetBuf(((255-Color)<<8)|((255-Color)<<16));
	if(i==12)WS2812B_SetBuf(((Color))|((Color)<<8)|((Color)<<16));
	if(i==13)WS2812B_SetBuf(((255-Color))|((255-Color)<<8)|((255-Color)<<16));

	Color++;
	if(Color==0)
	{
		i++;
		i%=14;
	}
}

void ColorLight_Mode1(void)
{
	uint8_t i,RandNum;
	uint32_t G,R,B;
	static uint8_t j;
	ColorLight_Time=20;
	for(i=WS2812B_LED_QUANTITY;i>0;i--)
	{
		WS2812B_Buf[i]=WS2812B_Buf[i-1];
	}
	
	if(j==0)
	{
		RandNum=rand()%7;
		if(RandNum==0)WS2812B_Buf[0]=0x0000FF;
		if(RandNum==1)WS2812B_Buf[0]=0x00FF00;
		if(RandNum==2)WS2812B_Buf[0]=0xFF0000;
		if(RandNum==3)WS2812B_Buf[0]=0x00FFFF;
		if(RandNum==4)WS2812B_Buf[0]=0xFF00FF;
		if(RandNum==5)WS2812B_Buf[0]=0xFFFF00;
		if(RandNum==6)WS2812B_Buf[0]=0xFFFFFF;
	}
	else if(j<15)
	{
		G=WS2812B_Buf[1]/0x10000%0x100;
		R=WS2812B_Buf[1]/0x100%0x100;
		B=WS2812B_Buf[1]%0x100;
		if(G>20)G-=20;
		if(R>20)R-=20;
		if(B>20)B-=20;
		WS2812B_Buf[0]=(G<<16)|(R<<8)|B;
	}
	else
	{
		WS2812B_Buf[0]=0;
	}
	
	j++;
	j%=50;
}

void ColorLight_Mode2(void)
{
	uint8_t i;
	static uint8_t j;
	ColorLight_Time=20;
	for(i=WS2812B_LED_QUANTITY;i>0;i--)
	{
		WS2812B_Buf[i]=WS2812B_Buf[i-1];
	}
	if(j==0)WS2812B_Buf[0]=rand()%0x1000000;
	else WS2812B_Buf[0]=WS2812B_Buf[1];
	j++;
	j%=10;
}

void ColorLight_Mode3(void)
{
	uint8_t i;
	ColorLight_Time=500;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=rand()%0x1000000;
	}
}

void ColorLight(void)	//MODE=3
{
	if(UpdateFlag)
	{
		UpdateFlag=0;
		WS2812B_SetBuf(0x000000);
		WS2812B_UpdateBuf();
	}
	if(ColorLight_Flag)
	{
		ColorLight_Flag=0;
		if(ColorLight_Mode==0)
		{
			ColorLight_Mode0();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==1)
		{
			ColorLight_Mode1();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==2)
		{
			ColorLight_Mode2();
			WS2812B_UpdateBuf();
		}
		if(ColorLight_Mode==3)
		{
			ColorLight_Mode3();
			WS2812B_UpdateBuf();
		}
	}
}
