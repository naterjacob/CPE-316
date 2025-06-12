#ifndef _BTN_H
#define _BTN_H

#include <stdbool.h>
#include "stm32l4xx_hal.h"

#define BUTTON_PRESSED 0

#define EVENT_BTN_PRESS 1
#define EVENT_BTN_RELEASE 2

bool btn_is_pressed();
unsigned short btn_debounced();

#endif
