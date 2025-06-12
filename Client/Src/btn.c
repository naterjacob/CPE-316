#include "btn.h"

typedef enum {
	RELEASED, R2P, PRESSED, P2R
} button_state_t;

bool btn_is_pressed() {
	return (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == BUTTON_PRESSED);
}

unsigned short btn_debounced() { // FSM
	static button_state_t state = RELEASED;

	unsigned short event = 0;
	if (state == R2P && btn_is_pressed()) {
		event = EVENT_BTN_PRESS;
	} else if (state == P2R && !btn_is_pressed()) {
		event = EVENT_BTN_RELEASE;
	}

	switch (state) {
		case RELEASED:
			state = (btn_is_pressed() ? R2P : RELEASED);
			break;
		case R2P:
			state = (btn_is_pressed() ? PRESSED : RELEASED);
			break;
		case PRESSED:
			state = (!btn_is_pressed() ? P2R : PRESSED);
			break;
		case P2R:
			state = (!btn_is_pressed() ? RELEASED : PRESSED);
			break;
		default:
			state = RELEASED;
			break;
	}

	return event;
}
