#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioMixer4              feedback;         //xy=110,37
AudioSynthWaveformSineModulated osc4;           //xy=112,98
AudioSynthWaveformSineModulated osc2;           //xy=112,194
AudioSynthWaveformSineModulated osc1;           //xy=112,245
AudioSynthWaveformSineModulated osc3;           //xy=113,146
AudioMixer4              mixOp;          //xy=114,418
AudioEffectEnvelope      env4;           //xy=251,97
AudioEffectEnvelope      env3;           //xy=251,146
AudioEffectEnvelope      env2;           //xy=252,194
AudioEffectEnvelope      env1;           //xy=252,245
AudioFilterBiquad        biquad1;        //xy=257,418
AudioMixer4              mixL;           //xy=472,402
AudioMixer4              mixR;           //xy=473,498
AudioOutputAnalogStereo  dacs1;          //xy=724,452
AudioConnection          patchCord1(feedback, osc4);
AudioConnection          patchCord2(osc4, env4);
AudioConnection          patchCord3(osc4, 0, feedback, 0);
AudioConnection          patchCord4(osc2, env2);
AudioConnection          patchCord5(osc1, env1);
AudioConnection          patchCord6(osc3, env3);
AudioConnection          patchCord7(mixOp, biquad1);
AudioConnection          patchCord8(env4, osc3);
AudioConnection          patchCord9(env4, 0, mixOp, 3);
AudioConnection          patchCord10(env3, 0, mixOp, 2);
AudioConnection          patchCord11(env2, osc1);
AudioConnection          patchCord12(env2, 0, mixOp, 1);
AudioConnection          patchCord13(env1, 0, mixOp, 0);
AudioConnection          patchCord14(biquad1, 0, mixL, 0);
AudioConnection          patchCord15(biquad1, 0, mixR, 0);
AudioConnection          patchCord17(mixL, 0, dacs1, 0);
AudioConnection          patchCord18(mixR, 0, dacs1, 1);
// GUItool: end automatically generated code
