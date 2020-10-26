#include <Audio.h>
#include <Wire.h>
#include <Adafruit_NeoTrellisM4.h>
#include <SFE_MicroOLED.h>
#include <SparkFun_Qwiic_Button.h>
#include <Adafruit_MPR121.h>
#include "synth.h"
#include "patches.h"
#include "notes.h"
#include "fingering.h"

//#define DEBUG
#define KNEE_OFFSET 0
#define KEY_OFFSET 2

FMVoice Chanter;
FMVoice Drones[3];
FMVoice Regulators[3];

AudioFilterBiquad        biquad1;
AudioMixer4              mixDrones;
AudioMixer4              mixRegulators;
AudioMixer4              mixL;
AudioMixer4              mixR;
AudioOutputAnalogStereo  dacs1;

AudioSynthNoiseWhite debug;

AudioConnection FMVoicePatchCords[] = {
  {debug, 0, mixR, 3}, // Don't know why, but the first one is ignored
  {debug, 0, mixL, 3},

  {mixL, 0, dacs1, 0},
  {mixR, 0, dacs1, 1},

  {Chanter.outputMixer, 0, biquad1, 0},
  {biquad1, 0, mixL, 0},
  {biquad1, 0, mixR, 0},

  {Drones[0].outputMixer, 0, mixDrones, 0},
  {Drones[1].outputMixer, 0, mixDrones, 1},
  {Drones[2].outputMixer, 0, mixDrones, 2},
  {mixDrones, 0, mixL, 1},
  {mixDrones, 0, mixR, 1},

  {Regulators[0].outputMixer, 0, mixRegulators, 0},
  {Regulators[1].outputMixer, 0, mixRegulators, 1},
  {Regulators[2].outputMixer, 0, mixRegulators, 2},
  {mixRegulators, 0, mixL, 2},
  {mixRegulators, 0, mixR, 2},

  FMVoiceWiring(Chanter),
  FMVoiceWiring(Drones[0]),
  FMVoiceWiring(Drones[1]),
  FMVoiceWiring(Drones[2]),
  FMVoiceWiring(Drones[3]),
  FMVoiceWiring(Regulators[0]),
  FMVoiceWiring(Regulators[1]),
  FMVoiceWiring(Regulators[2]),
  FMVoiceWiring(Regulators[3]),
};

int currentPatch = 0;

Adafruit_MPR121 cap = Adafruit_MPR121();
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
MicroOLED oled(9, 1);
QwiicButton bag;

void setup() {
  setupJustPitches(NOTE_D4, PITCH_D4);
  pinMode(LED_BUILTIN, OUTPUT);

  // Wire.begin needs a moment
  delay(100);
  Wire.begin();

  // Initialize OLED display
  oled.begin();
  oled.clear(ALL);

  // Initialize bag
  bag.begin();

  // Initialize the Trellis
  trellis.begin();

  // Initialize touch sensor
  bool blink = true;
  while (!cap.begin(0x5A)) {
    oled.clear(PAGE);
    oled.setCursor(0, 0);
    oled.print("No Pipe?");
    oled.display();

    trellis.setPixelColor(0, blink?0xff6666:0);
    blink = !blink;
    delay(200);
  }

  // Set aside some memory for the audio library  
  AudioMemory(120);

  // initialize tunables
  updateTunables(3, 0);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  // Turn on drones
  for (int i=0; i<3; i++) {
    float detune = (1-i) * 0.002;
    FMVoiceLoadPatch(&Drones[i], &Bank[0]);
    FMVoiceNoteOn(&Drones[i], JustPitches[NOTE_D4 - 12*i] * (1 + detune));
  }

  // Turn on all mixer channels
  for (int i=0; i<4; i++) {
    mixL.gain(i, 0.5);
    mixR.gain(i, 0.6);
  }
  
  debug.amplitude(0.1);
  mixL.gain(3, 0);
  mixR.gain(3, 0);
}

#define BUTTON_UP 0
#define BUTTON_DOWN 8
#define BUTTON_PITCH 24
#define BUTTON_VOLUME 25

#define INIT_PITCH_ADJUST 0
#define INIT_GAIN 0.7
#define INIT_PATCH 0

int16_t pitchAdjust;
float chanterGain;
int patch;

