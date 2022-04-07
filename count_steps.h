#ifndef count_steps_h
#define count_steps_h
#include "stdint.h"

// 20 hz sampling rate
#define SAMPLING_RATE 20

// 80 sets of accelerometer readings (so in other words, 80*3 = 240 samples)
#define NUM_TUPLES 80

// window length in seconds
#define WINDOW_LENGTH NUM_TUPLES / SAMPLING_RATE

uint8_t count_steps(int8_t *data);

#endif /* count_steps_h */
