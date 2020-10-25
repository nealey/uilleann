// "Factory" patches

#pragma once
#include "dx9.h"

// Waveform, offset, multiplier, delay, attack, holdAmp, hold, decay, sustainAmp, release
FMPatch Bank[] = {
  {
    "Venus Oboe",
    DX9_ALG_5(0),
    {
      // Waveform    off   mult  del   att  hldA   hld  dec  susA  rel
      {WAVEFORM_SINE,  0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.75,   5},
      {WAVEFORM_SINE,  0,  4.00,   0, 10.5,  1.0, 10.5,   0, 0.80,   5},
      {WAVEFORM_SINE,  0,  8.00,   0, 10.5,  1.0, 10.5,   0, 0.50,   5},
      {WAVEFORM_SINE,  0, 16.00,   0, 10.5,  1.0, 50.0,   0, 0.75,   5},
    },
  },
  {
    "IWantPizza",
    DX9_ALG_1(0),
    {
      {WAVEFORM_SINE,  0,  4.00,   0, 10.5,  1.0, 10.5,   0, 0.35,  20},
      {WAVEFORM_SINE,  0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.30,  20},
      {WAVEFORM_SINE,  0,  8.00,   0, 10.5,  1.0, 10.5,   0, 0.50,  20},
      {WAVEFORM_SINE,  0, 16.00,   0, 10.5,  1.0,   50,   0, 0.25,  20},
    },
  },
  {
    "Ray Gun",
    DX9_ALG_1(0),
    {
      {WAVEFORM_SINE,  0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.35,  20},
      {WAVEFORM_SINE,  0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.30,  20},
      {WAVEFORM_SINE,  0,  9.00,   0, 10.5,  1.0, 10.5,   0, 0.00,  20},
      {WAVEFORM_SINE,  0,  1.00,   0, 10.5,  1.0,   50,   0, 0.25,   8},
    },
  },
};