void updateTunables(uint8_t buttons, int note) {
  // Pitch adjust if playing A
  if (!note || (note == NOTE_A4)) {
    switch (buttons) {
    case 3:
      pitchAdjust = INIT_PITCH_ADJUST;
      break;
    case 2:
      pitchAdjust += 4;
      break;
    case 1:
      pitchAdjust -= 4;
      break;
    }
  }

  float adj = pow(2, pitchAdjust / 32768.0);
  setupJustPitches(NOTE_D4, PITCH_D4*adj);
  trellis.setPixelColor(BUTTON_PITCH, trellis.ColorHSV(uint16_t(pitchAdjust), 255, 80));

  if (!note || (note == NOTE_G4)) {
    // Volume adjust if playing G
    switch (buttons) {
    case 3:
      chanterGain = INIT_GAIN;
      break;
    case 2:
      chanterGain = min(chanterGain+0.005, 1.0);
      break;
    case 1:
      chanterGain = max(chanterGain-0.005, 0.0);
      break;
    }
  }

  for (int i=0; i<3; i++) {
    mixL.gain(i, chanterGain);
    mixR.gain(i, chanterGain);
  }
  trellis.setPixelColor(BUTTON_VOLUME, trellis.ColorHSV(uint16_t(chanterGain * 65535), 255, 80));

  if (!note || (note == NOTE_CS5)) {
    if (buttons == 3) {
      patch = INIT_PATCH;
    } else if (trellis.justPressed(BUTTON_DOWN)) {
      patch -= 1;
    } else if (trellis.justPressed(BUTTON_UP)) {
      patch += 1;
    }

    // wrap
    int bankSize = sizeof(Bank) / sizeof(Bank[0]);
    patch = (patch + bankSize) % bankSize;

    FMPatch *p = &Bank[patch];
    FMVoiceLoadPatch(&Chanter, p);

    oled.clear(PAGE);
    oled.setFontType(0);
    oled.setCursor(0, 0);
    oled.print(p->name);
    oled.setCursor(0, 10);
    oled.print("Patch ");
    oled.print(patch);
    oled.display();
  }
}

const uint8_t CLOSEDVAL = 0x30;
const uint8_t OPENVAL = 0x70;
const uint8_t GLISSANDO_STEPS = OPENVAL - CLOSEDVAL;

bool playing = false;

void loop() {
  uint8_t keys = 0;
  uint8_t note;
  uint8_t glissandoKeys = 0;
  uint8_t glissandoNote;
  float glissandoOpenness = 0;
  bool silent = false;
  bool knee = cap.filteredData(KNEE_OFFSET) < CLOSEDVAL;
  uint8_t buttons = trellis.isPressed(BUTTON_DOWN)?1:0 | trellis.isPressed(BUTTON_UP)?2:0;

  trellis.tick();

  for (int i = 0; i < 8; i++) {
    uint16_t val = max(cap.filteredData(i+KEY_OFFSET), CLOSEDVAL);
    float openness = ((val - CLOSEDVAL) / float(GLISSANDO_STEPS));

    // keys = all keys which are at least touched
    // glissandoKeys = all keys which are fully closed
    // The glissando operation computes the difference.
    if (openness < 1.0) {
      glissandoOpenness = max(glissandoOpenness, openness);
      bitSet(keys, i);

      if (openness == 0.0) {
        bitSet(glissandoKeys, i);
      }
    }
    
    // print key states
    //trellis.setPixelColor(7 - i, trellis.ColorHSV(65536/12, 255, 120*openness));
    trellis.setPixelColor(7 - i, trellis.ColorHSV(22222*openness, 255, 40));
  }
  
  note = uilleann_matrix[keys];
  glissandoNote = uilleann_matrix[glissandoKeys];

  bool alt = note & 0x80;
  bool galt = glissandoNote & 0x80;
  note = note & 0x7f;
  glissandoNote = glissandoNote & 0x7f;

  // All keys closed + knee = no sound
  if (knee) {
    if (keys == 0xff) {
      silent = true;
    }
  }

  // Jump octave if the bag is squished
  //bag = !digitalRead(BAG);
  if (bag.isPressed()) {
    if (keys & bit(7)) {
      note += 12;
      glissandoNote += 12;
    }
  }

  // Read some trellis button states
  if (buttons) {
    updateTunables(buttons, note);
  }

  if (silent) {
    FMVoiceNoteOff(&Chanter);
  } else {
    // Calculate pitch, and glissando pitch
    uint16_t pitch = JustPitches[note];
    uint16_t glissandoPitch = JustPitches[glissandoNote];

    if (alt) {
      biquad1.setLowShelf(0, 2000, 0.2, 1);
    } else {
      biquad1.setHighShelf(0, 1000, 1.0, 1);
    }
  
    // Bend pitch if fewer than 3 half steps away
    if (abs(glissandoNote - note) < 3) {
      float diff = glissandoPitch - pitch;
      pitch += diff * glissandoOpenness;
    }

    if (Chanter.playing) {
      FMVoiceSetPitch(&Chanter, pitch);
    } else {
      FMVoiceNoteOn(&Chanter, pitch);
    }
  }
}
