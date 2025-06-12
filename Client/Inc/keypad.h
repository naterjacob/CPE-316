#ifndef _KEYPAD_H
#define _KEYPAD_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define Number_of_Keys 12
#define Number_of_Cols  3
#define PA0 0x0001
#define PA1 0x0002
#define PA4 0x0010
#define PB0 0x0001
#define PC1 0x0002
#define PC0 0x0001
#define PA10 0x0400

#define KeyLow2High 		0x0001

char scanKeypad();
char processKey();

#endif
