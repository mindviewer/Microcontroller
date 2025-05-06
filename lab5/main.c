#include "NuMicro.h"
#include "ADCAgent.h"
#include "TempSensor.h"
#include "system_init.h"
#include "display.h"
#include "tmr.h"
#include "GUI.h"
#include "sys.h"
#include "BNCTL.h"
#include "StepMotorAgent.h"
#include "UART1.h"
#include <stdio.h>

/* define max and mini speed */
#define MaxSpeed	10
#define MinSpeed	1
/* global variable define */
uint32_t timecount = 0;
uint8_t speed;
uint8_t  dir;

char c;
char sendbuf[100];
unsigned int baudrate;
char baudrate_buf[20];

void Select_mode (void);
void BTN_speed_control (void);
void ADC_speed_control (void);
void UART1_speed_control (void);

int main(void)
{	
	/* local variable define */
	char ADC_value_buf[20];
	char M487sensor_temp_value_buf[20];
	char thermistor_temp_value_buf[20];
	char speed_buf[20];
	char mode_buf[20];
  char receive_buf[20];
	
	uint8_t mode = 0;	
	uint8_t btn_pressed_once = 0;
	
	/* Init System, peripheral clock */
	SYS_Init();
		
	/* Init temputer sensor */
	Temp_Sensor_Enable();

	/* Init TMR0 for timecount */
	TMR0_Initial();
	
	/* Opem GUI display */
	Display_Init();

	/* Init ADC */
	ADC_Initial();

  /* Init Button */
  BTN_init();
	/* Init UART */
	
	UART1_Initial();

  /*Init Step Motor */
  StepMtr_Initial();
	dir = 1;
	speed = 5;
	baudrate = 115200;
	

	while(1)
	{	
		if (Btn_IsDown(0x01) && Btn_IsDown(0x02) && btn_pressed_once == 0) {
			mode = (mode == 2)? 0 : mode + 1;
			btn_pressed_once = 1;
		}
		else if (!(Btn_IsDown(0x01) && Btn_IsDown(0x02))){
			btn_pressed_once = 0;
	  }
		
		switch (mode) {
			case 0:
				BTN_speed_control();
			break;
			
			case 1:
				ADC_speed_control();
				break;
			case 2:
				UART1_speed_control();
			break;
			
			default: 
				BTN_speed_control();
			break;
		}



		/* Print ADC value */
		sprintf(ADC_value_buf, "ADC value : %03d", ADC_GetVR());
		Display_buf(ADC_value_buf, 1, 1);
		/* Print Sensor temperature */
		sprintf(M487sensor_temp_value_buf, "M487sensor_temp : %2.1f", ADC_GetM487Temperature());
		Display_buf(M487sensor_temp_value_buf, 1, 40);
		/* Print Thermistor temperature */
		sprintf(thermistor_temp_value_buf, "ThermistorTemp : %d", ADC_ConvThermistorTempToReal());
		Display_buf(thermistor_temp_value_buf, 1, 79);
		/* write motor state buffer */
		sprintf(speed_buf,"Speed : %02d rpm" , speed*6);//6~102
		Display_buf(speed_buf, 1, 118);
		
		sprintf(mode_buf, "Mode = %d", mode);
		Display_buf(mode_buf,1, 157);
		
		sprintf(baudrate_buf, "baudrate: %d " ,baudrate);
		Display_buf(baudrate_buf, 130, 196);
		
		sprintf(receive_buf, "received: %c", c);
		Display_buf(receive_buf, 1, 196);
						

		/* Drivers */
		/* Motor Task */
		StepMtr_Task(dir, speed);
		/* Get ADC value */
		ADC_Task();
		/* Scan button*/
		BTN_task();
	}
}

void UART1_speed_control (void){
		if(UART1_IsRxDataReady()){
			c = UART1_ReadByte();                    //read the c value from RealTerm into queue
			GUI_Clear();
			switch(c){
				case '+':                              //case when c is '+'
					if(speed == MaxSpeed){               //if(reach the max speed)
						StrPush("Max speed\r\n");          //send "Max speed" this string back to UART
					}else {
						speed ++;                          //otherwise, speed-up the motor
						StrPush("Speed Up\r\n");           //send "Speed Up" this string back to UART
				  }
					break;                               //exit this case
			  case '-':                              //case when c is '-'
					if(speed == MinSpeed || speed == 0){ //if((reach the min speed)or(speed=0))
						StrPush("Min speed\r\n");          //send "Min speed" this string back to UART
					}else {
						speed --;                          //otherwise, speed-down the motor
						StrPush("Speed Down\r\n");         //send "Speed Down" this string back to UART
				  }
					break;                               //exit this case
			  case 's':                              //case when c is 's'
					speed = 0;                           //stop the motor
				  StrPush("Stop\r\n");                 //send "Stop" this string back to UART
					break;                               //exit this case
				case 'r':                              //case when c is 'r'
					dir ^= 0x01;                         //change motor direction
				  StrPush("Reverse\r\n");              //send "Reverse" this string back to UART
				  break;                               //exit this case
				case 'p':                              //case when c is 'p'
					sprintf(sendbuf, "Speed: %d \r\nrpm: %d \r\nDirection: %s \r\n", speed, speed*6, (dir?"Clockwise" : "Counterclockwise"));
				                                       //write send_display_state_value buffer
				  StrPush(sendbuf);                    //send "Speed: ?
				                                       //      rpm: ?
				                                       //      Direction: Clockwise/Counterclockwise(decided by dir value)" this string back to UART
				
				/* change baudrate */
				case 'i':                              //case when c is 'i'
				
				baudrate = 9600;                       //set UART baud rate value = 9600bps
				ChangeBaudRate (baudrate);             //call function ChangeBaudRate() in UART1.c
				break;                                 //exit this case
				
				default:
					StrPush("Unknown\r\n");              //if c value doesn't match any case,  send "Unknown" this string back to UART
				break;                                 //exit this case
			}
		}
		UART1_TxTask();
	}
	
	
void BTN_speed_control (void) {
		if(Btn_IsOneShot(0x01) == 0x01){
			//speed control
			speed = 0;
			//clear the GUI display
			GUI_Clear();
			//clear one-shot flag
			Btn_OneShotClear(0x01);
		}
		if(Btn_IsOneShot(0x02) == 0x02){
			dir ^= 0x01;
			//clear the GUI display
			GUI_Clear();				
			Btn_OneShotClear(0x02);
		}
		if(Btn_IsOneShot(0x04) == 0x04){
			if(speed < MaxSpeed)
				speed ++;
			else
				speed = MaxSpeed;
			GUI_Clear();
			Btn_OneShotClear(0x04);
		}
		if(Btn_IsOneShot(0x08) == 0x08){
			if(speed > MinSpeed)
				speed --;
			else 
				speed = MinSpeed;
			
			GUI_Clear();
			Btn_OneShotClear(0x08);
		}
	}

void ADC_speed_control (void) {
	uint8_t v;
	v = ADC_GetVR() ;
	if(v<=30) {
		speed = 2;
	}
	else if (v>30 && v<=60) {
		speed = 5;
	}
	else if (v>60 && v<=90) {
		speed = 8;
	}
	else {
		speed = 10;
	}
}

