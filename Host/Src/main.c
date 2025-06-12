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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "snake.h"
#include "TIMER.h"
#include "NEC.h"
#include "screens.h"
#include "sseg.h"
#include "eeprom.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define LEADERBOARD_POSITIONS 5

enum ArcadeState {
	TITLE_SCREEN,
	SNAKE,
	GAME_OVER,
	ENTER_NAME,
};

typedef struct highscore{
	int score;
	char name[4];
} highscore;

highscore leaderboard[LEADERBOARD_POSITIONS] = {0};

const char CLEAR_SCREEN[] = { 0x1B, '[', '2', 'J', 0 };  // Clear the screen
const char CURSOR_HOME[] = { 0x1B, '[', 'H', 0 };        // Move cursor home

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */
void PrintScreen(char screen[][WIDTH + 1]);
void ClearScreen(void);
void MoveHome(void);
void Keypadscan(void);
void KeyProcess(void);
void copy_snake_positions(struct Point dest[], struct Point[], int length);
void update_screen_selective(struct Point[], struct Point [], int, int);
void move_and_draw(int, int, char*, int);
bool is_occupied(struct Point snake_body[], int, int, int);
void move_cursor_to(int, int);
int set_leaderboard(highscore[], int);
void print_leaderboard(highscore[]);
void RENDER_HAN_DO_IT(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// GPIO interrupt handler
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    NEC_Decode();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	TIMER2_HANDLE();
}

enum Direction snake_direction = RIGHT;
enum ArcadeState ArcadeState = TITLE_SCREEN;
bool FirstRenderFlag = true;
char command;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start(&htim5);

  NEC_Init();
  char GAME_BOARD[HEIGHT][WIDTH + 1];
  int snake_length = 5;
  int prev_snake_length;
  int score = 0;
  int place = 0;
  int temporary_char_index = 0;
  struct Point snake_body[50];
  struct Point prev_snake_body[50];
  struct Point food;
  bool FoodReadyFlag = true;
  bool DataFlag = false;

  // Hide cursor
  HAL_UART_Transmit(&huart2, (uint8_t *)"\x1B[?25l", strlen("\x1B[?25l"), HAL_MAX_DELAY);

  // TEST -----------------------------------------------------------------------------------
//  uint16_t testIn = 255;
//  Save_EEPROM((uint8_t *) &testIn, 2, EEPROM_USER);
//  char testIn1 = 'a';
//  char testIn2 = 'z';
//  EEPROM_SEND(EEPROM_USER, testIn1, testIn2);
//
//  char testOut[3] = {0};
//  EEPROM_GET_MEM(EEPROM_USER, 2, (uint8_t *) testOut);
//  char message[32] = {0};
////  snprintf(message, 32, "testOut: %hu\r\n", (uint16_t) testOut[0]);
//  snprintf(message, 32, "testOut: %s\r\n", testOut);
//  HAL_UART_Transmit(&huart2, (uint8_t *) message, strlen(message), HAL_MAX_DELAY);
//  return 0;
  // END TEST -------------------------------------------------------------------------------

  // uncomment to clear leaderboard
