#include "process.h"
#include <emscripten.h>

DenoiseState *st;
PitchFilterState *pf;
PitchFilterState *pf_frames;

float *EMSCRIPTEN_KEEPALIVE pre_processing(float *input, int frames) {
  float x[FRAME_SIZE];
  float y[NB_FEATURES];
  int silence = 0;
  int input_size = FRAME_SIZE * frames;
  float rnnoise_pcm[input_size];

  st = (DenoiseState *)malloc(rnnoise_get_size());
  pf = (PitchFilterState *)malloc(sizeof(PitchFilterState));
  pf_frames = (PitchFilterState *)malloc(sizeof(PitchFilterState) * frames);

  for (int i = 0; i < input_size; i++) {
    rnnoise_pcm[i] = input[i] * 32768.0f;
  }

  float *features = (float *)malloc(sizeof(*features) * NB_FEATURES * frames);
  for (int n = 0; n < frames; n++) {
    for (int i = 0; i < FRAME_SIZE; i++) {
      x[i] = rnnoise_pcm[i + n * FRAME_SIZE];
    }
    silence = pre_process_frame(st, x, pf->X, pf->P, pf->Ex, pf->Ep, pf->Exp, y);
    for (int i = 0; i < NB_FEATURES; i++) {
      features[i + n * NB_FEATURES] = y[i];
    }
    pf_frames[n] = *pf;
  }

  return features;
}

float *EMSCRIPTEN_KEEPALIVE post_processing(float *input, int frames) {
  float gains[NB_BANDS];
  float out[FRAME_SIZE];
  int input_size = FRAME_SIZE * frames;
  float *web_pcm = (float *)malloc(sizeof(*web_pcm) * input_size);

  for (int n = 0; n < frames; n++) {
    for (int i = 0; i < NB_BANDS; i++) {
      gains[i] = input[i + n * NB_BANDS];
    }
    memcpy(pf, &pf_frames[n], sizeof(PitchFilterState));
    post_process_frame(st, gains, pf->X, pf->P, pf->Ex, pf->Ep, pf->Exp, out);
    for (int i = 0; i < FRAME_SIZE; i++) {
      web_pcm[i + n * FRAME_SIZE] = out[i];
    }
  }

  for (int i = 0; i < input_size; i++) {
    web_pcm[i] = _clamp(web_pcm[i], -32768, 32767) * (1.0f / 32768.0);
  }

  free(st);
  free(pf);
  free(pf_frames);

  return web_pcm;
}
