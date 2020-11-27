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

  // Knee sensor
  if (!kneeSensor.begin()) {
    return false;
  }
  
  // Bag button
  bagSensor.begin();
  // This library takes the entire program out if you poll it 5-40 times without anything connected
  bag_enabled = bagSensor.isConnected();

  return true;
}

void Pipe::Update() {
  uint8_t glissandoKeys = 0;

  KeysLast = Keys;
  Keys = 0;

  // Read the bag state, if there's a bag.
  // if there isn't a bag, don't try, or this library will crash the program.
  if (bag_enabled) {
    Bag = bagSensor.isPressed();
  } else {
    Bag = false;
  }

  // 0x6c is actually 8 bytes, but all 8 are always the same...
  KneeClosedness = 255 - kneeSensor.readRange();

  for (int i = 0; i < NUM_KEYS; ++i) {
    uint16_t sensorReading = capSensor.filteredData(i);
    uint16_t val = OPENVAL - min(max(sensorReading, CLOSEDVAL), OPENVAL);
    KeyPressure[i] = val / float(GLISSANDO_STEPS);

    // keys = all keys which are at least touched
    // glissandoKeys = all keys which are fully closed
    // The glissando operation computes the difference.
    if (KeyPressure[i] > 0.0) {
      bitSet(Keys, i);
    }
    if (KeyPressure[i] == 1.0)  {
      bitSet(glissandoKeys, i);
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

  // If the bag is squished, jump up an octave
  // But only if the left thumb is down!
  if (Bag && (Keys & bit(7))) {
    CurrentNote += NOTE_OCTAVE;
    GlissandoNote += NOTE_OCTAVE;
  }

  // All keys closed + knee = no sound
  Silent = ((KneeClosedness > 240) && (Keys == 0xff));
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
