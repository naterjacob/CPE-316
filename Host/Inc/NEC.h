#ifndef NEC_DECODER_H
#define NEC_DECODER_H

#include <stdint.h>
#include <stdbool.h>
#include "TIMER.h"

// NEC protocol timing thresholds
#define NEC_START_BURST_MIN     8000
#define NEC_START_BURST_MAX     10000
#define NEC_START_SPACE_MIN     3500
#define NEC_START_SPACE_MAX     5500
#define NEC_BIT_BURST_MIN       400
#define NEC_BIT_BURST_MAX       750
#define NEC_BIT_0_SPACE_MIN     400
#define NEC_BIT_0_SPACE_MAX     750
#define NEC_BIT_1_SPACE_MIN     1500
#define NEC_BIT_1_SPACE_MAX     1900
#define NEC_REPEAT_SPACE_MIN    2000
#define NEC_REPEAT_SPACE_MAX    2500

#define NEC_START_TOTAL_MIN     12000
#define NEC_START_TOTAL_MAX     15000
#define NEC_BIT_0_TOTAL_MIN     800
#define NEC_BIT_0_TOTAL_MAX     1400
#define NEC_BIT_1_TOTAL_MIN     1800
#define NEC_BIT_1_TOTAL_MAX     2700
#define NEC_TIMEOUT_US          20000

// Decoder state enumeration
typedef enum {
    NEC_IDLE,
    NEC_START_BURST,
    NEC_BIT_BURST,
    NEC_FRAME_COMPLETE
} nec_state_t;

// NEC decoder structure
typedef struct {
    nec_state_t state;
    uint32_t last_time;
    uint32_t pulse_width;
    uint8_t command_data;
    uint8_t bit_count;
    bool frame_ready;
    uint8_t command;
} nec_decoder_t;

// Public API
void NEC_Init(void);
void NEC_Decode(void);
bool NEC_Frame_Available(void);
uint8_t NEC_Get_Command(void);
uint8_t NEC_Get_Address(void);
bool NEC_Is_Repeat(void);

#endif // NEC_DECODER_H
