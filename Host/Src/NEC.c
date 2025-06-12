#include "NEC.h"

static nec_decoder_t nec = {0};

// Initialize NEC decoder
void NEC_Init(void) {
    nec.state = NEC_IDLE;
    nec.frame_ready = false;
    nec.bit_count = 0;
    nec.command_data = 0;
}

// Get current time in microseconds
static uint32_t get_time_us(void) {
    return __HAL_TIM_GET_COUNTER(&htim5);
}

// Calculate pulse width handling timer overflow
static uint32_t calculate_pulse_width(uint32_t current_time, uint32_t last_time) {
    if (current_time >= last_time) {
        return current_time - last_time;
    } else {
        // Handle timer overflow
        return (0xFFFFFFFF - last_time) + current_time + 1;
    }
}

// Reset decoder state
static void reset_decoder(void) {
    nec.state = NEC_IDLE;
    nec.bit_count = 0;
    nec.command_data = 0;
    nec.frame_ready = false;
}

// Decode complete frame
static void decode_frame(void) {
    nec.command = nec.command_data;
    nec.frame_ready = true;
}

// Main NEC decoder function - call from GPIO interrupt (FALLING EDGE ONLY)
void NEC_Decode(void) {
    uint32_t current_time = get_time_us();

    // For the first interrupt, we don't have a previous time to compare
    if (nec.state == NEC_IDLE) {
        nec.state = NEC_START_BURST;
        nec.last_time = current_time;
        nec.bit_count = 0;
        nec.command_data = 0;
        return;
    }

    uint32_t time_since_last = calculate_pulse_width(current_time, nec.last_time);

    switch (nec.state) {
        case NEC_START_BURST:
            // Second falling edge - check if start sequence timing is correct
            if (time_since_last >= NEC_START_TOTAL_MIN &&
                time_since_last <= NEC_START_TOTAL_MAX) {
                nec.state = NEC_BIT_BURST;
                nec.bit_count = 0;
                nec.command_data = 0;
            } else {
                // Invalid start sequence

                reset_decoder();
                // Try to start over with this edge
                nec.state = NEC_START_BURST;
                nec.bit_count = 0;
                nec.command_data = 0;
            }
            break;

        case NEC_BIT_BURST:
            {
                uint8_t bit_value = 0;

                // Determine bit value based on time between falling edges
                if (time_since_last >= NEC_BIT_0_TOTAL_MIN &&
                    time_since_last <= NEC_BIT_0_TOTAL_MAX) {
                    bit_value = 0;
                } else if (time_since_last >= NEC_BIT_1_TOTAL_MIN &&
                          time_since_last <= NEC_BIT_1_TOTAL_MAX) {
                    bit_value = 1;
                } else {
                    // Invalid bit timing - reset and try to restart

                    reset_decoder();
                    nec.state = NEC_START_BURST;
                    nec.bit_count = 0;
                    nec.command_data = 0;
                    break;
                }

                // Store bit (LSB first)
                nec.command_data |= (bit_value << nec.bit_count);
                nec.bit_count++;

                if (nec.bit_count >= 8) {
                    // Complete 8-bit command received
                    decode_frame();
                    nec.state = NEC_FRAME_COMPLETE;
                }
                // Stay in NEC_BIT_BURST state for next bit
            }
            break;

        case NEC_FRAME_COMPLETE:
            // Frame complete - could be start of new frame
            nec.state = NEC_START_BURST;
            nec.bit_count = 0;
            nec.command_data = 0;
            break;

        case NEC_IDLE:
        default:
            // Should not reach here, but handle gracefully
            nec.state = NEC_START_BURST;
            nec.bit_count = 0;
            nec.command_data = 0;
            break;
    }

    nec.last_time = current_time;
}



// Check if new frame is available
bool NEC_Frame_Available(void) {
    return nec.frame_ready;
}

// Get decoded command and clear flag
uint8_t NEC_Get_Command(void) {
    nec.frame_ready = false;
    return nec.command;
}

// Get decoded address (not used in simplified protocol)
uint8_t NEC_Get_Address(void) {
    return 0;
}

