#ifndef __BNCTL_H__
#define __BNCTL_H__

#include "NuMicro.h"
#include "tmr.h"

#define SW1	PF11	//BUTTON LEFT
#define SW2	PG15	//BUTTON RIGHT
#define SW3	PA0		//TOP UP
#define SW4	PA1		//TOP DOWN

#define BTN_UP				0
#define BTN_BNC				1
#define BTN_ONESHOT		2
#define BTN_DOWN			3

#define DBC_DEL				1000

extern uint32_t	timecount;

void BTN_init(void);
void BTN_task(void);

uint8_t Btn_IsDown(uint8_t btnSelMask);
uint8_t Btn_IsOneShot(uint8_t btnSelMask);
void Btn_OneShotClear(uint8_t btnSelMask);

#endif	/* __BNCTL_H__ */
