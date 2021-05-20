#include "Type_def.h"
#include "STC8xxxx.h"
#include "STC8G_H_UART.h"
#include "STC8G_H_NVIC.h"
#include "STC8G_H_Switch.h"

u16 MbCrc16(u8 xdata * pucFrame, u16 usLen);

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

#define   UART_STATUS_RX              0
#define   UART_STATUS_TX              1
#define   UART_STATUS_WAIT            2
#define   UART_STATUS_WAIT_C          3
//需要初始化
#define   UART_STATUS_INIT            (UART_STATUS_WAIT_C + 1)

#define  BUF1_LENGTH      256

static data volatile u16 xindex;
static data volatile u16 xlength;

static xdata u8 xbuf[BUF1_LENGTH];
static data volatile u8 state;
static u8 xsessid;
static volatile u8 xti;

static volatile u16 xtime;

static void uart3_isr(void) interrupt UART3_VECTOR {
	EA = 0;
	if (RI3) {
		CLR_RI3();
		xsessid++;
		timer_get(xtime);
		if (state == UART_STATUS_RX) {
			if (xindex < BUF1_LENGTH) {
				xbuf[xindex++] = S3BUF;
			} else {
				xindex = BUF1_LENGTH + 1;
			}
		} else {
			state = UART_STATUS_RX;
			xindex = 1;
			xbuf[0] = S3BUF;
		}
	}
	if (TI3) {
		CLR_TI3();
		xti = 0;
		if ((state == UART_STATUS_TX) || (state == UART_STATUS_WAIT_C)) {
			if (xindex != xlength) {
				if ((xindex > xlength) || (xlength >= BUF1_LENGTH)) {
					xindex = 0;
					state = UART_STATUS_RX;
				} else {
					xti = 1;
					S3BUF = xbuf[xindex++];
				}
			}
		}
	}
	EA = 1;
}

void uart3_init(void) {
	P0M0 = 0; 		//设置准双向口
	P0M1 = 0;
	S3_BRT_UseTimer2();
	UART3_Interrupt(DISABLE);
	UART3_Polity(Polity_2);
	UART3_SW(UART3_SW_P00_P01);
	UART3_RxEnable(1);//COMx->UART_RxEnable);	//UART接收使能
	UART3_Interrupt(ENABLE);
	state = UART_STATUS_INIT;
}

static void uart3_parse(void) {
	u8 len;
	u16 crc;

	len = xindex;
	if (xbuf[0] != 0x31) {  //地址
		return;
	}
	if (len > BUF1_LENGTH) {
		return;
	}
	if (len < 4) {
		return;
	}
	crc = MbCrc16(xbuf, len - 2);
	if (((u8)crc != xbuf[len - 2]) || ((u8)(crc>>8) != xbuf[len - 1])) {
		return;
	}
	xlength = len;
}

u16 uart3_sch(u8 *sessid, u8 *buf) {
	u16 i;

	if (state == UART_STATUS_WAIT_C) {
		xindex = 0;
		state = UART_STATUS_RX;
	} else if (state == UART_STATUS_WAIT) {
		if (xlength == 0) {
			xindex = 0;
			state = UART_STATUS_RX;
		} else {
			for (i = 0; i < xlength; i++) {
				buf[i] = xbuf[i];
			}
			i = xlength;
			xlength = 0;
			xindex = 0;
			state = UART_STATUS_WAIT_C;
			*sessid = xsessid;
			return i;
		}
	}
	return 0;
}
void uart3_pip(u8 sessid, u8 c) {
	if ((sessid == xsessid) && (state == UART_STATUS_WAIT_C)) {
		if (xlength < BUF1_LENGTH) {
			xbuf[xlength++] = c;
			if (xti == 0) {
				SET_TI3();
				xti = 1;
			}
		}
	}
}

void uart3_complted(u8 sessid) {
	if ((sessid == xsessid) && (state == UART_STATUS_WAIT_C)) {
		xsessid++;
		state = UART_STATUS_TX;
		if (xti == 0) {
			SET_TI3();
			xti = 1;
		}
	}
}
//主程序轮询
void uart3_poll(void) {
	EA = 0;
	if (state == UART_STATUS_RX) {
		if ((xindex) && (timer_expired(xtime, UART1_RX_PROCESS))) {
			xlength = 0;
			uart3_parse();
			xsessid++;
			if (xlength) {
				xti = 0;
				xindex = xlength;
				state = UART_STATUS_WAIT;
			} else {
				xindex = 0;
			}
		}
	} else if (state == UART_STATUS_TX) {
		if (xindex == xlength) {
			xindex = 0;
			state = UART_STATUS_RX;
		}
	}
	EA = 1;
}
