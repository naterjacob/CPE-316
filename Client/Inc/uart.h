#ifndef _UART_H
#define _UART_H

#ifdef _UART_C
	#define SCOPE_UART
#else
	#define SCOPE_UART extern
#endif

#include <stdbool.h>
#include "stm32l4xx_hal.h"

#define UART_RX 0x01
#define DONE 0x02

SCOPE_UART bool uartRX;
SCOPE_UART char uartBuf[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#undef SCOPE_UART
#endif
