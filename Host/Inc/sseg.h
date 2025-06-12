#ifndef SSEG_H
#define SSEG_H

#include "main.h"  // for HAL_GPIO_WritePin and GPIO definitions

// ---------------- Pin Definitions ----------------
#define PIN0   0x0001
#define PIN1   0x0002
#define PIN2   0x0004
#define PIN3   0x0008
#define PIN4   0x0010
#define PIN5   0x0020
#define PIN6   0x0040
#define PIN7   0x0080
#define PIN8   0x0100
#define PIN9   0x0200
#define PIN10  0x0400

// Common cathode digit control pins
#define CCdigit1 GPIO_PIN_6  // Port B
#define CCdigit2 GPIO_PIN_7  // Port A
#define CCdigit3 GPIO_PIN_6  // Port A
#define CCdigit4 GPIO_PIN_5  // Port A

// Segment bitmasks for each digit [0-9]
// Format: [PA_bits, PB_bits, PC_bits]

// GPIO masks for each port
#define PA_MASK ((1 << 8) | (1 << 9))
#define PB_MASK ((1 << 3) | (1 << 4) | (1 << 5) | (1 << 10))
#define PC_MASK (1 << 7)

// ---------------- Function Prototypes ----------------
void sseg_clear(void);
void sseg_write(int score);

#endif // SSEG_H
