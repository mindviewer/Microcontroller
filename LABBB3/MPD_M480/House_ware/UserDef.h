#ifndef __USER_DEF_H
#define __USER_DEF_H

#include "M480.h"

#define	PORT_A					  0
#define	PORT_B					  1
#define	PORT_C					  2
#define	PORT_D					  3
#define	PORT_E					  4
#define	PORT_F					  5
#define	PORT_G					  6
#define	PORT_H					  7
						
#define	PIN_00						0
#define	PIN_01						1
#define	PIN_02						2
#define	PIN_03						3
#define	PIN_04						4
#define	PIN_05						5
#define	PIN_06						6
#define	PIN_07						7
#define	PIN_08						8
#define	PIN_09						9
#define	PIN_10						10
#define	PIN_11						11
#define	PIN_12						12
#define	PIN_13						13
#define	PIN_14						14
#define	PIN_15						15

// Definnition of WRITE_PIN_VALUE
#define SET_HIGH              							0x1
#define SET_LOW               							0x0

#define LED_ON			0x0
#define LED_OFF     0x1

/*#define GPIO_PIN_DATA(port, pin)    (*((volatile uint32_t *)((GPIO_PIN_DATA_BASE+(0x40*(port))) + ((pin)<<2))))*/

typedef enum {false = 0, true = !false} bool;

void GPIO_SetMODE (uint32_t GPIO_Px, uint32_t GPIO_Pin, uint32_t u32Mode) ;
bool GPIO_ReadInput_Bit(uint32_t GPIO_Px, uint32_t GPIO_Pin) ;
void GPIO_WriteOutput_Bit(uint32_t GPIO_Px, uint32_t GPIO_Pin, uint32_t Level) ;


#endif 
