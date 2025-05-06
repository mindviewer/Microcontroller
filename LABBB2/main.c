#include "stdio.h"
#include "NuMicro.h"
#include "tmr.h"
#include "system_init.h"
#include "GUI.h"
#include "display.h"

/* define */
#define MaxSpeed	1000		//10Hz(0.1s)	led toogle speed
#define MinSpeed	50000		//0.2Hz(5s)		led toogle speed
#define SW_UP			PC9			//UP					JoyStick
#define SW_DOWN		PG4			//DOWN				JoyStick
#define SW_CTR		PG3			//CENTER			JoyStick

/* global variable define */
uint32_t timecount;
uint32_t sec = 0;
uint32_t hour = 0;
uint32_t min = 0;
char buf[20];

//function define
void Clock_Task(void);
void clock_init(void);
void clock_tick(void);
void LED_showing(uint32_t SpeedCtl);
void GPIO_init(void);
uint32_t JoyStick(unsigned char BTN_state);

//In order to enable it to compile in current Keil's version
#if defined(__CC_ARM)
#pragma anon_unions
#elif defined(__ICCARM__)
#endif

typedef union{
	struct{
		unsigned UP		:1;
		unsigned DOWN	:1;
		unsigned CTR	:1;
	};
unsigned char B;
}Pins;

Pins joystick_pins;

int main(void)
{
	/* Init System, peripheral clock and multi-function I/O */
	SYS_Init();
	
	/* Init TMR0 for timecount */
	TMR0_Initial();
	
	/* Set Initialization time */
	clock_init();
	
	/* Set GPIO MODE */
	GPIO_init();
	
	/* Opem GUI display */
	Display_Init();
		
	while(1)
	{
		/* local variable define */
		char clock_buf[20];
			
		joystick_pins.UP = SW_UP;     //SW_UP
    joystick_pins.DOWN = SW_DOWN; //SW_DOWN
    joystick_pins.CTR = SW_CTR;	  //SW_CTR;
    
		/* LED and clock function */
    LED_showing(JoyStick(joystick_pins.B)); //function controls the LED display based on joystick pins input.
		clock_tick();                           //clock update
		
		/* write buffer */
		sprintf(clock_buf, "%02d:%02d:%02d", hour, min, sec);
		
		/* print buffer */
		Display_buf(clock_buf, 270, 1);	//clock display :	Display_buf( buffer, x-axis, y-axis)
	}
}
/* GPIO initialize */
void GPIO_init(void)
{
		GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT) ;	 // SW1
		GPIO_SetMode(PH, BIT6, GPIO_MODE_OUTPUT) ; // LEDR1
		GPIO_SetMode(PH, BIT7, GPIO_MODE_OUTPUT) ; // LEDG1
		GPIO_SetMode(PC, BIT9, GPIO_MODE_INPUT) ;	 // Joystyick_LEFT(UP)
		GPIO_SetMode(PG, BIT4, GPIO_MODE_INPUT) ;	 // Joystyick_RIGHT(DOWN)
		GPIO_SetMode(PG, BIT3, GPIO_MODE_INPUT) ;	 // Joystyick_CENTER
}

//time initialize
void clock_init(void){
		sec = 0;
		min = 0;
		hour = 0;
}

/* clock update function */
void clock_tick(void)
{
	static uint32_t old_timecount = 0;
	
	if((uint32_t)(timecount - old_timecount) < 10000)//default SpeedCtl : 1Hz
		return;
		
	old_timecount = timecount;	//update the old time
	sec++;

	//time update
	if (sec == 60)
  {
    sec = 0;
    min++;
    if (min == 60)
    {
        min = 0;
        hour++;
        if (hour == 24)
          hour = 0;
    }
  }
  sprintf(buf, "%02d:%02d:%02d", hour, min, sec); //write clock buffer
  Display_buf(buf, 100, 100);                     //clock display :	Display_buf( buffer, x-axis, y-axis)
}

static uint32_t old_timecount = 0; //old time
static uint32_t SpeedCtl = 10000;  //set default speed frequency

/* JoyStick control */
uint32_t JoyStick(unsigned char BTN_state)
{
	if(timecount - old_timecount < 1000)//default SpeedCtl : 1Hz
		return SpeedCtl;
	
	old_timecount = timecount;	 //update the old time
		
	switch(BTN_state){
		case 0x06:	               //press the up button
        if(SpeedCtl > MaxSpeed)
          SpeedCtl -= 1000;
			break;
		case 0x05:	               //press the down button
        if(SpeedCtl < MinSpeed)
          SpeedCtl += 1000;
			break;
		case 0x03:	               //press the center button
        SpeedCtl = 0;          //pause the LED
			break;
		default:
			SpeedCtl = SpeedCtl;     //make it won't change the speed
			break;	
		}//switch	
	return SpeedCtl;
}

/* LED Toggle */
void LED_showing(uint32_t SpeedCtl)
{
		static uint32_t old_timecount = 0;
		static unsigned char f=0x01;
		if(timecount-old_timecount >= SpeedCtl){ //checks if enough time has passed since the last update exceeding the SpeedCtl value.
			f^=0x01;                               //xor operation
			PH6=(f)?1:0;                           //if (f is false),then PH6=0, LEDR1 ON
			PH7=(f)?0:1;                           //if (f is true),then PH7=0, LEDG1 ON
			old_timecount = timecount;             //update old time
		}  	
}