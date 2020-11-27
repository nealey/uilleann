#pragma once

#include <Adafruit_MPR121.h>
#include <Adafruit_VL6180X.h>
#include <SparkFun_Qwiic_Button.h>
#include <stdint.h>
#include "tuning.h"

#define NUM_KEYS 12

enum Adjust {
  ADJUST_DOWN = -1,
  ADJUST_NONE = 0,
  ADJUST_UP = 1,
  ADJUST_BOTH,
};

class Pipe {
 public:
  // kneeClosedness indicates how "closed" the knee sensor is. 0 = wide open.
  uint8_t KneeClosedness;

  // keys are which keys are being pressed.
  uint16_t Keys;
  uint16_t KeysLast;
  float KeyPressure[NUM_KEYS];

  // note holds the note being played, according to the fingering chart.
  Note CurrentNote;

  // glissandoNote is the note that would be played if partially open keys were fully open.
  Note GlissandoNote;

  // glissandoPressure is how "closed" the holes are in the direction away from the glissandoNote.
  float GlissandoPressure;

  // silent is true if all keys and the knee are closed.
  bool Silent;

  // bag is true if the bag is being squished.
  bool Bag;

  // altFingering is true if the "alternate fingering" is being played.
  // This should sound different than the standard fingering.
  bool AltFingering;

  Pipe();

  // Init initializes everything.
  //
  // Returns true if it all worked. You can run it again if it didn't.
  bool Init();

  // Update reads sensors and updates pipe state.
  //
  // It should be run once per loop.
  void Update();

  // Pressed returns whether the given key is pressed.
  bool Pressed(uint8_t key);

  // JustPressed returns whether the given key was just pressed.
  bool JustPressed(uint8_t key);

  // ReadAdjust returns the input for two keys paired as up/down.
  //
  // delay is the number of milliseconds to wait before repeating a key
  // repeat is the number of milliseconds to wait between repeated keystrokes
  Adjust ReadAdjust(uint8_t upKey, uint8_t downKey, uint16_t delay, uint16_t repeat);

 private:
  Adafruit_MPR121 capSensor;
  Adafruit_VL6180X kneeSensor;
  QwiicButton bagSensor;
  bool bag_enabled;
  unsigned long nextRepeat[NUM_KEYS];
  bool typematicEvent(uint8_t key, uint16_t delay, uint16_t repeat);
};
