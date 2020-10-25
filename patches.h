// "Factory" patches

#pragma once
#include "algorithms.h"

// Waveform, offset, multiplier, delay, attack, holdAmp, hold, decay, sustainAmp, release
FMPatch Bank[] = {
  {
    "Square",
    ALG_SIMPLE,
    {
      // Waveform     offs   mult dely  attk  hldA   hld decy  susA rels
      {WAVEFORM_SQUARE,  0,  1.00,   0, 10.5,  0.8, 10.5,  10, 0.50,   5},
    },
  },
  {
    "Tubey",
    ALG_OPL2_1(0),
    {
      // Waveform     offs   mult dely  attk  hldA   hld decy  susA rels
      {WAVEFORM_SINE,    0,  1.00,   0, 10.5,  1.0, 10.5,  10, 0.80,   5},
      {WAVEFORM_SINE,    0,  8.00,   0, 10.5,  0.2, 10.5,  10, 0.10,   5},
    },
  },
  {
    "IWantPizza",
    ALG_DX9_1(0),
    {
      {WAVEFORM_SINE,    0,  4.00,   0, 10.5,  1.0, 10.5,   0, 0.35,  20},
      {WAVEFORM_SINE,    0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.30,  20},
      {WAVEFORM_SINE,    0,  8.00,   0, 10.5,  1.0, 10.5,   0, 0.50,  20},
      {WAVEFORM_SINE,    0, 16.00,   0, 10.5,  1.0,   50,   0, 0.25,  20},
    },
  },
  {
    "Ray Gun",
    ALG_DX9_1(0),
    {
      {WAVEFORM_SINE,    0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.35,  20},
      {WAVEFORM_SINE,    0,  1.00,   0, 10.5,  1.0, 10.5,   0, 0.30,  20},
      {WAVEFORM_SINE,    0,  9.00,   0, 10.5,  1.0, 10.5,   0, 0.00,  20},
      {WAVEFORM_SINE,    0,  1.00,   0, 10.5,  1.0,   50,   0, 0.25,   8},
    },
  },
};
