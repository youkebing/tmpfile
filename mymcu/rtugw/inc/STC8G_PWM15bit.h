/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef	__PWM15BIT_H
#define	__PWM15BIT_H

#include	"config.h"

//========================================================================
//                              PWM����
//========================================================================

#define PWM15_PWM0_Set(n)					(n==1?(PWMSET |= 0x01):(PWMSET &= ~0x01))				//ʹ��/�ر� PWM0������ PWM00~PWM07��
#define PWM15_PWM1_Set(n)					(n==1?(PWMSET |= 0x02):(PWMSET &= ~0x02))				//ʹ��/�ر� PWM1������ PWM10~PWM17��
#define PWM15_PWM2_Set(n)					(n==1?(PWMSET |= 0x04):(PWMSET &= ~0x04))				//ʹ��/�ر� PWM2������ PWM20~PWM27��
#define PWM15_PWM3_Set(n)					(n==1?(PWMSET |= 0x08):(PWMSET &= ~0x08))				//ʹ��/�ر� PWM3������ PWM30~PWM37��
#define PWM15_PWM4_Set(n)					(n==1?(PWMSET |= 0x10):(PWMSET &= ~0x10))				//ʹ��/�ر� PWM4������ PWM40~PWM47��
#define PWM15_PWM5_Set(n)					(n==1?(PWMSET |= 0x20):(PWMSET &= ~0x20))				//ʹ��/�ر� PWM5������ PWM50~PWM54��

#define PWM0_Clock_Sel(n)					(n==1?(PWM0CKS |= 0x10):(PWM0CKS &= ~0x10))			//PWMnʱ��Դѡ��
#define PWM1_Clock_Sel(n)					(n==1?(PWM1CKS |= 0x10):(PWM1CKS &= ~0x10))			//PWMnʱ��Դѡ��
#define PWM2_Clock_Sel(n)					(n==1?(PWM2CKS |= 0x10):(PWM2CKS &= ~0x10))			//PWMnʱ��Դѡ��
#define PWM3_Clock_Sel(n)					(n==1?(PWM3CKS |= 0x10):(PWM3CKS &= ~0x10))			//PWMnʱ��Դѡ��
#define PWM4_Clock_Sel(n)					(n==1?(PWM4CKS |= 0x10):(PWM4CKS &= ~0x10))			//PWMnʱ��Դѡ��
#define PWM5_Clock_Sel(n)					(n==1?(PWM5CKS |= 0x10):(PWM5CKS &= ~0x10))			//PWMnʱ��Դѡ��

#define PWM15_Counter0_Set(n)			(n==1?(PWMCFG01 |= 0x01):(PWMCFG01 &= ~0x01))		//PWM0������ʹ������
#define PWM15_Counter1_Set(n)			(n==1?(PWMCFG01 |= 0x10):(PWMCFG01 &= ~0x10))		//PWM1������ʹ������
#define PWM15_Counter2_Set(n)			(n==1?(PWMCFG23 |= 0x01):(PWMCFG23 &= ~0x01))		//PWM2������ʹ������
#define PWM15_Counter3_Set(n)			(n==1?(PWMCFG23 |= 0x10):(PWMCFG23 &= ~0x10))		//PWM3������ʹ������
#define PWM15_Counter4_Set(n)			(n==1?(PWMCFG45 |= 0x01):(PWMCFG45 &= ~0x01))		//PWM4������ʹ������
#define PWM15_Counter5_Set(n)			(n==1?(PWMCFG45 |= 0x10):(PWMCFG45 &= ~0x10))		//PWM5������ʹ������

#define PWM15_Counter0Int_Set(n)	(n==1?(PWMCFG01 |= 0x04):(PWMCFG01 &= ~0x04))		//PWM0�����������ж�ʹ������
#define PWM15_Counter1Int_Set(n)	(n==1?(PWMCFG01 |= 0x40):(PWMCFG01 &= ~0x40))		//PWM1�����������ж�ʹ������
#define PWM15_Counter2Int_Set(n)	(n==1?(PWMCFG23 |= 0x04):(PWMCFG23 &= ~0x04))		//PWM2�����������ж�ʹ������
#define PWM15_Counter3Int_Set(n)	(n==1?(PWMCFG23 |= 0x40):(PWMCFG23 &= ~0x40))		//PWM3�����������ж�ʹ������
#define PWM15_Counter4Int_Set(n)	(n==1?(PWMCFG45 |= 0x04):(PWMCFG45 &= ~0x04))		//PWM4�����������ж�ʹ������
#define PWM15_Counter5Int_Set(n)	(n==1?(PWMCFG45 |= 0x40):(PWMCFG45 &= ~0x40))		//PWM5�����������ж�ʹ������

//========================================================================
//                              ��������
//========================================================================

#define	PWM0				0
#define	PWM1				1
#define	PWM2				2
#define	PWM3				3
#define	PWM4				4
#define	PWM5				5

#define	PWM00				0
#define	PWM01				1
#define	PWM02				2
#define	PWM03				3
#define	PWM04				4
#define	PWM05				5
#define	PWM06				6
#define	PWM07				7
#define	PWM10				8
#define	PWM11				9
#define	PWM12				10
#define	PWM13				11
#define	PWM14				12
#define	PWM15				13
#define	PWM16				14
#define	PWM17				15
#define	PWM20				16
#define	PWM21				17
#define	PWM22				18
#define	PWM23				19
#define	PWM24				20
#define	PWM25				21
#define	PWM26				22
#define	PWM27				23
#define	PWM30				24
#define	PWM31				25
#define	PWM32				26
#define	PWM33				27
#define	PWM34				28
#define	PWM35				29
#define	PWM36				30
#define	PWM37				31
#define	PWM40				32
#define	PWM41				33
#define	PWM42				34
#define	PWM43				35
#define	PWM44				36
#define	PWM45				37
#define	PWM46				38
#define	PWM47				39
#define	PWM50				40
#define	PWM51				41
#define	PWM52				42
#define	PWM53				43
#define	PWM54				44
#define	PWM55				45
#define	PWM56				46
#define	PWM57				47

#define	PWMn_CLK_SYS			0
#define	PWMn_CLK_TM2			1

typedef struct
{
	u8	PWM_Enable;	//PWMʹ��,  ENABLE, DISABLE
	u8	PWM_Counter;		//������ʹ��,  ENABLE, DISABLE
	u8	PWM_CInt;				//�����������ж�ʹ��,  ENABLE, DISABLE
	u8	PWM_Clock_Sel;	//ʱ��Դѡ��,  PWMn_CLK_SYS, PWMn_CLK_TM2
	u8	PWM_Clock_PS;		//ϵͳʱ�ӷ�Ƶ����,  0~15
	u16	PWM_Period;			//PWM����,  0~0x7fff
} PWM15_InitTypeDef;

u8	PWM15_Init(u8 PWM_id, PWM15_InitTypeDef *PWMx);
u8	PWMChannelCtrl(u8 PWM_id, u8 pwm_eno, u8 pwm_ini, u8 pwm_eni, u8 pwm_ent2i, u8 pwm_ent1i);
u8	PWMLevelSet(u8 PWM_id, u8 pwm_hldl, u8 pwm_hldh);
u8	PWM15Duty(u8 PWM_id, u16 dutyL, u16 dutyH);

#endif

