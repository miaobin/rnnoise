#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include "rnnoise.h"

#define FRAME_SIZE_SHIFT 2
#define FRAME_SIZE (120<<FRAME_SIZE_SHIFT)
#define WINDOW_SIZE (2*FRAME_SIZE)
#define FREQ_SIZE (FRAME_SIZE + 1)

#define NB_BANDS 22
#define NB_DELTA_CEPS 6
#define NB_FEATURES (NB_BANDS+3*NB_DELTA_CEPS+2)

#define _min(a, b)                    (((a) < (b)) ? (a) : (b))
#define _max(a, b)                    (((a) > (b)) ? (a) : (b))
#define _clamp(x, lo, hi)             (_max((lo), _min((hi), (x))))

typedef struct {
  kiss_fft_cpx X[FREQ_SIZE];
  kiss_fft_cpx P[WINDOW_SIZE];
  float Ex[NB_BANDS];
  float Ep[NB_BANDS];
  float Exp[NB_BANDS];
} PitchFilterState;

#endif
