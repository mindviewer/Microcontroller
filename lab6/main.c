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
#include "I2C_EEPROM.h"
#include <stdio.h>

/* define max and mini speed */
#define MaxSpeed	10
#define MinSpeed	1

/* global variable define */
uint32_t timecount;
uint8_t BTN_speed;
uint8_t ADC_speed;
uint8_t UART_speed;
uint8_t speed;
uint8_t dir;
uint8_t sec;
uint8_t min;
uint8_t hour;
uint8_t BAUD_DIV_LOW, BAUD_DIV_HIGH;
uint8_t CMDlen, CMDstate;
uint16_t BAUD_DIV;
uint32_t baudrate;

char c;
char sendbuf[100];
char baudrate_buf[20];
char CMD[20];
char *BAUDCMD = "BAUD=";
char buf[20];	                    //define clock buffer variable

unsigned int CMD_NUM;

void Select_mode (void);
void BTN_speed_control (void);
void ADC_speed_control (void);
void UART1_speed_control (void);
void EEPROM_control (void);
void SaveDataToEEPROM	(void);
void ReadDataFromEEPROM	(void);
int ConfigWithEEPROM (void);
void SaveAge (void);
void ClearEEPROM(void);
void clock_tick(void);            //define clock update function

typedef union{
	struct{
		uint8_t CHECK;		//0x0001
		uint32_t BR;			//0x0002 ~ 0x0005
		uint8_t SPEED;		//0x0006
		uint8_t DIR;			//0x0007
		uint8_t MIN;			//0x0008
	};
	uint8_t DATA[10];
}EEPROM_table;

EEPROM_table eepromData;

