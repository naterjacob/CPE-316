/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "sseg.h"

/* Segment A - PB3
 * Segment B - PB5
 * Segment C - PB4
 * Segment D - PB10
 * Segment E - PA8
 * Segment F - PA9
 * Segment G - PC7
 */

uint16_t LED_Port_Map[10][3]={
		{(PIN8|PIN9), (PIN3|PIN4|PIN5|PIN10), 0},	// 0, PA(8|9), PB(3|4|5|10), PC=none
		{0, (PIN4|PIN5), 0},						// 1, PA=none, PB (4|5), PC= none
		{PIN8, (PIN3|PIN5|PIN10), PIN7},			// 2, PA8, PB(3|5|10), PC7
		{0, (PIN3|PIN4|PIN5|PIN10), PIN7},			// 3, PA=none, PB(3|4|5|10), PIN7)
		{PIN9, (PIN4|PIN5), PIN7},					// 4, PA9, PB(4|5), PC7
		{PIN9, (PIN3|PIN4|PIN10), PIN7},			// 5, PA9, PB(3|4|10), PC7
		{(PIN8|PIN9), (PIN3|PIN4|PIN10), PIN7},		// 6, PA(8|9), PB(3|4|10), PC7
		{0, (PIN3|PIN4|PIN5), 0},					// 7, PA=none, PB(3|4|5), PC=none
		{(PIN8|PIN9), (PIN3|PIN4|PIN5|PIN10), PIN7},// 8, PA(8|9), PB(3|4|5|10), PC7
		{PIN9, (PIN3|PIN4|PIN5|PIN10), PIN7}		// 9, PA9, PB(3|4|5|10), PC7
};

void sseg_clear() {
  HAL_GPIO_WritePin(GPIOB, CCdigit1, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, CCdigit2, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, CCdigit3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, CCdigit4, GPIO_PIN_RESET);
}

void sseg_write(int score) {
	uint16_t IDR_temp;

	static int digit = -1;
	digit = (digit + 1) % 4;

	int First = score / 1000;
	int Rem = score % 1000;

	int Second = Rem / 100;
	Rem = Rem % 100;

	int Third = Rem / 10;
	int Fourth = Rem % 10;

		sseg_clear();

		 switch (digit)
		 {
		 	  case 0:
		 	  {
		 		  IDR_temp = (GPIOA->IDR & ~PA_MASK);				// read it back first
		 		  GPIOA->ODR = (LED_Port_Map[First][0] | IDR_temp);	// set what should be on, on PA
		  		  IDR_temp = (GPIOB->IDR & ~PB_MASK);
		  		  GPIOB->ODR = (LED_Port_Map[First][1] | IDR_temp);
		  		  IDR_temp = (GPIOC->IDR & ~PC_MASK);
		  		  GPIOC->ODR = (LED_Port_Map[First][2] | IDR_temp);
		  		  HAL_GPIO_WritePin(GPIOB, CCdigit1, GPIO_PIN_SET);
		  		  break;
		  	  }

		  	  case 1:
		   	  {
		  		  IDR_temp = (GPIOA->IDR & ~PA_MASK);				// read it back first
		  		  GPIOA->ODR = (LED_Port_Map[Second][0] | IDR_temp);// set what should be on, on PA
		  		  IDR_temp = (GPIOB->IDR & ~PB_MASK);
		  		  GPIOB->ODR = (LED_Port_Map[Second][1] | IDR_temp);
		  		  IDR_temp = (GPIOC->IDR & ~PC_MASK);
		  		  GPIOC->ODR = (LED_Port_Map[Second][2] | IDR_temp);
		  		  HAL_GPIO_WritePin(GPIOA, CCdigit2, GPIO_PIN_SET);
		  		  break;
		   	  }

		  	  case 2:
			  {
		  		  IDR_temp = (GPIOA->IDR & ~PA_MASK);
		  		  GPIOA->ODR = (LED_Port_Map[Third][0] | IDR_temp);
		  		  IDR_temp = (GPIOB->IDR & ~PB_MASK);
		  		  GPIOB->ODR = (LED_Port_Map[Third][1] | IDR_temp);
		  		  IDR_temp = (GPIOC->IDR & ~PC_MASK);
		  		  GPIOC->ODR = (LED_Port_Map[Third][2] | IDR_temp);
				  HAL_GPIO_WritePin(GPIOA, CCdigit3, GPIO_PIN_SET);
				  break;
			  }

		  	  case 3:
			  {
		  		  IDR_temp = (GPIOA->IDR & ~PA_MASK);
		  		  GPIOA->ODR = (LED_Port_Map[Fourth][0] | IDR_temp);
		  		  IDR_temp = (GPIOB->IDR & ~PB_MASK);
		  		  GPIOB->ODR = (LED_Port_Map[Fourth][1] | IDR_temp);
		  		  IDR_temp = (GPIOC->IDR & ~PC_MASK);
		  		  GPIOC->ODR = (LED_Port_Map[Fourth][2] | IDR_temp);
				  HAL_GPIO_WritePin(GPIOA, CCdigit4, GPIO_PIN_SET);
				  break;
		   	  }

		  	  default:

	  	  }
}
