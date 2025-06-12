#ifndef SNAKE_H
#define SNAKE_H

#include "main.h"
#include <stdbool.h>
// Initializes the game board with walls and empty spaces
void init_game_board(char GAME_BOARD[][WIDTH + 1]);

// Updates the snake's body based on current direction
int update_snake(struct Point snake_body[], enum Direction direction, int* length, struct Point food, bool* FoodReadyFlag);

// Draws the snake onto the game board
//void update_game_board(char GAME_BOARD[][WIDTH + 1], struct Point snake_body[], int length);


#endif // SNAKE_H