int main(void)
{	
	
	/* local variable define */
	char ADC_value_buf[20];
	char M487sensor_temp_value_buf[20];
	char thermistor_temp_value_buf[20];
	char speed_buf[20];
	char mode_buf[20];
  char receive_buf[20];
	char clock_buf[20];

	
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
	
	/*Init I2C_EEPROM*/
	I2C_EEPROM_Init();
	
	/*Init EEPROM buffer data*/
	if(!ConfigWithEEPROM()){
		baudrate = 115200;
		ChangeBaudRate (baudrate);
		UART_speed = 5;
		dir = 1;
		min = 0;
	}
	
	CMDlen = 0;
	CMDstate = 0;
	
	while(1){
		
		if (Btn_IsDown(0x01) && Btn_IsDown(0x02) && btn_pressed_once == 0) {
			mode = (mode == 3)? 0 : mode + 1;
			btn_pressed_once = 1;
		}
		else if (!(Btn_IsDown(0x01) && Btn_IsDown(0x02))){
			btn_pressed_once = 0;
	  }
				
		switch (mode) {
			case 0:
				BTN_speed_control();
				speed = BTN_speed;
				break;
			
			case 1:
				ADC_speed_control();
				speed = ADC_speed;
				break;
			
			case 2:
				UART1_speed_control();
				speed = UART_speed;
				break;
			
			case 3:
				EEPROM_control();
				speed = UART_speed;
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
		/* Print Mode*/
		sprintf(mode_buf, "Mode = %d", mode);
		Display_buf(mode_buf,1, 157);
		/* Print Baudrate*/
		sprintf(baudrate_buf, "baudrate: %d " ,baudrate);
		Display_buf(baudrate_buf, 130, 196);
		/* Print received char*/
		sprintf(receive_buf, "received: %c", c);
		Display_buf(receive_buf, 1, 196);
		
		clock_tick();                 //call clock update function

		/* Drivers */
		/* Motor Task */
		StepMtr_Task(dir, speed);
		/* Get ADC value */
		ADC_Task();
		/* Scan button*/
		BTN_task();
			

		
	}
}

void ClearEEPROM()
{
	I2C_EEPROM_Write(0x0001, 0x00);
}

void SaveAge (void)
{
	I2C_EEPROM_Write(0x0008, min);
	eepromData.MIN = min;
}

int ConfigWithEEPROM (void)
{
	ReadDataFromEEPROM();
	if(eepromData.CHECK != 0xA5){
		return 0;
	}
	baudrate = eepromData.BR;
	UART_speed = eepromData.SPEED;
	dir = eepromData.DIR;
	min = eepromData.MIN;
	ChangeBaudRate (baudrate);
	return 1;
}
	


void SaveDataToEEPROM	(void)
{
	uint16_t i = 0x0001;
	
	eepromData.CHECK = 0xA5;
	for(i = 0x0001; i < 0x000A; i++){
		I2C_EEPROM_Write(i, eepromData.DATA[i-1]);	//set data in EEPROM
	}
}

void ReadDataFromEEPROM	(void)
{
	uint16_t i = 0x0001;
	
	for(i = 0x0001; i < 0x000A; i++){
		eepromData.DATA[i-1] = I2C_EEPROM_Read(i);	//Read data from EEPROM
	}
		
}
void EEPROM_control (void)
{
	
	if(Btn_IsOneShot(0x01) == 0x01) {
		ClearEEPROM();
	}
	
	if(Btn_IsOneShot(0x04) == 0x04){
		eepromData.BR = baudrate;
		eepromData.SPEED = UART_speed;
		eepromData.DIR = dir;
		SaveDataToEEPROM();
		Btn_OneShotClear(0x04);
	}
	
	if(Btn_IsOneShot(0x08) == 0x08){
		ReadDataFromEEPROM();
		baudrate = eepromData.BR;
		UART_speed = eepromData.SPEED;
		dir = eepromData.DIR;
		min = eepromData.MIN;
		ChangeBaudRate (baudrate);
		GUI_Clear();
		Btn_OneShotClear(0x08);
	}
	
}
	
void UART1_speed_control (void){
		if(UART1_IsRxDataReady()){
			c = UART1_ReadByte();
			GUI_Clear();
			switch(c){
				case '+':
					if(UART_speed == MaxSpeed){
						StrPush("Max speed\r\n");
					} 
					else {
						UART_speed++;
						StrPush("Speed Up\r\n");
					}
					break;
					
				case '-':
					if(UART_speed == MinSpeed || UART_speed == 0){
						StrPush("Min speed\n\r");
					} 
					else {
						UART_speed--;
						StrPush("Speed Down\r\n");
					}
					break;
					
				case 's':
					UART_speed = 0;
					StrPush("Stop\r\n");
					break;
				
				case 'r':
					dir ^= 1;
					StrPush("Reverse\r\n");
					break;
				
				case 'p':
					sprintf(sendbuf, "Speed: %d \r\nrpm: %d \r\nDirection: %s \r\n", UART_speed, UART_speed*6 , (dir ? "Clockwise" : "Counterclockwise"));
					StrPush(sendbuf);
					break;
				
				default:
				switch(CMDstate){
					case 0:
						if(c == BAUDCMD[CMDlen]){
							CMDlen++;
						}
						else {
							CMDlen = 0;
							StrPush("Error\n");
						}
						if(BAUDCMD[CMDlen] == 0x00){
							CMDlen = 0;
							CMDstate = 1;
						}
						break;
					case 1:
						if(c != 0x0D){
							CMD[CMDlen++] = c;
						}
						else {
							CMD[CMDlen++]=0x00;                               //clear received CMD[] string in the back
							sscanf(CMD,"%d", &CMD_NUM);	                      //read CMD value and write integral number into CMD_NUM
							sprintf(sendbuf,"Get CMD : BUAD=%d\r\n",CMD_NUM); //write sendbuf
							StrPush(sendbuf);                                 //send "Get CMD : BUAD=CMD_NUM" this string back to UART
							baudrate=CMD_NUM;                                 //update baudrate value
							UART1_TxData();                                   //function: transmit data to UART
							CMDstate=0;                                       //reset state to case 0
							CMDlen=0;                                         //reset variable
							ChangeBaudRate(baudrate);                         //call function ChangeBaudRate() in UART1.c
						}
						break;
				}	
			}
		}
		UART1_TxTask();
}
void BTN_speed_control (void) {
		if(Btn_IsOneShot(0x01) == 0x01){
			//speed control
			BTN_speed = 0;
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
			if(BTN_speed < MaxSpeed)
				BTN_speed ++;
			else
				BTN_speed = MaxSpeed;
			GUI_Clear();
			Btn_OneShotClear(0x04);
		}
		if(Btn_IsOneShot(0x08) == 0x08){
			if(BTN_speed > MinSpeed)
				BTN_speed --;
			else 
				BTN_speed = MinSpeed;
			
			GUI_Clear();
			Btn_OneShotClear(0x08);
		}
	}
void ADC_speed_control (void) {
	uint8_t v;
	v = ADC_GetVR() ;
	if(v<=30) {
		ADC_speed = 2;
	}
	else if (v>30 && v<=60) {
		ADC_speed = 5;
	}
	else if (v>60 && v<=90) {
		ADC_speed = 8;
	}
	else {
		ADC_speed = 10;
	}
}
void clock_tick(void){                              //clock update function
	static uint32_t old_timecount = 0;
	
	if((uint32_t)(timecount - old_timecount) < 10000) //default SpeedCtl : 1Hz
		return;
		
		old_timecount = timecount;	                    //update the old time
		sec++;                                          // update second

	//time update
	if (sec == 60){                                   //sec count to 60
        sec = 0;                                    //reset sec value
        min++;                                      // update minute
		SaveDataToEEPROM();                             //function: save data to EEPROM
    if (min == 60){                                 //min count to 60
            min = 0;                                //reset min value
            hour++;                                 // update hour
            if (hour == 24) hour = 0;               // reset hour when hour count to 24
        }
    }
    sprintf(buf, "%02d", min);                      // clock buffer
    Display_buf(buf, 240, 1);                       // display clock
}
	
