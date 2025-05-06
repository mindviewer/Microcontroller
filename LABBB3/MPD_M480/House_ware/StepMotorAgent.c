#include "StepMotorAgent.h"

uint8_t StepMtrState;

void StepMtr_Initial(void){
	GPIO_SetMode(PG, BIT5, GPIO_MODE_OUTPUT) ;	 // MTR_A
	GPIO_SetMode(PG, BIT6, GPIO_MODE_OUTPUT) ;	 // MTR_B
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT) ;	 // MTR_A_BAR
	GPIO_SetMode(PC, BIT11, GPIO_MODE_OUTPUT) ;	 // MTR_B_BAR

	StepMtrState = 4;
	MTR_A     = 0;
	MTR_B     = 0;
	MTR_A_BAR = 0;
	MTR_B_BAR = 0;
}

void StepMtr_Task(uint8_t dir, uint32_t d){
	static uint32_t StepMtrOldCount = 0;
	
	if((uint32_t)(timecount - StepMtrOldCount) < d)
		return;
	StepMtrOldCount = timecount;
	
	if(d ==0 )
		StepMtrState = 4;
	//step motor FSM
	switch(StepMtrState){
		case 0:
				MTR_A     = 1;
				MTR_B     = 0;
				MTR_A_BAR = 0;
				MTR_B_BAR = 1;
				StepMtrState = dir ? 1:3;
			break;
		case 1:
				MTR_A     = 1;
				MTR_B     = 1;
				MTR_A_BAR = 0;
				MTR_B_BAR = 0;
				StepMtrState = dir ? 2:0;
			break;
		case 2:
				MTR_A     = 0;
				MTR_B     = 1;
				MTR_A_BAR = 1;
				MTR_B_BAR = 0;
				StepMtrState = dir ? 3:1;
			break;
		case 3:
				MTR_A     = 0;
				MTR_B     = 0;
				MTR_A_BAR = 1;
				MTR_B_BAR = 1;
				StepMtrState = dir ? 0:2;
			break;
		case 4:
				MTR_A     = 0;
				MTR_B     = 0;
				MTR_A_BAR = 0;
				MTR_B_BAR = 0;
				StepMtrState = d	 ? 0:4;
			break;
		default:
			StepMtrState = 4;
	}
}
