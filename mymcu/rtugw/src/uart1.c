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

static void uart1_isr(void) interrupt UART1_VECTOR {
	EA = 0;
	if (RI) {
		RI = 0;
		xsessid++;
		timer_get(xtime);
		if (state == UART_STATUS_RX) {
			if (xindex < BUF1_LENGTH) {
				xbuf[xindex++] = SBUF;
			} else {
				xindex = BUF1_LENGTH + 1;
			}
		} else {
			state = UART_STATUS_RX;
			xindex = 1;
			xbuf[0] = SBUF;
		}
	}
	if (TI) {
		TI = 0;
		xti = 0;
		if ((state == UART_STATUS_TX) || (state == UART_STATUS_WAIT_C)) {
			if (xindex != xlength) {
				if ((xindex > xlength) || (xlength >= BUF1_LENGTH)) {
					xindex = 0;
					state = UART_STATUS_RX;
				} else {
					xti = 1;
					SBUF = xbuf[xindex++];
				}
			}
		}
	}
	EA = 1;
}

void uart1_init(void) {
	u32	j;

	SCON = (SCON & 0x3f) | UART_8bit_BRTx;	//模式设置
	j = (MAIN_Fosc / 4) / UART1_BAUDRATE;//115200ul;	//按1T计算
	//if(j >= 65536UL)	return FAIL;	//错误
	j = 65536UL - j;
	//j--;
	//_JJ = j;
	//if(COMx->UART_BRT_Use == BRT_Timer1)
	//{
		//	TR1 = 0;
		//	AUXR &= ~0x01;		//S1 BRT Use Timer1;
		//	TMOD &= ~(1<<6);	//Timer1 set As Timer
		//	TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		//	AUXR |=  (1<<6);	//Timer1 set as 1T mode
		//	TH1 = (u8)(j>>8);
		//	TL1 = (u8)j;
		//	ET1 = 0;	//禁止中断
		//	TMOD &= ~0x40;	//定时
		//	INT_CLKO &= ~0x02;	//不输出时钟
		//	TR1  = 1;
	//}
	//else if(COMx->UART_BRT_Use == BRT_Timer2)
	//{
		AUXR &= ~(1<<4);	//Timer stop
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		AUXR &= ~(1<<3);	//Timer2 set As Timer
		AUXR |=  (1<<2);	//Timer2 set as 1T mode
		TH2 = (u8)(j>>8);
		TL2 = (u8)j;
		IE2  &= ~(1<<2);	//禁止中断
		AUXR |=  (1<<4);	//Timer run enable
	//}

	UART1_RxEnable(1);	//UART接收使能

	//NVIC_UART1_Init(ENABLE,Polity_2);
	UART1_Interrupt(ENABLE);
	UART1_Polity(Polity_2);

	UART1_SW(UART1_SW_P30_P31);

	state = UART_STATUS_INIT;
}

static void uart1_parse(void) {
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

u16 uart1_sch(u8 *sessid, u8 *buf) {
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
void uart1_pip(u8 sessid, u8 c) {
	if ((sessid == xsessid) && (state == UART_STATUS_WAIT_C)) {
		if (xlength < BUF1_LENGTH) {
			xbuf[xlength++] = c;
			if (xti == 0) {
				TI = 1;
				xti = 1;
			}
		}
	}
}

void uart1_complted(u8 sessid) {
	if ((sessid == xsessid) && (state == UART_STATUS_WAIT_C)) {
		xsessid++;
		state = UART_STATUS_TX;
		if (xti == 0) {
			TI = 1;
			xti = 1;
		}
	}
}
//主程序轮询
void uart1_poll(void) {
	EA = 0;
	if (state == UART_STATUS_RX) {
		if ((xindex) && (timer_expired(xtime, UART1_RX_PROCESS))) {
			xlength = 0;
			uart1_parse();
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
