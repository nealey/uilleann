#pragma once
#include <stdint.h>

enum TuningSystem {
  TUNINGSYSTEM_JUST,
  TUNINGSYSTEM_EQUAL,
  TUNINGSYSTEM_MAX = TUNINGSYSTEM_EQUAL,
};

enum Note {
  NOTE_C0,
  NOTE_CS0,
  NOTE_D0,
  NOTE_DS0,
  NOTE_E0,
  NOTE_F0,
  NOTE_FS0,
  NOTE_G0,
  NOTE_GS0,
  NOTE_A0,
  NOTE_AS0,
  NOTE_B0,
  NOTE_C1,
  NOTE_CS1,
  NOTE_D1,
  NOTE_DS1,
  NOTE_E1,
  NOTE_F1,
  NOTE_FS1,
  NOTE_G1,
  NOTE_GS1,
  NOTE_A1,
  NOTE_AS1,
  NOTE_B1,
  NOTE_C2,
  NOTE_CS2,
  NOTE_D2,
  NOTE_DS2,
  NOTE_E2,
  NOTE_F2,
  NOTE_FS2,
  NOTE_G2,
  NOTE_GS2,
  NOTE_A2,
  NOTE_AS2,
  NOTE_B2,
  NOTE_C3,
  NOTE_CS3,
  NOTE_D3,
  NOTE_DS3,
  NOTE_E3,
  NOTE_F3,
  NOTE_FS3,
  NOTE_G3,
  NOTE_GS3,
  NOTE_A3,
  NOTE_AS3,
  NOTE_B3,
  NOTE_C4,
  NOTE_CS4,
  NOTE_D4,
  NOTE_DS4,
  NOTE_E4,
  NOTE_F4,
  NOTE_FS4,
  NOTE_G4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_AS4,
  NOTE_B4,
  NOTE_C5,
  NOTE_CS5,
  NOTE_D5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_F5,
  NOTE_FS5,
  NOTE_G5,
  NOTE_GS5,
  NOTE_A5,
  NOTE_AS5,
  NOTE_B5,
  NOTE_C6,
  NOTE_CS6,
  NOTE_D6,
  NOTE_DS6,
  NOTE_E6,
  NOTE_F6,
  NOTE_FS6,
  NOTE_G6,
  NOTE_GS6,
  NOTE_A6,
  NOTE_AS6,
  NOTE_B6,
  NOTE_C7,
  NOTE_CS7,
  NOTE_D7,
  NOTE_DS7,
  NOTE_E7,
  NOTE_F7,
  NOTE_FS7,
  NOTE_G7,
  NOTE_GS7,
  NOTE_A7,
  NOTE_AS7,
  NOTE_B7,
  NOTE_C8,
  NOTE_CS8,
  NOTE_D8,
  NOTE_DS8,
  NOTE_E8,
  NOTE_F8,
  NOTE_FS8,
  NOTE_G8,
  NOTE_GS8,
  NOTE_A8,
  NOTE_AS8,
  NOTE_B8,
  NOTE_ZERO = 0,
  NOTE_SEMITONE = 1,
  NOTE_WHOLETONE = 2,
  NOTE_OCTAVE = NOTE_C1,
  NOTE_MAX = NOTE_B8,
};

#define PITCH_CONCERT_C0 16.35
#define PITCH_CONCERT_A4 440.00
#define PITCH_CONCERT_D4 293.66

// Twelvetone Equal Temperament semitone multiplier
// Take any frequency and multiply it by this magic number to get a semitone higher!
// Divide to get a semitone lower!
// This is an approximation of exp(2, 1/12),
// which was worked out in around the 1500s.
#define TET_SEMITONE_MULTIPLIER 1.059463

class Tuning {
 public:
  // name contains the name of the current tuning system
  const char *name;

  Tuning(Note base, float pitch, TuningSystem system);
  Tuning(Note base, float pitch);
  Tuning();
  void Setup(Note base, float pitch, TuningSystem system);
  void Setup(Note base, float pitch);
  void SetTuningSystem(TuningSystem system);
  TuningSystem GetTuningSystem();
  Note GetBaseNote();
  float GetPitch(Note note);

 private:
  TuningSystem system;
  Note baseNote;
  float pitches[NOTE_MAX];

  void setupOctaves(Note base);
  void setupJust(Note base, float pitch);
  void setupEqual(Note base, float pitch);
};

// NearestNote returns the note nearest to pitch.
Note NearestNote(float pitch);

// NoteName returns the name of a note (without octave).
const char *NoteName(Note note);

// TuningSystemName returns the name of a tuning system.
const char *TuningSystemName(TuningSystem system);

// Make notes support some arithmetic
inline Note toNote(int a) {
  if (a < NOTE_ZERO) {
    return NOTE_ZERO;
  } else if (a > NOTE_MAX) {
    return NOTE_MAX;
  } else {
    return Note(a);
  }
}
inline Note operator+(const Note &a, const Note b) {
  return toNote(int(a) + int(b));
}
inline Note &operator+=(Note &a, const Note b) {
  return a = a + b;
}
inline Note &operator++(Note &a) {
  return a += NOTE_SEMITONE;
}
inline Note operator-(const Note a, const Note b) {
  return toNote(int(a) - int(b));
}
inline Note &operator-=(Note &a, const Note b) {
  return a = a - b;
}
inline Note &operator--(Note &a) {
  return a -= NOTE_SEMITONE;
}
inline Note operator*(const Note a, const int b) {
  return toNote(int(a) * b);
}
inline int operator/(const Note a, const int b) {
  return int(a) / b;
}
inline int operator/(const Note a, const Note b) {
  return int(a) / b;
}

inline TuningSystem operator++(TuningSystem &a) {
  return a = TuningSystem((int(a) + 1) % int(TUNINGSYSTEM_MAX + 1));
}
