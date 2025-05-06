#include "M480.h"                                     //include the functions library of M480 microcontroller

int32_t main(void)                                    //main function
{
	
	// Input Pin set
	GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT);            //configures input pin PA.0 for the Button(SW1), as picture above
	GPIO_SetMode(PA, BIT1, GPIO_MODE_INPUT);            //configures input pin PA.1 for the Button(SW2)
	
	GPIO_SetMode(PG, BIT2, GPIO_MODE_INPUT);            //configures input pin PG.2 for the Joystick(Up)
	GPIO_SetMode(PG, BIT3, GPIO_MODE_INPUT);            //configures input pin PG.3 for the Joystick(Center)
	GPIO_SetMode(PC, BIT10, GPIO_MODE_INPUT);           //configures input pin PC.10 for the Joystick(Down)
	
	// Output Pin set
	GPIO_SetMode(PH, BIT6, GPIO_MODE_OUTPUT);           //configures output pin PH.6 for the LEDR1(red)
	GPIO_SetMode(PH, BIT7, GPIO_MODE_OUTPUT);           //configures output pin PH.7 for the LEDG1(green)
	
	while(1) {                                          //it runs an infinite while loop, logically while(1) always true
		
		PH6 = (PA0 == 0 || PG2 == 0 || PG3 == 0) ? 0 : 1; //if (PA0=0 or PG2=0 or PG3=0),then PH6=0
		                                                  //in other words, if we (push SW1)or(push Joystick Up)or(push Joystick Center), LEDR1 ON
		
		PH7 = (PA1 == 0 || PC10 == 0 || PG3 == 0) ? 0 : 1;//if we (push SW2)or(push Joystick Down)or(push Joystick Center), LEDG1 ON

	}
}
