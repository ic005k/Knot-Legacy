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

//--------------------------------------------------------------------------------

#define EPSILON (1E-6)

#define SENSOR_SAMP_FREQ (104)
#define SENSOR_SAMP_INTVL (1.0f / SENSOR_SAMP_FREQ)
#define BUFF_FACTOR (2)
#define SAMP_BUFF_LEN (SENSOR_SAMP_FREQ / BUFF_FACTOR)
#define MAX_TC_SAMPLES (20)
float timestamp = 0.0f; /* sec */

/* Filter data struct for 2nd order filter */
/* Y(n) = b0*X(n) + b1*X(n-1) + b2*X(n-2)  */
/*           - a1*Y(n-1) - a2*Y(n-2)       */
/* X(n) is new input, X(n-1) is prev input */
/* and so on                               */
typedef struct {
  float b0;
  float b1;
  float b2;
  float a1;
  float a2;
  float prev_in;
  float prev_prev_in;
  float prev_out;
  float prev_prev_out;
  unsigned int TC_samples;
} filter_t;

/* enum for motion types */
typedef enum { STATIC = 0, WALK, HOP, RUN, NUM_TYPES } motion_type_t;

/* 3axis enumeration */
typedef enum { CHX = 0, CHY, CHZ, NUM_DIM } axis_t;

/* algorithm output data structure */
typedef struct {
  unsigned int step_count;
  motion_type_t step_type;
  float prev_max;
  float prev_min;
  float prev_max_ts;
  float prev_min_ts;
} algo_out_t;

/* Function prototypes */
static float apply_filter(filter_t *filt_data, float in_data);

static unsigned int step_algo_preproc(float timestamp, float arx, float ary,
                                      float arz, float grx, float gry,
                                      float grz);

static void step_algo_run(algo_out_t *step_algo_output);

#endif /* count_steps_h */
