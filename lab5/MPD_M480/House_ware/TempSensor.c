#include "TempSensor.h"

void Temp_Sensor_Enable(void) 
{
	SYS->IVSCTL &= ~0x1ul << SYS_IVSCTL_VTEMPEN_Pos;
	SYS->IVSCTL |= SYS_IVSCTL_VTEMPEN_Msk;
}
