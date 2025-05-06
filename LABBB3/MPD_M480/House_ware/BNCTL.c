#include "BNCTL.h"

static uint8_t BTNDownFlag;
static uint8_t BTNoneshot;
static uint8_t BTNState[4];
static uint32_t BTNOldCount[4];
static uint8_t bt[4];		

void BTN_init(void)
{
	int i;
	GPIO_SetMode(PF, BIT11, GPIO_MODE_INPUT) ; // SW1 BUTTON LEFT stop
	GPIO_SetMode(PG, BIT15, GPIO_MODE_INPUT) ; // SW2 BUTTON RIGHT change direction
	GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT) ; // SW3 TOP UP speed up
	GPIO_SetMode(PA, BIT1, GPIO_MODE_INPUT) ; // SW4 TOP DOWN speed down
	BTNDownFlag = 0;
	BTNoneshot = 0;

	for(i = 0 ; i < 4 ; i++ )
	{
		BTNState[i] = BTN_UP;
		BTNOldCount[i] = 0;
	}

}

void BTN_task(void)
{
	int i;
	bt[0] = SW1;
	bt[1] = SW2;
	bt[2] = SW3;
	bt[3] = SW4;
	for(i = 0 ; i<4 ; i++)
	{
		switch(BTNState[i])
		{
			
			case BTN_UP:
				if(bt[i] == 0)
				{
					BTNState[i] = BTN_BNC;
				}
				BTNOldCount[i] = timecount;
				BTNDownFlag &= ~(0x01 << i);
				BTNoneshot &= ~(0x01 << i);
				break;
				
			case BTN_BNC:
				if(bt[i] == 1)
				{
					BTNState[i] = BTN_UP;
				}
				else if((uint32_t)(timecount - BTNOldCount[i]) > DBC_DEL)
				{
					BTNState[i] = BTN_ONESHOT;
				}
				break;
				
			case BTN_ONESHOT:
				BTNState[i] = BTN_DOWN;
			  BTNoneshot |= 0x01<<i;
			  break;
			
			case BTN_DOWN:
				if(bt[i] == 1)
				{
					BTNState[i] = BTN_UP;
				}
				else
				{
					BTNDownFlag |= (0x01 << i);
				}
				break;
				
			default:
				BTNState[i] = BTN_UP;
				break;
		}//switch
	}//for
}
uint8_t Btn_IsDown(uint8_t btnSelMask)
{
	return BTNDownFlag & btnSelMask;
}
uint8_t Btn_IsOneShot(uint8_t btnSelMask)
{
	return BTNoneshot & btnSelMask;
}
void Btn_OneShotClear(uint8_t btnSelMask)
{
	BTNoneshot &= ~btnSelMask;
}