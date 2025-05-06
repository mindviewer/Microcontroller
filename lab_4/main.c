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

/* define max and mini speed */
#define MaxSpeed	17
#define MinSpeed	1

/* global variable define */
uint32_t timecount = 0;
uint32_t speed;
uint8_t  dir;
uint8_t speed_control_mode;         //define mode_switching variable

int main(void)
{	
	/* local variable define */
	char ADC_value_buf[20];
	char M487sensor_temp_value_buf[20];
	char thermistor_temp_value_buf[20];
	char speed_buf[20];
	char speed_control_mode_buf[20];  //define mode_switching buffer
  uint32_t  speedCTL;
	
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
	
  /*Init Step Motor */
  StepMtr_Initial();
	dir = 1;
	speed = 10;
	speed_control_mode = 0x00;        //set the initial mode_switching value

	while(1)
	{	
		if(Btn_IsOneShot(0x01) == 0x01){
			//speed control
			speed = 0;
			//clear the GUI display
			GUI_Clear();
			//clear one-shot flag
			Btn_OneShotClear(0x01);
		}
		if(Btn_IsOneShot(0x02) == 0x02){
			if(Btn_IsDown(0x01) == 0x01)
				speed_control_mode ^= 0x01; //press SW1 & SW2 simultaneously to switch to use ADC value for the motor speed
			else
				dir ^= 0x01;                //change its direction
			
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

		/* Step motor output */
		if(speed_control_mode == 0x00){ //original button control mode(from lab3)
			if(speed)
				speedCTL = 1000/speed;      //motor speed control
			else
				speedCTL = 0;
		}else{                          //ADC mode
			speedCTL = ADC_GetVR()*8;     //read the ADC value from ADCAgent.c to speedCTL
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
		sprintf(speed_control_mode_buf, "mode : %d", speed_control_mode); //write mode_switching buffer
		Display_buf(speed_control_mode_buf, 1, 150);                      //mode_switching display, Display_buf(value, x-axis, y-axis)
		
		/* Drivers */
		/* Motor Task */
		StepMtr_Task(dir, speedCTL);
		/* Get ADC value */
		ADC_Task();
		/* Scan button*/
		BTN_task();
		
	}
}
