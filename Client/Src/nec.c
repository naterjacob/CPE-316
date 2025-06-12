#define _NEC_C

#include "nec.h"
#include "timer.h"

void NECSendStart() {
	// 9ms pulse followed by 4.5ms of 0
	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_SET);
	TIM2_timers[NEC_START_ON] = NEC_START_ON_CNT;
	isTransmitting = true;
	while(TIM2_timers[NEC_START_ON] != 0) {
		; // send pulse
	}
	isTransmitting = false;

	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_RESET);
	TIM2_timers[NEC_START_OFF] = NEC_START_OFF_CNT;
	while(TIM2_timers[NEC_START_OFF] != 0) {
		; // wait
	}

	// uncomment to see end of start sequence for testing
//	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_SET);
//	HAL_Delay(1);
//	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_RESET);
}

void NECSendEnd() {
	// one 562.5us pulse
	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_SET);
	TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
	isTransmitting = true;
	while(TIM2_timers[NEC_BIT] != 0) {
		; // send pulse
	}
	isTransmitting = false;

	HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_RESET);
	TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
	while(TIM2_timers[NEC_BIT] != 0) {
		; //wait
	}
}

void NECSendByte(uint8_t byte) {
	NECSendStart();

	// LSB first;
	// binary 0 is a 562.5us pulse of a 38kHz square wave followed by 562.5us off; total time = 1.125ms
	// binary 1 is a 562.5us pulse of a 38kHz square wave followed by 1.6875ms off; total time = 2.25ms
	for (int i = 0; i < 8; i++) { // for each bit
		HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_SET);
		TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
		isTransmitting = true;
		while(TIM2_timers[NEC_BIT] != 0) {
			; // send pulse
		}
		isTransmitting = false;

		HAL_GPIO_WritePin(txGPIO, txPIN, GPIO_PIN_RESET);
		TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
		while(TIM2_timers[NEC_BIT] != 0) {
			; //wait
		}

		if (byte & 0x01) {
			TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
			while(TIM2_timers[NEC_BIT] != 0) {
				; //wait
			}

			TIM2_timers[NEC_BIT] = NEC_BIT_CNT;
			while(TIM2_timers[NEC_BIT] != 0) {
				; //wait
			}
		}

		byte = byte >> 1;
	}

	NECSendEnd();
}

void NECStartPulse() {
	isTransmitting = true;
}

void NECStopPulse() {
	isTransmitting = false;
}
