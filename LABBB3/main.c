#include "stdio.h"
#include "NuMicro.h"
#include "tmr.h"
#include "system_init.h"
#include "GUI.h"
#include "display.h"
#include "BNCTL.h"
#include "StepMotorAgent.h"

/* define max and mini speed */
#define MaxSpeed	17
#define MinSpeed	1

/* global variable define */
uint32_t	timecount;
uint8_t		dir;
uint32_t	speed;
	
int main(void){
	/* local variable define */
	char motor_state_buf[30];
	char SPD_buf[30];
	char direction_buf[30];
	uint32_t  speedCTL;
	
	/* Init System */
  SYS_Init();
	
	/*button initialize*/
	BTN_init();
	
	/*Step Motor initialize*/
	StepMtr_Initial();
	
    /* GUI display initialize */
	Display_Init();
	
	/* Init TMR0 for timecount */
	TMR0_Initial();
	
	/* Set initial value for speed direction */
	dir = 0x01;                        //set the initial direction value
	speed = 10;                        //set the initial speed value
		
    while(1){                        //infinite loop
		/* Scan button */
		BTN_task();

		if(Btn_IsOneShot(0x01) == 0x01){ //check whether the button is in 0x01 one-shot state
            //stop
			speed = 0;                     //set speed=0 to stop the motor
			GUI_Clear();
			Btn_OneShotClear(0x01);        //clear the one-shot state
		}
		if(Btn_IsOneShot(0x02) == 0x02){ //check whether the button is in 0x02 one-shot state
            //direction control
			dir ^= 0x01;                   //change dir value to determine clockwise/counterclockwise
			GUI_Clear();
			Btn_OneShotClear(0x02);        //clear the one-shot state
		}				
		if(Btn_IsOneShot(0x04) == 0x04){ //check whether the button is in 0x04 one-shot state
            //speed up
			if(speed < MaxSpeed)           //if speed value less than 17
				speed ++;                    //speed-up the motor
			else
				speed = MaxSpeed;            //else, remain at the maximum speed=17
			GUI_Clear();
			Btn_OneShotClear(0x04);        //clear the one-shot state
		}
		if(Btn_IsOneShot(0x08) == 0x08){ //check whether the button is in 0x08 one-shot state
            //speed down
			if(speed > MinSpeed)           //if speed value more than 1
				speed --;                    //speed-down the motor
      else
				speed = MinSpeed;            //else, remain at the minimum speed=1
			GUI_Clear();
			Btn_OneShotClear(0x08);        //clear the one-shot state
		}
					
		/* Step motor output */
		if(speed)
			speedCTL = 1000/speed;
		else
			speedCTL = 0;
				
		StepMtr_Task(dir, speedCTL);
				
        //writ motor state buffer
		sprintf(SPD_buf,"speed : %02d rpm" , speed*6);//6~102
        //writ direction buffer

        //Display_buf(, 1, 1);motor state
    if(speed == 0){
			Display_buf("motor state : Stopped", 1, 1);        //if motor stop(speed=0), display this string on the screen
    }
    else{
			Display_buf("motor state : Running", 1, 1);        //else, display this string on the screen
    }

		Display_buf(SPD_buf, 1, 25);
    if(dir)
			Display_buf("direction : Clockwise", 1, 49);       //if (dir) is true, display this string on the screen
    else
      Display_buf("direction : CounterClockwise", 1, 49);//else, display this string on the screen
       
	  }
}