//  EraseLeaderboard(LEADERBOARD_POSITIONS);
//  return 0;

  // load leaderboard
  uint8_t EEPROM_BUF[40] = {0};
  EEPROM_GET_MEM(EEPROM_USER, 40, EEPROM_BUF);

  for (int i = 0; i < LEADERBOARD_POSITIONS; i++) {
	  memcpy(&leaderboard[i].score, &EEPROM_BUF[8 * i], 4);
	  memcpy(leaderboard[i].name, &EEPROM_BUF[8 * i + 4], 4);

  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  // Write score to seven segment
	  if (sTimer[SSEG_TIMER] == 0) {
		  sseg_write(score);
		  sTimer[SSEG_TIMER] = SSEG_TIME;
	  }

	  switch (ArcadeState) {

	  	  case TITLE_SCREEN:
	  		  if (FirstRenderFlag) {
	  			  ClearScreen();
	  			  MoveHome();
	  			  PrintScreen(TITLE);
	  			  FirstRenderFlag = false;


	  		  } else {
	  			  if (NEC_Frame_Available()) {
	  			      command = (char) NEC_Get_Command();
	  			      FirstRenderFlag = true;
	  				  ArcadeState = SNAKE;
	  			  }
	  		  }
	  		  break;

	  	  case SNAKE:
	  		  if (FirstRenderFlag) {
	  			  // Init game variables
	  			  snake_direction = RIGHT;
	  			  snake_length = 8;

	  			  for (int i = 0; i < snake_length; i++) {
	  				  snake_body[i].x = 25;
	  				  snake_body[i].y = 25;
	  			  }

	  			  // Init creates an empty board
	  			  init_game_board(GAME_BOARD);
	  			  MoveHome();
	  			  ClearScreen();
	  			  PrintScreen(GAME_BOARD);
	  			  FoodReadyFlag = true;
	  			  FirstRenderFlag = false;

	  		  } else {

	  			// Get NEC command
	  			if (NEC_Frame_Available()) {
	  			        command = (char) NEC_Get_Command();
	  			        DataFlag = true;
	  			}

	  			if (sTimer[SNAKE_TICK_TIMER] == 0) {
	  				score++;

	  				if(DataFlag) {
	  					switch (command) {
	  						case '2':
	  						case 'w':
	  							snake_direction = UP;
	  							break;
	  						case '4':
	  						case 'a':
	  							snake_direction = LEFT;
	  							break;
	  						case '8':
	  						case 's':
	  							snake_direction = DOWN;
	  							break;
	  						case '6':
	  						case 'd':
	  							snake_direction = RIGHT;
	  							break;
	  						default:
	  							break;
	  					 }

	  					DataFlag = false;
	  				}

	  				MoveHome();
	  				if(rand() % 10 == 0 && FoodReadyFlag) {
	  					// spawn food
	  					int x = rand() % 39 + 1;
	  					int y = rand() % 40;

	  					if (!is_occupied(snake_body, snake_length, x, y)) {
	  						move_and_draw(x, y, "()", 2);
		  					food.x = x;
		  					food.y = y;
		  					MoveHome();
		  					FoodReadyFlag = false;
	  					  }
	  				  }

	  				copy_snake_positions(prev_snake_body, snake_body, snake_length);
	  				prev_snake_length = snake_length;

	  				if (update_snake(snake_body, snake_direction, &snake_length, food, &FoodReadyFlag) == -1) {
	  					if (score == 69) {
	  						ClearScreen();
	  						MoveHome();
	  						RENDER_HAN_DO_IT();
	  						return 0;
	  					}
	  					FirstRenderFlag = true;
	  					if ((place = set_leaderboard(leaderboard, score)) == -1) {
		  					ArcadeState = GAME_OVER;
	  					} else {
	  						ArcadeState = ENTER_NAME;
	  					}
	  				}

	  				// Score increases if we ate food!
	  				if (snake_length != prev_snake_length) score += 50;
	  				update_screen_selective(snake_body, prev_snake_body, snake_length, prev_snake_length);

	  				sTimer[SNAKE_TICK_TIMER] = SNAKE_TICK_TIME;
	  			}

	  		  }

	  		  break;

	  	  case ENTER_NAME:
	  		  // These values will persist from previous state
	  		  if (FirstRenderFlag){
	  			  leaderboard[place].score = score;
	  			  MoveHome();
	  			  ClearScreen();
	  			  PrintScreen(LEADERBOARD_PROMPT);
	  			  MoveHome();
	  			  FirstRenderFlag = false;
	  			  temporary_char_index = 0;
	  		  } else {
	  			  if (NEC_Frame_Available()) {
	  				  command = (char) NEC_Get_Command();
		  			  DataFlag = true;
		  			  if (temporary_char_index < 2) {
		  				  leaderboard[place].name[temporary_char_index] = command;
		  				  move_and_draw(31 + temporary_char_index, 18, &command, 1);
		  				  MoveHome();
		  			      temporary_char_index++;
		  			      } else {
		  			    	  leaderboard[place].name[temporary_char_index] = command;
		  			    	  leaderboard[place].name[3] = '\0';
		  			    	  ArcadeState = GAME_OVER;
		  			    	  FirstRenderFlag = true;

		  			    	  //eepwom
		  			    	  for (int i = 0; i < LEADERBOARD_POSITIONS; i++) {
		  			    		  write_score(leaderboard[i].score, leaderboard[i].name, i);
		  			    	  }

		  			      }
		  			}
	  		  }

	  		  break;

	  	  case GAME_OVER:
	  		  if (FirstRenderFlag) {
	  			  ClearScreen();
	  			  MoveHome();
	  			  PrintScreen(GAME_OVER_SCREEN);
	  			  MoveHome();
	  			  print_leaderboard(leaderboard);
	  			  FirstRenderFlag = false;
	  		  } else {
	  			  // No uses right now
		  			if (NEC_Frame_Available()) {
		  			        command = (char) NEC_Get_Command();
		  			        DataFlag = true;
		  			        ArcadeState = TITLE_SCREEN;
		  			        FirstRenderFlag = true;
		  			        score = 0;
		  			}

	  		  }
	  		  break;

	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
}
void RENDER_HAN_DO_IT() {
	for (int i = 0; i < 128; i++) {
	    HAL_UART_Transmit(&huart2, (uint8_t *)ART[i], strlen(ART[i]), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t *)"\n\r", 2, HAL_MAX_DELAY);  // new line
	}
}

