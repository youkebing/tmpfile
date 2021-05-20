#include "Type_def.h"
#include "STC8xxxx.h"
#include "STC8G_H_UART.h"
#include "STC8G_H_NVIC.h"
#include "STC8G_H_Switch.h"

u16 g_xcnt;

u8 timer_expired(u16 d, u16 dly) {
	u16 ii;
	timer_update();
	ii = g_xcnt - d;
	return ii >= dly;
}

void timer_init(void) {
	Timer0_Stop();		//停止计数
	//if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return FAIL;	//错误
	TMOD = (TMOD & ~0x03) | TIM_16Bit;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 不可屏蔽16位自动重装
	//if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return FAIL;
	Timer0_CLK_Select(TIM_CLOCK_12T);//TIMx->TIM_ClkSource);	//对外计数或分频, 定时12T/1T
	Timer0_CLK_Output(DISABLE);//TIMx->TIM_ClkOut);		//输出时钟使能
	//T0_Load(TIMx->TIM_Value);
	Timer0_Run(1);//TIMx->TIM_Run);
	//return	SUCCESS;		//成功
}