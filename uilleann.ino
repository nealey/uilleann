#include <Audio.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_Qwiic_Button.h>
#include <Adafruit_MPR121.h>
#include <paj7620.h>
#include "synth.h"
#include "patches.h"
#include "notes.h"
#include "fingering.h"

#define DRONES
#define DEBUG
#define KEY_OFFSET 2

FMVoice Chanter;
FMVoice Drones[3];
FMVoice Regulators[3];

AudioFilterBiquad    biquad1;
AudioMixer4          mixDrones;
AudioMixer4          mixRegulators;
AudioMixer4          mixL;
AudioMixer4          mixR;
AudioOutputI2S       out1;
AudioSynthNoiseWhite noise;

AudioConnection FMVoicePatchCords[] = {
  //{0, 0, 0, 0}, // For some reason, the first one is ignored

  {noise, 0, mixDrones, 3},
  {noise, 0, mixL, 3},
  {noise, 0, mixR, 3},

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

  {mixL, 0, out1, 0},
  {mixR, 0, out1, 1},

  FMVoiceWiring(Chanter),
  FMVoiceWiring(Drones[0]),
  FMVoiceWiring(Drones[1]),
  FMVoiceWiring(Drones[2]),
  FMVoiceWiring(Regulators[0]),
  FMVoiceWiring(Regulators[1]),
  FMVoiceWiring(Regulators[2]),
};

int currentPatch = 0;

Adafruit_MPR121 cap = Adafruit_MPR121();
Adafruit_SSD1306 display(128, 32, &Wire, -1);
QwiicButton bag;
bool use_bag;

void blink(bool forever) {
  for (;;) {
    digitalWrite(LED_BUILTIN, true);
    delay(200);
    digitalWrite(LED_BUILTIN, false);
    delay(200);
    if (! forever) break;
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  setupJustPitches(NOTE_D4, PITCH_D4);

  // Wire.begin needs a moment
  delay(100);
  Wire.begin();

  // Initialize gesture/proximity sensor
  if (paj7620Init()) {
    // XXX: Error handling
  }

  // Initialize display display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    blink(true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Starting");
  display.display();

  // Initialize bag
  bag.begin();
  use_bag = bag.isConnected();

  // Initialize touch sensor
  while (!cap.begin(0x5A)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Pipe?");
    display.display();
    blink(false);
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
    FMVoiceLoadPatch(&Drones[i], &Bank[0]);
    FMVoiceNoteOn(&Drones[i], JustPitches[NOTE_D4 - 12*i] + i);
  }

  // Turn on all mixer channels
  for (int i=0; i<4; i++) {
    mixL.gain(i, 0.5);
    mixR.gain(i, 0.6);
  }
  
#ifdef DEBUG
  noise.amplitude(0.1);
  mixL.gain(3, 0.1);
  mixR.gain(3, 0.1);
#endif
}

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

  if (!note || (note == NOTE_CS5)) {
    if (buttons == 3) {
      patch = INIT_PATCH;
    }

    // wrap
    int bankSize = sizeof(Bank) / sizeof(Bank[0]);
    patch = (patch + bankSize) % bankSize;

    FMPatch *p = &Bank[patch];
    FMVoiceLoadPatch(&Chanter, p);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(p->name);
    display.setCursor(0, 10);
    display.print("Patch ");
    display.print(patch);
    display.display();
  }
}

const uint8_t CLOSEDVAL = 0x30;
const uint8_t OPENVAL = 0x70;
const uint8_t GLISSANDO_STEPS = OPENVAL - CLOSEDVAL;

uint8_t loopno = 0;
uint8_t last_note = 0;

void loop() {
  uint8_t keys = 0;
  uint8_t note;
  uint8_t glissandoKeys = 0;
  uint8_t glissandoNote;
  float glissandoOpenness = 0;
  bool silent = false;
  uint8_t paj_knee = 127;
  bool knee = false;

  loopno++;

  paj7620ReadReg(0x6c, 1, &paj_knee);
  if (paj_knee  > 240) {
    knee = true;
  }

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
  // Look up the note name
    char *note_name = NoteNames[note % 12];
    if (silent) {
      note_name = "-";
    }

  // Jump octave if the bag is squished
  //bag = !digitalRead(BAG);
  if (use_bag && bag.isPressed()) {
    if (keys & bit(7)) {
      note += 12;
      glissandoNote += 12;
    }
  }

#if 0
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("mem: ");
    display.print(AudioMemoryUsageMax());
    display.print(" prx: ");
    display.print(paj_knee);
    display.setCursor(0, 24);
    display.print("Note: ");
    display.print(note);
    display.print(" n: ");
    display.print(loopno);
    display.display();
    return;
#endif


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

  if (note != last_note) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(note_name);
    display.display();
    last_note = note;
  }
}
