#include "Type_def.h"
#include "STC8xxxx.h"
#include "STC8G_H_UART.h"
#include "STC8G_H_NVIC.h"
#include "STC8G_H_Switch.h"

u16 uart1_sch(u8 *sessid, u8 *buf);
void uart1_complted(u8 sessid);
void uart1_pip(u8 sessid, u8 c);

u16 uart3_sch(u8 *sessid, u8 *buf);
void uart3_complted(u8 sessid);
void uart3_pip(u8 sessid, u8 c);

#define  MB_ADDR    0x31

#define UART1_BAUDRATE    9600

#if UART1_BAUDRATE == 1200
	#define  UART1_RX_DLY      27
	#define  UART1_TX_DLY      40
#elif UART1_BAUDRATE == 2400
	#define  UART1_RX_DLY      14
	#define  UART1_TX_DLY      30
#elif UART1_BAUDRATE == 4800
	#define  UART1_RX_DLY      7
	#define  UART1_TX_DLY      30
#elif UART1_BAUDRATE == 9600
	#define  UART1_RX_PROCESS  20
	#define  UART1_RX_DLY      20
	#define  UART1_TX_DLY      20
#else
	fprintf("UART1_BAUDRATE err!!!");
#endif

#define   UART_STATUS_RX     0
#define   UART_STATUS_TX     1
#define   FRAME_WAIT_REQ     2
//需要初始化
#define   UART_STATUS_INIT   255

#define  BUF1_LENGTH      256

static data volatile u16 xindex;
static data volatile u16 xlength;
static volatile u16 xtime;
static xdata u8 xbuf[BUF1_LENGTH];
static data volatile u8 state;

static u8 xsessid;
static u8 xuartindex = 0;

//#define uart_sch(a, b)     uart1_sch(a, b)
//#define uart_complted(a)   uart1_complted(a)
//#define uart_pip(a, b)        uart1_pip(a, b)

#define uart_pip(a, b)	if (xuartindex == 0) {  \
		uart1_pip(a, b);                        \
	} else {                                    \
		uart3_pip(a, b);                        \
	}

//inline void uart_pip(u8 sessid, u8 c) {
//	if (xuartindex == 0) {
//		uart1_pip(sessid, c);
//	} else {
//		uart3_pip(sessid, c);
//	}
//}
static u16 uart_sch(u8 *sessid, u8 *buf) {
	if (xuartindex) {
		xuartindex = 0;
		return uart1_sch(sessid, buf);
	} else {
		xuartindex = 1;
		return uart3_sch(sessid, buf);
	}
}
void uart_complted(u8 sessid) {
	if (xuartindex == 0) {
		uart1_complted(sessid);
	} else {
		uart3_complted(sessid);
	}
}
//void uart_pip(u8 sessid, u8 c) {
//	//uart1_pip(sessid, c);
//	if (xuartindex == 0) {
//		uart1_pip(sessid, c);
//	} else {
//		uart1_pip(sessid, c);
//	}
//}

static void uart2_isr(void) interrupt UART2_VECTOR {
	EA = 0;
	if (RI2) {
		CLR_RI2();
		if (state == UART_STATUS_RX) {
			timer_get(xtime);
			//xindex++;
			xbuf[xlength++] = S2BUF;
			//uart_pip(xsessid, S2BUF);
		}
	}
	if (TI2) {
		CLR_TI2();
		if (state == UART_STATUS_TX) {
			timer_get(xtime);
			if (xindex < xlength) {
				S2BUF = xbuf[xindex++];
			} else {
				xindex = 0;
				xlength = 0;
				state = UART_STATUS_RX;
			}
		}
	}
	EA = 1;
}

void uart2_init(void) {
	P1M0 = 0; 		//设置准双向口
	P1M1 = 0;

	UART2_Interrupt(DISABLE);
	S2CON &= ~(1<<7);	//8bit
	UART2_RxEnable(1);//COMx->UART_RxEnable);	//UART接收使能
	//return SUCCESS;
	UART2_SW(UART2_SW_P10_P11);

	S2CON |=  (1<<4);	//允许接收
	//else								S2CON &= ~(1<<4);	//禁止接收
	P_SW2 = (P_SW2 & ~1) | (UART2_SW_P10_P11 & 0x01);	//切换IO
	UART2_Interrupt(ENABLE);
	//_uart2TxBusy = 0;
	//SET_TI2();
	state = FRAME_WAIT_REQ;
	UART2_Polity(Polity_2);
	EA = 1;
}

//主程序轮询
void uart2_poll(void) {
	EA = 0;
	if (state == FRAME_WAIT_REQ) {    //等待有消息发送
		xlength = uart_sch(&xsessid, xbuf);
		if (xlength) {
			xindex = 0;
			state = UART_STATUS_TX;
			timer_get(xtime);
			SET_TI2();
		}
	} else if (state == UART_STATUS_RX) {
		if (xlength > 4) {
			while (xindex < xlength) {
				uart_pip(xsessid, xbuf[xindex++]);
			}
		}
		if ((xlength == 0) && (timer_expired(xtime, 2000))) {
			uart_complted(xsessid);
			state = FRAME_WAIT_REQ;
		} else if ((xlength) && (timer_expired(xtime, UART1_RX_PROCESS))) {
			while (xindex < xlength) {
				uart_pip(xsessid, xbuf[xindex++]);
			}
			uart_complted(xsessid);
			state = FRAME_WAIT_REQ;
		}
	} else if (state == UART_STATUS_TX) {
		//太久没有发送中断，那么提前结束，复位uart,正常不会出现
		if (timer_expired(xtime, UART1_TX_DLY)) {
			uart_complted(xsessid);
			state = UART_STATUS_INIT;
		}
	} else {
		uart2_init();
		state = FRAME_WAIT_REQ;
	}
	EA = 1;
}
