#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "snake.h"
#include "main.h"


void init_game_board(char GAME_BOARD[][WIDTH + 1])
{
    // Initialize all cells to space first
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            GAME_BOARD[i][j] = ' ';
        }
        GAME_BOARD[i][WIDTH] = '\0'; // Null terminate each row
    }

    // Create borders - top and bottom walls
    for (int j = 0; j < WIDTH; j++) {
        GAME_BOARD[0][j] = '#';           // Top border
        GAME_BOARD[HEIGHT-1][j] = '#';    // Bottom border
    }

    // Create left and right walls
    for (int i = 0; i < HEIGHT; i++) {
        GAME_BOARD[i][0] = '#';
        GAME_BOARD[i][1] = '#';
        GAME_BOARD[i][WIDTH-1] = '#';     // Right border
        GAME_BOARD[i][WIDTH-2] = '#';
    }
}

int update_snake(struct Point snake_body[], enum Direction direction, int* snake_length, struct Point food, bool* FoodReadyFlag)
{
    // Move body segments - each segment takes position of the one in front
	struct Point old_tail = snake_body[(*snake_length) - 1];
    for (int i = *snake_length - 1; i > 0; i--) {
        snake_body[i] = snake_body[i - 1];
    }

    // Move head based on direction
    switch (direction) {
        case UP:    snake_body[0].y--; break;
        case DOWN:  snake_body[0].y++; break;
        case LEFT:  snake_body[0].x--; break;
        case RIGHT: snake_body[0].x++; break;
    }

    if (snake_body[0].x == food.x && snake_body[0].y == food.y) {
    	snake_body[(*snake_length)] = old_tail;
    	(*snake_length)++;
    	(*FoodReadyFlag) = true;
    }

    for (int i = 1; i < (*snake_length) - 1; i++) {
    	if (snake_body[0].x == snake_body[i].x && snake_body[0].y == snake_body[i].y) return -1;
    }
    // Basic collision detection with walls
    if (snake_body[0].x <= 0) return -1;
    if (snake_body[0].x > (WIDTH / 2) - 2) return -1;
    if (snake_body[0].y < 0) return -1;
    if (snake_body[0].y >= HEIGHT - 1) return -1;



    return 0;
}

//void update_game_board(char GAME_BOARD[][WIDTH + 1], struct Point snake_body[], int length)
//{
//    // Clear previous snake positions (except borders)
//    for (int i = 1; i < HEIGHT-1; i++) {
//        for (int j = 1; j < WIDTH-1; j++) {
//            if (GAME_BOARD[i][j] == 'O' || GAME_BOARD[i][j] == 'H') {
//                GAME_BOARD[i][j] = ' ';
//            }
//        }
//    }
//
//    // Place snake body
//    for (int i = 1; i < length; i++) {
//        if (snake_body[i].y >= 0 && snake_body[i].y < HEIGHT &&
//            snake_body[i].x >= 0 && snake_body[i].x < WIDTH) {
//            GAME_BOARD[snake_body[i].y][snake_body[i].x] = 'O';
//        }
//    }
//
//    // Place snake head
//    if (snake_body[0].y >= 0 && snake_body[0].y < HEIGHT &&
//        snake_body[0].x >= 0 && snake_body[0].x < WIDTH) {
//        GAME_BOARD[snake_body[0].y][snake_body[0].x] = 'H';
//    }
//}


