#ifndef __ADCAGENT_H__
#define __ADCAGENT_H__
#include "NuMicro.h"

typedef unsigned short u16;

void ADC_Initial(void);
void ADC_Task(void);
uint32_t ADC_GetVR(void);
float ADC_GetM487Temperature(void);
u16 ADC_ConvThermistorTempToReal(void) ;

#endif  /* __ADCAGENT_H__ */
