#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#ifdef _TIMER_C
   #define SCOPE
#else
   #define SCOPE extern
#endif

#include "stm32l4xx_hal.h"

#define NUMBER_OF_TIMERS			5

#define SSEG_TIMER				0
#define KEY_WAIT_REPEAT_TIMER		1
#define KEY_REPEAT_TIMER			2
#define SNAKE_TICK_TIMER			3
#define IR_RECIEVE_TIMER			4

#define SNAKE_TICK_TIME			100
#define SSEG_TIME				1
#define KEY_WAIT_REPEAT_TIME		1000
#define KEY_REPEAT_TIME		    333
#define IR_RECIEVE_TIME			1

SCOPE TIM_HandleTypeDef htim2;
SCOPE TIM_HandleTypeDef htim5;

SCOPE unsigned short sTimer[NUMBER_OF_TIMERS];

SCOPE void TIMER2_HANDLE(void);

SCOPE void Delay_1_plus_us();

#undef SCOPE
#endif /* INC_TIMER_H_ */
