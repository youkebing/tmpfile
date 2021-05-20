#include "Type_def.h"
#include "STC8xxxx.h"

static u8 xstate;
u16 xled;
static u16 xledv;

#define   LED_DLY   200

sbit RELAY1 = P5^5;
void led_init(void) {
	P5M0 = 0; 		//设置准双向口
	P5M1 = 0;
	xled = 0xeeff;//0b1110111011111111;
}

void led_poll(void) {
	u8 t;
	t = TH0 - xstate;
	if (t > LED_DLY) {
		xstate = LED_DLY + xstate;
		xledv = xledv >> 1;
		if (xledv == 0) {
			xledv = xled;
		}
		RELAY1 = xledv & 0x01;
	}
}
