#pragma once
#include <stdint.h>

enum TuningSystem {
  TUNINGSYSTEM_JUST,
  TUNINGSYSTEM_EQUAL,
  TUNINGSYSTEM_MAX = TUNINGSYSTEM_EQUAL,
};

// Twelve-Tone Note (one chromatic scale)
#define NOTE_TT(o) NOTE_C##o, NOTE_Cs##o, NOTE_Db##o = NOTE_Cs##o,                        \
                                          NOTE_D##o, NOTE_Ds##o, NOTE_Eb##o = NOTE_Ds##o, \
                                          NOTE_E##o,                                      \
                                          NOTE_F##o, NOTE_Fs##o, NOTE_Gb##o = NOTE_Fs##o, \
                                          NOTE_G##o, NOTE_Gs##o, NOTE_Ab##o = NOTE_Gs##o, \
                                          NOTE_A##o, NOTE_As##o, NOTE_Bb##o = NOTE_As##o, \
                                          NOTE_B##o

enum Note {
  NOTE_TT(0),
  NOTE_TT(1),
  NOTE_TT(2),
  NOTE_TT(3),
  NOTE_TT(4),
  NOTE_TT(5),
  NOTE_TT(6),
  NOTE_TT(7),
  NOTE_TT(8),
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