int set_leaderboard(highscore leaderboard[], int score) {
	for (int i = 0; i < LEADERBOARD_POSITIONS; i++) {
		if (score > leaderboard[i].score) {
			for (int j = LEADERBOARD_POSITIONS - 1; j > i; j--) {

				leaderboard[j] = leaderboard[j - 1];
			}
			leaderboard[i].score = score;
			return i;
		}
	}
	return -1;
}

void print_leaderboard(highscore leaderboard[]) {
	char score_string[10] = {0};
	for (int i = 0; i < LEADERBOARD_POSITIONS; i++) {
		// row, co4
		move_cursor_to(20 + i, 35);
		itoa(leaderboard[i].score, score_string, 10);
	    HAL_UART_Transmit(&huart2, (uint8_t *)score_string, strlen(score_string), HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart2, (uint8_t *)"   ", 3, HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart2, (uint8_t *)leaderboard[i].name, strlen(leaderboard[i].name), HAL_MAX_DELAY);
	    MoveHome();
	}
}

bool is_occupied(struct Point snake_body[], int snake_length, int x, int y) {
	  for (int i = 0; i < (snake_length - 1); i++) {
		  if (x == snake_body[i].x && y == snake_body[i].y) {
			  return true;
		  }
	  }
	  return false;
}

void update_screen_selective(struct Point snake_body[],
		struct Point prev_snake_body[],
		int snake_length,
		int prev_snake_length)
{

    move_and_draw(snake_body[0].x, snake_body[0].y, "{}", 2);
    MoveHome();

    move_and_draw(prev_snake_body[0].x, prev_snake_body[0].y, "[]", 2);
    MoveHome();

    if (snake_length == prev_snake_length) {
        move_and_draw(prev_snake_body[prev_snake_length - 1].x, prev_snake_body[prev_snake_length - 1].y, "  ", 2);
    }
}

void move_and_draw(int col, int row, char* c, int cnt) {
    char cursor_cmd[40];
    sprintf(cursor_cmd, "\x1B[%d;%dH", row + 1, ((2 * col) + 1));
    HAL_UART_Transmit(&huart2, (uint8_t *)cursor_cmd, strlen(cursor_cmd), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)c, cnt, HAL_MAX_DELAY);
}



void copy_snake_positions(struct Point dest[], struct Point src[], int length)
{
    for (int i = 0; i < length; i++) {
        dest[i] = src[i];
    }
}

void PrintScreen(char screen[][WIDTH + 1]) {
	for (int i = 0; i < HEIGHT; i++) {
		HAL_UART_Transmit(&huart2, (uint8_t *)screen[i], strlen(screen[i]), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t *)"\n\r", 2, HAL_MAX_DELAY);  // new line
	}
}

void ClearScreen() {
    HAL_UART_Transmit(&huart2, (uint8_t *)CLEAR_SCREEN, strlen(CLEAR_SCREEN), HAL_MAX_DELAY);
}

void MoveHome()
{
	HAL_UART_Transmit(&huart2, (uint8_t *)CURSOR_HOME, strlen(CURSOR_HOME), HAL_MAX_DELAY);
}

void move_cursor_to(int row, int col)
{
    // ANSI escape sequence to move cursor to specific position
    // Format: ESC[row;colH (1-based indexing)
    char cursor_cmd[20] = {0};
    snprintf(cursor_cmd, 20, "\x1B[%hu;%huH", row + 1, col + 1);
    HAL_UART_Transmit(&huart2, (uint8_t *)cursor_cmd, strlen(cursor_cmd), HAL_MAX_DELAY);
}


  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 3999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim2.Init.Period = 19;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 79;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5
                           PA6 PA7 PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB3 PB4 PB5
                           PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
