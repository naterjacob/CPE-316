#define _UART_C

#include "uart.h"
#include "timer.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartRX = true; // set the receiving flag
	HAL_UART_Receive_IT(huart, (uint8_t *) uartBuf, 1); // reenable UART interrupts
}
