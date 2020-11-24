#include "tuning.h"

#include <math.h>

// 12th root of 2, used for Twelvetone Equal Temperament
#define TET_CONST 1.059463

Tuning::Tuning(Note base, float pitch, TuningSystem system) {
  Setup(base, pitch, system);
}
Tuning::Tuning(Note base, float pitch) {
  Tuning(base, pitch, TUNINGSYSTEM_JUST);
}

// I like just Intonation.
Tuning::Tuning() {
#if 0
  Tuning(NOTE_D4, PITCH_CONCERT_D4, TUNINGSYSTEM_JUST);
#endif
}

Note Tuning::GetBaseNote() { return baseNote; }

void Tuning::SetTuningSystem(TuningSystem system) {
  Setup(baseNote, GetPitch(baseNote), system);
}

TuningSystem Tuning::GetSystem() { return system; }

// setupOctaves computes the entire tuning frequency chart.
//
// You must call this after setting a full octave chromatic scale, rooted at
// base.
void Tuning::setupOctaves(Note base) {
  int multiplier = 1;
  for (Note octave = NOTE_ZERO; octave < NOTE_MAX; octave += NOTE_OCTAVE) {
    for (Note note = base; note < base + NOTE_OCTAVE; note += NOTE_SEMITONE) {
      Note upNote = note + octave;
      Note dnNote = note - octave;

      if (upNote < NOTE_MAX) {
        pitches[upNote] = pitches[note] * multiplier;
      }
      if (dnNote >= NOTE_ZERO) {
        pitches[dnNote] = pitches[note] / multiplier;
      }
    }
    multiplier <<= 1;
  }
}

// setupEqual sets an even-temperament chromatic scale rooted at base
void Tuning::setupEqual(Note base, float pitch) {
  pitches[base] = pitch;
  for (int i = 1; i < 12; i++) {
    pitches[base + i] = pitches[base + i - 1] * TET_CONST;
  }
}

// setupJust sets a just-temperament chromatic scale rooted at base
void Tuning::setupJust(Note base, float pitch) {
  // Diatonic scale
  pitches[base + 0] = pitch * 1 / 1;    // Unison
  pitches[base + 2] = pitch * 9 / 8;    // Second
  pitches[base + 4] = pitch * 5 / 4;    // Third
  pitches[base + 5] = pitch * 4 / 3;    // Fourth
  pitches[base + 7] = pitch * 3 / 2;    // Fifth
  pitches[base + 9] = pitch * 5 / 3;    // Sixth
  pitches[base + 11] = pitch * 15 / 8;  // Seventh

  // I got this off various Wikipedia pages.
  // The main thing here is that the minor seventh works out to be a diatonic
  // fourth up from the fourth computed above, since the music I want to play
  // frequently wants to play G major on a D major instrument
  pitches[base + 1] = pitch * 16 / 15;  // min2
  pitches[base + 3] = pitch * 6 / 5;    // min3
  pitches[base + 6] = pitch * 10 / 7;   // dim5
  pitches[base + 8] = pitch * 8 / 5;    //  min6
  pitches[base + 10] = pitch * 16 / 9;  // min7 = fourth + fourth
}

void Tuning::Setup(Note base, float pitch, TuningSystem system) {
  this->baseNote = base;
  this->system = system;

  switch (system) {
    case TUNINGSYSTEM_EQUAL:
      setupEqual(base, pitch);
      break;
    case TUNINGSYSTEM_JUST:
    default:
      setupJust(base, pitch);
      break;
  }
  setupOctaves(base);
}

void Tuning::Setup(Note base, float pitch) { Setup(base, pitch, system); }

float Tuning::GetPitch(Note note) { return pitches[note]; }

Note NearestNote(float pitch) {
  return Note(round(log2(pitch / PITCH_CONCERT_C0)));
}

const char *noteNames[]{
    "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G ", "Ab", "A ", "Bb", "B ",
};

const char *NoteName(Note note) { return noteNames[note % 12]; }

