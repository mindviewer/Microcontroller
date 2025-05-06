#ifndef __STEPMOTORAGENT_H__
#define __STEPMOTORAGENT_H__

#include "NuMicro.h"
#include "tmr.h"

#define MTR_A			PG5
#define MTR_B			PG6
#define MTR_A_BAR	PC12
#define MTR_B_BAR	PC11

extern uint32_t timecount;

void StepMtr_Initial(void);
void StepMtr_Task(uint8_t dir, uint32_t d);

#endif  /* __STEPMOTORAGENT_H__ */


