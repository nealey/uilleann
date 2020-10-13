typedef struct Operator {
  float gain;
  float delay;
  float attack;
  float hold;
  float decay;
  float sustain;
  float release;
  float baseFrequency;
  float multiplier;
} Operator;

typedef struct Patch {
  char *name;
  Operator operators[4];
  float feedback;
} Patch;

Patch Bank[] = {
  {
    "Venus Oboe",
    {
      {1.0,  0, 10.5,  0, 5000, 0.75,  5.0,  0,  1.00},
      {1.0,  0, 10.5,  0, 2000, 0.80,  5.0,  0,  4.00},
      {0.0,  0, 10.5,  0, 2000, 0.50,  5.0,  0,  8.00},
      {0.0,  0, 50.0,  0,  800, 0.75,  5.0,  0, 16.00},
    },
    0.0,
  },
  {
    "IWantPizza",
    {
      {1.0,  0, 10.5,  0, 5000, 0.35,  100, 0,  4.00},
      {1.0,  0, 10.5,  0, 2000, 0.30,  100, 0,  1.00},
      {1.0,  0, 10.5,  0, 2000, 0.50,  100, 0,  8.00},
      {1.0,  0,  200,  0,  800, 0.25,  100, 0, 16.00},
    },
    0.0,
  },
  {
    "Ray Gun",
    {
      {1.0,  0, 10.5,  0, 5000, 0.35, 2000,  0, 1.00},
      {1.0,  0, 10.5,  0, 2000, 0.30, 2000,  0, 1.00},
      {1.0,  0, 10.5,  0, 2000, 0.00, 2000,  0, 9.00},
      {1.0,  0,  200,  0,  800, 0.25,  800,  0, 1.00},  
    },
    0.0,
  },
};
