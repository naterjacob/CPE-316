#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#ifdef _TIMER_C
   #define SCOPE
#else
   #define SCOPE extern
#endif

#include <stdbool.h>
#include "stm32l4xx_hal.h"

#define NUM_TIM2_TIMERS 6
#define BTN				0
#define NEC_START_ON 	1
#define NEC_START_OFF 	2
#define NEC_BIT 		3
#define KEYPAD			4
#define UART			5

#define BTN_POLL_CNT 		18 	// 10ms
#define NEC_START_ON_CNT 	16 	// 9ms
#define NEC_START_OFF_CNT 	8 	// 45ms
#define NEC_BIT_CNT 		1 	// 562.5us
#define KEY_POLL_CNT		18	// 10ms
#define UART_POLL_CNT		18

SCOPE unsigned short TIM2_timers[NUM_TIM2_TIMERS];

SCOPE void TIM2_HANDLE(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#undef SCOPE
#endif /* INC_TIMER_H_ */
