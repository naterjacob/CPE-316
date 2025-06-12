#ifndef _NEC_H
#define _NEC_H

#ifdef _NEC_C
	#define SCOPE_NEC
#else
	#define SCOPE_NEC extern
#endif

#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"

#define txGPIO GPIOB
#define txPIN GPIO_PIN_9

//#define rxGPIO GPIOB
//#define rxPIN GPIO_PIN_12

SCOPE_NEC bool isTransmitting;

void NECSendStart();
void NECSendByte(uint8_t);
void NECSendEnd();
void NECStartPulse();
void NECStopPulse();

#undef SCOPE_NEC
#endif
