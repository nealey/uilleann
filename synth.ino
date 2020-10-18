#include <Audio.h>
#include <Wire.h>
#include <Adafruit_NeoTrellisM4.h>
#include <SFE_MicroOLED.h>
#include <Adafruit_MPR121.h>
#include "synth.h"
#include "patches.h"
#include "notes.h"
#include "fingering.h"

#define KNEE_OFFSET 0
#define KEY_OFFSET 2

float cmaj_low[8] = { 130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94, 261.63 };
float cmaj_high[8] = { 261.6, 293.7, 329.6, 349.2, 392.0, 440.0, 493.9, 523.3 };

AudioEffectEnvelope *envs[] = {&env1, &env2, &env3, &env4};
AudioSynthWaveformSineModulated *oscs[] = {&osc1, &osc2, &osc3, &osc4};

int currentPatch = 0;

Adafruit_MPR121 cap = Adafruit_MPR121();
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
MicroOLED oled(9, 1);

// Hat tip to Kyle Gann
// https://www.kylegann.com/tuning.html
float JustPitches[MaxNote + 1];
void setupJustPitches(uint8_t baseNote, float basePitch) {
  JustPitches[baseNote +  0] = basePitch *  1 /  1;
  JustPitches[baseNote +  1] = basePitch * 16 / 15;
  JustPitches[baseNote +  2] = basePitch *  9 /  8;
  JustPitches[baseNote +  3] = basePitch *  6 /  5;
  JustPitches[baseNote +  4] = basePitch *  5 /  4;
  JustPitches[baseNote +  5] = basePitch *  4 /  3;
  JustPitches[baseNote +  6] = basePitch * 45 / 32;
  JustPitches[baseNote +  7] = basePitch *  3 /  2;
  JustPitches[baseNote +  8] = basePitch *  8 /  5;
  JustPitches[baseNote +  9] = basePitch *  5 /  3;
  JustPitches[baseNote + 10] = basePitch *  9 /  5;
  JustPitches[baseNote + 11] = basePitch * 15 /  8;

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

void loadPatch(Patch p) {
  for (int i=0; i<4; i++) {
    Operator op = p.operators[i];

    oscs[i]->amplitude(op.gain);
    envs[i]->delay(op.delay);
    envs[i]->attack(op.attack);
    envs[i]->hold(op.hold);
    envs[i]->decay(op.decay);
    envs[i]->sustain(op.sustain);
    envs[i]->release(op.release);
  }
  feedback.gain(0, p.feedback);
}

void setup(){
  setupJustPitches(NOTE_D4, PITCH_D4);
  pinMode(LED_BUILTIN, OUTPUT);

  // Wire.begin needs a moment
  delay(100);
  Wire.begin();

  // Initialize OLED display
  oled.begin();
  oled.clear(ALL);

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

  // Set up high shelf filter, for vibrato effects
  biquad1.setHighShelf(0, 1100, 1.0, 1);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

void setPitch(float freq) {
  for (int i=0; i<4; i++) {
    Operator op = Bank[currentPatch].operators[i];
    oscs[i]->frequency(op.baseFrequency + freq*op.multiplier);
  }
}

void noteOn(float freq) {
  AudioNoInterrupts();
  for (int i=0; i<4; i++) {
    Operator op = Bank[currentPatch].operators[i];
    oscs[i]->frequency(op.baseFrequency + freq*op.multiplier);
    envs[i]->noteOn();
  }
  AudioInterrupts();
}

void noteOff() {
  AudioNoInterrupts();
  for (int i=0; i<4; i++) {
    envs[i]->noteOff();
  }
  AudioInterrupts();
}

#define BUTTON_UP 0
#define BUTTON_DOWN 8
#define BUTTON_PATCH 12
#define BUTTON_PITCH 14
#define BUTTON_VOLUME 15

const uint8_t CLOSEDVAL=0x30;
const uint8_t OPENVAL=0x70;

#define INIT_PITCH_ADJUST 0
#define INIT_GAIN 0.1
#define INIT_PATCH 0

int16_t pitchAdjust;
float gain;
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
      gain = INIT_GAIN;
      break;
    case 2:
      gain = min(gain+0.005, 1.0);
      break;
    case 1:
      gain = max(gain-0.005, 0.0);
      break;
    }
  }

  mixL.gain(0, gain);
  mixR.gain(0, gain);
  trellis.setPixelColor(BUTTON_VOLUME, trellis.ColorHSV(uint16_t(gain * 65535), 255, 80));

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

    Patch p = Bank[patch];
    loadPatch(p);

    oled.clear(PAGE);
    oled.setFontType(0);
    oled.setCursor(0, 0);
    oled.print(p.name);
    oled.setCursor(0, 10);
    oled.print("Patch ");
    oled.print(patch);
    oled.display();
  }
}

bool playing = false;

void loop() {
  uint8_t keys = 0;
  uint8_t note;
  uint8_t glissandoKeys = 0;
  uint8_t gnote;
  uint8_t gaffinity = 0;
  bool bag = false;
  bool silent = false;
  bool knee = cap.filteredData(KNEE_OFFSET) < CLOSEDVAL;
  uint8_t buttons = trellis.isPressed(BUTTON_DOWN)?1:0 | trellis.isPressed(BUTTON_UP)?2:0;

  trellis.tick();

  for (int i = 0; i < 8; i++) {
    uint16_t val = cap.filteredData(i+KEY_OFFSET);
    uint8_t c = 0;

    if (val < OPENVAL) {
      bitSet(keys, i);
      c = 7;

      // If they're just sort of touching it, we're doing a glissando!
      if (val > CLOSEDVAL) {
        int aff = val - CLOSEDVAL;
        
        gaffinity = max(gaffinity, aff);
        c = 7 - (7 * aff / (OPENVAL - CLOSEDVAL));
      } else {
        bitSet(glissandoKeys, i);
      }
    }
    // print key states
    trellis.setPixelColor(7 - i, 1 << c);
  }
  
  note = uilleann_matrix[keys];
  gnote = uilleann_matrix[glissandoKeys];

  bool alt = note & 0x80;
  bool galt = gnote & 0x80;
  note = note & 0x7f;
  gnote = gnote & 0x7f;


  // All keys closed + knee = no sound
  if (knee) {
    if (keys == 0xff) {
      silent = true;
    }
  }

  // Jump octave if the bag is squished
  //bag = !digitalRead(BAG);
  if (bag) {
    if (keys & bit(7)) {
      note += 12;
      gnote += 12;
    }
  }

  // Read some trellis button states
  if (buttons) {
    updateTunables(buttons, note);
  }

  if (silent) {
    noteOff();
    playing = false;
  } else {
    // Calculate pitch, and glissando pitch
    uint16_t pitch = JustPitches[note];
    uint16_t gpitch = JustPitches[gnote];

    if (alt) {
      biquad1.setHighShelf(0, 1000, 0.5, 1);
    } else {
      biquad1.setHighShelf(0, 1000, 1.0, 1);
    }
  
    // Bend pitch
    if (gaffinity && (abs(gnote - note) < 3)) {
      uint32_t sum = (pitch * (OPENVAL-CLOSEDVAL)) + (gpitch * gaffinity);
      pitch = sum / ((OPENVAL-CLOSEDVAL) + gaffinity);
    }

    if (playing) {
      setPitch(pitch);
    } else {
      noteOn(pitch);
    }
    playing = true;
  }
}
