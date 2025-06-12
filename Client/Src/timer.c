#define _TIMER_C

#include "timer.h"
#include "nec.h"

void TIM2_HANDLE(void)
{
	unsigned short sIndex;
	//__disable_irq();

	for (sIndex=0; sIndex<NUM_TIM2_TIMERS; sIndex++)
	{
		if (TIM2_timers[sIndex] != 0)
			TIM2_timers[sIndex]--;
	}
	//__enable_irq();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	TIM_TypeDef *instance = htim->Instance;
	if (instance == TIM2) {
		TIM2_HANDLE();
	} else if (instance == TIM5 && isTransmitting) {
		HAL_GPIO_TogglePin(txGPIO,  txPIN);
	}
}
