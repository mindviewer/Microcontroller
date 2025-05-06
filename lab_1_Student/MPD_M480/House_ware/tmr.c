#include "NuMicro.h"
#include "tmr.h"
#include "time.h"

extern uint32_t timecount;
 
void TMR0_Initial(void)
{
		CLK->APBCLK0 |= CLK_APBCLK0_TMR0CKEN_Msk;			//TMR0_clock enable
		
		CLK->CLKSEL1 &= ~CLK_CLKSEL1_TMR0SEL_Msk;			//TMR0 clock select CTL pin reset to 0
		CLK->CLKSEL1 |= CLK_CLKSEL1_TMR0SEL_HXT;			//TMR0 clock source select XTAL 12MHz
	
		TIMER0->CTL	&= TIMER_CTL_PSC_Msk;							//TIMER0 prescale CTL reset
		TIMER0->CTL |= 0x00 << TIMER_CTL_PSC_Pos;			//TIMER0 prescale	= 0
	
		TIMER0->CTL &= TIMER_CTL_OPMODE_Msk;					//TIMER0 OPMODE CTL reset
		TIMER0->CTL |= TIMER_PERIODIC_MODE;						//TIMER0 OPMODE = Periodic mode
		//10kHz
		TIMER0->CMP = 1200UL;//Time-out period = (Period of timer clock input) * (8-bit PSC + 1) * (24-bit CMPDAT(12k))
	
		TIMER0->CTL |= TIMER_CTL_INTEN_Msk;						//TIMER0 interrupt enable
		TIMER0->CTL |= TIMER_CTL_CNTEN_Msk;						//TIMER0 counting enable
		NVIC_EnableIRQ(TMR0_IRQn);
}

void TMR0_IRQHandler(void)
{
		 if (TIMER_GetIntFlag(TIMER0)) {
				timecount++;
				TIMER_ClearIntFlag(TIMER0);	//timer->EINTSTS = TIMER_EINTSTS_CAPIF_Msk(FLAG);
		 }
}

