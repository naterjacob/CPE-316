#define _TIMER_C

#include "TIMER.h"

void TIMER2_HANDLE(void)
{
	unsigned short sIndex;
	//__disable_irq();

	for (sIndex=0; sIndex<NUMBER_OF_TIMERS; sIndex++)
	{
		if (sTimer[sIndex] != 0)
			sTimer[sIndex]--;
	}
	//__enable_irq();
}

void Delay_1_plus_us()
{
    uint32_t time5now = htim5.Instance->CNT;
    while (htim5.Instance->CNT == time5now)
        ; ;
    time5now = htim5.Instance->CNT;

    while (htim5.Instance->CNT == time5now)
        ; ;
}
