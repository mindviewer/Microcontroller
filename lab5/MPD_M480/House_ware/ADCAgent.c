#include "ADCAgent.h"

static uint8_t ADCState;
static uint16_t TempBuf;
static uint16_t VRBuf;
extern uint32_t timecount;

void ADC_Initial(void)
{
	/* Enable EADC module clock */
		CLK_EnableModuleClock(EADC_MODULE);

	/* EADC clock source is 96MHz, set divider to 8, EADC clock is 96/8 MHz */
		CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(8));

	/* Set input mode as single-end and enable the A/D converter */
		EADC_Open(EADC, EADC_CTL_DIFFEN_SINGLE_END);	

	/* Set sample module 17 and 0 external sampling time to 0x3F */
		EADC_SetExtendSampleTime(EADC, 17, 0x3F);
		EADC_SetExtendSampleTime(EADC,  0, 0x3F);
	
	/* Configure EADC0 sample module 0 for analog input channel 14 and enable software trigger source */
		EADC_ConfigSampleModule(EADC, 0, EADC_SOFTWARE_TRIGGER, 14);
	
	/* GPIO Setting */
	/* Set GPIO PB14 to input mode */
		GPIO_SetMode(PB, BIT14, GPIO_MODE_INPUT);
	
	/* Configure the GPIO PB14 EADC0 analog input pins. */
		SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB14MFP_Msk);
		SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB14MFP_EADC0_CH14);
	
	/* Disable the GPIO PB14 digital input path to avoid the leakage current. */
		GPIO_DISABLE_DIGITAL_PATH(PB, BIT14);
	
		ADCState = 0;	
}

void ADC_Task(void)
{
	static uint32_t ADCOldCount = 0;
	
	if((uint32_t)(timecount - ADCOldCount) < 1000)
	{	
		return;
	}
	
	ADCOldCount = timecount;
	
	switch(ADCState)
	{
		/* Start conversion, Software trigger temperature and VR channel */
		case 0:
			EADC_START_CONV(EADC, BIT17);
			EADC_START_CONV(EADC, BIT0);
			ADCState = 1;
			break;		
			/* wait for all conversion done */		
		case 1:
			if(EADC->SWTRG == 0)
			{
				ADCState = 2;
				break;
			}
			/* Get data */
			case 2:		
				TempBuf = EADC_GET_CONV_DATA(EADC, 17);
				VRBuf = EADC_GET_CONV_DATA(EADC, 0);
				ADCState = 0;		
				break;
			default:
            ADCState = 0;	
	}
}

float ADC_GetM487Temperature(void)
{
	/* The equation of converting to real temperature is as below
	 * (25+(((float)i32ConversionData/4095*3300)-675)/(-1.83)), 3300 means ADCVREF=3.3V
	 * If ADCREF set to 1.6V, the equation should be updated as below
	 * (25+(((float)i32ConversionData/4095*1600)-675)/(-1.83)), 1600 means ADCVREF=1.6V
	 */
	return (25+(((float)TempBuf/4095*3300)-675)/(-1.83));
}

uint32_t ADC_GetVR(void) 
{
	return VRBuf >> 4;
}

u16 ADC_ConvThermistorTempToReal(void) 
{
	return (3340 + ((1480 - (int)VRBuf)*6*100/270))/80 ;
}
