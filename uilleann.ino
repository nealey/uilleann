#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <Fonts/FreeSans9pt7b.h>
#include <stdio.h>

#include "patches.h"
#include "pipe.h"
#include "synth.h"
#include "tuning.h"

const char *buildDate = __DATE__;

#include <Adafruit_NeoTrellisM4.h>
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();


Pipe pipe;
Tuning tuning = Tuning(NOTE_D4, PITCH_CONCERT_D4, TUNINGSYSTEM_JUST);
Adafruit_SSD1306 display(128, 32, &Wire, -1);


// Settings
#define VOLUME_INITIAL 0.8
uint8_t patch[4] = {0};
float volume[5] = {VOLUME_INITIAL, VOLUME_INITIAL, VOLUME_INITIAL, VOLUME_INITIAL, 0.5};

// Pipes
#define NUM_DRONES 3
#define NUM_REGULATORS 3
FMVoice Chanter;
FMVoice Drones[NUM_DRONES];
FMVoice Regulators[NUM_REGULATORS];

AudioFilterBiquad biquad1;
AudioMixer4 mixDrones;
AudioMixer4 mixRegulators;
AudioMixer4 mixL;
AudioMixer4 mixR;
AudioSynthNoiseWhite noise;

AudioOutputAnalogStereo out1;

AudioConnection FMVoicePatchCords[] = {
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

void blink(int count, bool forever) {
  for (;;) {
    for (int i = 0; i < count; i++) {
      digitalWrite(LED_BUILTIN, true);
      delay(200);
      digitalWrite(LED_BUILTIN, false);
      delay(200);
    }
    if (!forever) {
      return;
    }
    delay(1200);
  }
}

void diag(const char *fmt, ...) {
  va_list args;
  char s[80];

  va_start(args, fmt);
  vsnprintf(s, sizeof(s) - 1, fmt, args);
  va_end(args);

  display.clearDisplay();
  display.drawRect(124, 16, 4, 16, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setFont();
  display.setTextSize(1);

  display.setCursor(56, 24);
  display.print(buildDate);

#if 0
  display.setCursor(0, 16);
  display.print(fn);
  display.print(":");
  display.print(lineno);
#endif

  display.setCursor(0, 0);
  display.print(s);

  display.display();
}

// The right way to do this would be to make a Uilleann object,
// and pass that around.
// The Auido library makes this sort of a pain,
// and honestly, is anybody other than me going to use this?
#include "main-play.h"
#include "main-setup.h"

void setup() {
  // PREPARE TO BLINK
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  // Set up I2C. Apparently this needs a bit of startup delay.
  Wire.begin();

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    blink(2, true);
  }
  digitalWrite(LED_BUILTIN, false);
  diag("Hello!");

  diag("Trellis...");
  trellis.begin();

  diag("Pipe...");
  while (!pipe.Init()) {
    diag("Pipe connected?");
    blink(3, false);
  }

  diag("Audio...");
  AudioMemory(120);
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  diag("Synth...");
  loadPatch(0);
  loadPatch(1);
  loadPatch(2);
  noise.amplitude(0.0);

  diag("Mixer...");
  // Turn on all mixer channels
  for (int i = 0; i < 4; i++) {
    mixL.gain(i, volume[i]);
    mixR.gain(i, volume[i]);
  }
  for (int i = 0; i < NUM_REGULATORS; ++i) {
    mixRegulators.gain(i, 1);
  }
  for (int i = 0; i < NUM_DRONES; ++i) {
    mixDrones.gain(i, 1);
  }
  biquad1.setNotch(0, PITCH_CONCERT_A4, 0.001);

  diag("Drones...");
  playDrones();

  diag("Done!");
  display.dim(true);
}

void loadPatch(uint8_t where) {
  FMPatch *p = &Bank[where];

  switch (where) {
    case 0:
      Chanter.LoadPatch(p);
      break;
    case 1:
      for (int i = 0; i < NUM_REGULATORS; ++i) {
        Regulators[i].LoadPatch(p);
      }
      break;
    case 2:
      for (int i = 0; i < NUM_DRONES; ++i) {
        Drones[i].LoadPatch(p);
      }
      break;
    default:
      break;
  }
}


void loop() {
  static bool upSetting = true;  // GET IT?

  pipe.Update();
  trellis.tick();

  if (trellis.justPressed(0)) {
    upSetting = !upSetting;
  }

  if (upSetting) {
    trellis.setPixelColor(0, 0x200000);
    doSetup();
    return;
  }

  for (int i = 0; i < 12; i += 1) {
    trellis.setPixelColor(i, 0xffffff * pipe.KeyPressure[i]);
  }

  doPlay(upSetting);
  upSetting = false;
}
