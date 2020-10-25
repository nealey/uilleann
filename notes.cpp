#include <Arduino.h>
#include "notes.h"

const char *NoteNames[] {
  "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G ", "Ab", "A ", "Bb", "B ",
};
float JustPitches[MaxNote + 1];

// Hat tip to Kyle Gann
// https://www.kylegann.com/tuning.html
void setupJustPitches(uint8_t baseNote, float basePitch) {
  JustPitches[baseNote +  0] = basePitch *  1 /  1; // D
  JustPitches[baseNote +  1] = basePitch * 16 / 15; // Eb
  JustPitches[baseNote +  2] = basePitch *  9 /  8; // E
  JustPitches[baseNote +  3] = basePitch *  6 /  5; // F
  JustPitches[baseNote +  4] = basePitch *  5 /  4; // F#
  JustPitches[baseNote +  5] = basePitch *  4 /  3; // G
  JustPitches[baseNote +  6] = basePitch * 45 / 32; // Ab
  JustPitches[baseNote +  7] = basePitch *  3 /  2; // A
  JustPitches[baseNote +  8] = basePitch *  8 /  5; // Bb
  JustPitches[baseNote +  9] = basePitch *  5 /  3; // B
  JustPitches[baseNote + 10] = basePitch *  9 /  5; // C
  JustPitches[baseNote + 11] = basePitch * 15 /  8; // C#

  // Two fourths up from the base pitch, so G major scale sounds right
  JustPitches[baseNote + 11] = basePitch *  8 /  3; // C#

  // Octaves
  for (int note = baseNote; note < baseNote + 12; note++) {
    for (int i = 1; i < 9; i++) {
      int multiplier = 1<<i;
      int shift = i*12;
      int upNote = note + shift;
      int dnNote = note - shift;
      
      if (upNote <= MaxNote) {
        JustPitches[upNote] = JustPitches[note] * multiplier;
      }
      if (dnNote >= 0) {
        JustPitches[dnNote] = JustPitches[note] / multiplier;
      }
    }
  }
}
