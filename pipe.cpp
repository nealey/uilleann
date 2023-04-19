#include "pipe.h"
#include "fingering.h"
#include "tuning.h"


#define CLOSEDVAL 0x30
#define OPENVAL 0x70
#define GLISSANDO_STEPS (OPENVAL - CLOSEDVAL)

Pipe::Pipe() {
  KeysLast = 0;
}

bool Pipe::Init() {
  // Capacative touch sensor
  if (!capSensor.begin(0x5A)) {
    return false;
  }
  
  // Bag button

  return true;
}

void Pipe::Update() {
  uint8_t glissandoKeys = 0;

  KeysLast = Keys;
  Keys = 0;

  // Read the bag state, if there's a bag.
  if (bag_enabled) {
    // XXX: re-enable the bag sensor at some point
    //Bag = bagSensor.isPressed();
  } else {
    Bag = false;
  }

  for (int i = 0; i < NUM_KEYS; ++i) {
    int key = KeySensor[i];
    if (key == -1) {
      continue;
    }

    uint16_t sensorReading = capSensor.filteredData(i);
    uint16_t val = OPENVAL - min(max(sensorReading, CLOSEDVAL), OPENVAL);
    KeyPressure[key] = val / float(GLISSANDO_STEPS);

    // keys = all keys which are at least touched
    // glissandoKeys = all keys which are fully closed
    // The glissando operation computes the difference.
    if (KeyPressure[key] > 0.0) {
      bitSet(Keys, key);
    }
    if (KeyPressure[key] == 1.0)  {
      bitSet(glissandoKeys, key);
    }
  }

  // Compute glissando amount
  GlissandoPressure = 1.0;
  for (int i = 0; i < 8; ++i) {
    if (KeyPressure[i] > 0) {
      GlissandoPressure = min(GlissandoPressure, KeyPressure[i]);
    }
  }

  // Look up notes in the big table
  struct Fingering f = FingeredNote(Keys);
  struct Fingering gf = FingeredNote(glissandoKeys);

  CurrentNote = f.note;
  GlissandoNote = gf.note;

  // Was the high bit set? That indicates "alternate fingering", which sounds different.
  AltFingering = f.alt;

  // All keys closed + knee = no sound
  Silent = ((KeyPressure[11] > 0) && (CurrentNote == NOTE_D4));

  // If the bag is squished, jump up an octave
  // But only if the left thumb is down!
  if (Bag && (Keys & bit(7))) {
    CurrentNote += NOTE_OCTAVE;
    GlissandoNote += NOTE_OCTAVE;
  }

}

bool Pipe::Pressed(uint8_t key) {
  return bitRead(Keys, key);
}

bool Pipe::JustPressed(uint8_t key) {
  if (bitRead(Keys, key)) {
    return !bitRead(KeysLast, key);
  }
  return false;
}

bool Pipe::typematicEvent(uint8_t key, uint16_t delay, uint16_t repeat) {
  if (Pressed(key)) {
    unsigned long now = millis();

    if (JustPressed(key)) {
      nextRepeat[key] = now + max(delay, repeat);
      return true;
    }
    if (now >= nextRepeat[key]) {
      nextRepeat[key] = now + repeat;
      return true;
    }
  }
  return false;
}

Adjust Pipe::ReadAdjust(uint8_t keyUp, uint8_t keyDown, uint16_t delay, uint16_t repeat) {
  bool eventUp = typematicEvent(keyUp, delay, repeat);
  bool eventDown = typematicEvent(keyDown, delay, repeat);

  if (Pressed(keyUp) && Pressed(keyDown)) {
    unsigned long nr = max(nextRepeat[keyUp], nextRepeat[keyDown]);

    nextRepeat[keyUp] = nr;
    nextRepeat[keyDown] = nr;
  }

  if (eventUp && eventDown) {
    return ADJUST_BOTH;
  } else if (eventUp) {
    return ADJUST_UP;
  } else if (eventDown) {
    return ADJUST_DOWN;
  }
  return ADJUST_NONE;
}
