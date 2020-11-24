#include <Adafruit_GFX.h>
#include <Adafruit_MPR121.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SparkFun_Qwiic_Button.h>
#include <Wire.h>
#include <paj7620.h>
#include <stdio.h>

#include "patches.h"
#include "pipe.h"
#include "synth.h"
#include "tuning.h"

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
#include <Adafruit_NeoTrellisM4.h>
Adafruit_NeoTrellisM4 trellis;  // = Adafruit_NeoTrellisM4();
#endif

Pipe pipe;
Tuning tuning = Tuning(NOTE_D4, PITCH_CONCERT_D4, TUNINGSYSTEM_JUST);

Adafruit_SSD1306 display(128, 32, &Wire, -1);
int currentPatch = 0;

// Settings
uint8_t intonation = 0;
int16_t pitchAdjust = 0;
uint8_t patch[4] = {0};
float volume[4] = {0};
const char *settingNames[4] = {"c", "r", "d", "*"};

// Pipes
FMVoice Chanter;
FMVoice Drones[3];
FMVoice Regulators[3];

AudioFilterBiquad biquad1;
AudioMixer4 mixDrones;
AudioMixer4 mixRegulators;
AudioMixer4 mixL;
AudioMixer4 mixR;
AudioSynthNoiseWhite noise;

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
AudioOutputAnalogStereo out1;
#else
AudioOutputI2S out1;
#endif

AudioControlSGTL5000 sgtl5000;

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

void blink(bool forever) {
  for (;;) {
    digitalWrite(LED_BUILTIN, true);
    delay(200);
    digitalWrite(LED_BUILTIN, false);
    delay(200);
    if (!forever) {
      return;
    }
  }
}

void diag(const char *fmt, ...) {
  va_list args;
  char s[80];

  va_start(args, fmt);
  vsnprintf(s, sizeof(s)-1, fmt, args);
  va_end(args);

  display.clearDisplay();
  display.drawRect(124, 16, 4, 16, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(56, 24);
  display.print(__DATE__);

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

void setup() {
  // Initialize settings
  // XXX: Read these from persistent storage later
  for (int i = 0; i < 4; i++) {
    patch[i] = 0;
    volume[i] = 0.75;
  }

  Wire.begin();

  // PREPARE TO BLINK
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    blink(true);
  }
  diag("Hello!");

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
  diag("Trellis...");
  trellis.begin();
#endif

  diag("Pipe...");
  while (!pipe.Init()) {
    diag("No pipe. Is it connected?");
    blink(false);
  }

  diag("Audio...");
  // Set aside some memory for the audio library
  AudioMemory(20);

  // Set up the SGTL5000 using I2C
  sgtl5000.enable();
  sgtl5000.volume(0.3);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  diag("Drones...");
  // Turn on drones
  for (int i = 0; i < 3; i++) {
    Note note = NOTE_D4 - (NOTE_OCTAVE * i);
    float pitch =
        tuning.GetPitch(note) * (0.01 * (i - 1));  // Detune just a touch
    Drones[i].LoadPatch(&Bank[0]);
    Drones[i].NoteOn(pitch);
  }

  // Turn on all mixer channels
  for (int i = 0; i < 4; i++) {
    mixL.gain(i, 0.5);
    mixR.gain(i, 0.6);
  }

  diag("Done!");
}

void loop() {
  pipe.Update();
  diag("loop %d", millis());

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
  trellis.tick();

  trellis.setPixelColor(1, trellis.ColorHSV(millis(), 255, 120));
  {
    uint16_t color = trellis.ColorHSV(64 * pipe.kneeClosedness, 255, 120);
    trellis.setPixelColor(0, color);
  }
#endif

  // If we're infinitely (for the sensor) off the knee,
  // go into setup mode!
  if (pipe.kneeClosedness == 0) {
    doSetup();
  } else {
    doPlay();
  }
}

/** doSetup performs "setup mode" behavior for the pipe.
 *
 * Setup mode sets the following new meanings to the buttons:
 *
 *  key: function [alternate]
 * C♯:  Alt
 * B♮: Chanter
 * A♮: Regulators
 * G♮: Drones
 * F♯: Up [+ coarse]
 * E♮: Down [- coarse]
 * E♭: + [+ fine]
 * D♮: - [- fine]
 *
 */
void doSetup() {
  display.clearDisplay();

  bool alt = pipe.Pressed(7);

  // Draw indicator bar
  display.fillRect(126, 0, 2, 32, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (alt) {
    // Show settings for each of Chanter, Regulators, Drones
    for (int i = 0; i < 3; i++) {
      int p = patch[i];
      int16_t x = 0;
      int16_t y = i * 8;

      display.setCursor(x, y);
      if (pipe.Pressed(6 - i)) {
        display.fillRect(x - 1, y, 8, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.print(settingNames[i]);
      x += 7;

      display.drawRect(x, y + 2, 32, 4, SSD1306_WHITE);
      display.fillRect(x, y + 2, 32 * volume[i], 4, SSD1306_WHITE);

      x += 34;
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(x, y);
      if (p < 10) {
        display.print(" ");
      }
      display.print(p);
      display.print(" ");
      display.print(Bank[p].name);
    }
  } else {
    if (pipe.Pressed(6)) {
      float freq = PITCH_CONCERT_D4 + pitchAdjust;
      Note note = NearestNote(freq);

      display.setCursor(0, 0);
      display.print(NoteName(note));
      display.setCursor(6 + 6, 0);
      display.print(freq);
    } else if (pipe.Pressed(5)) {
      display.print("fn2");
    } else if (pipe.Pressed(4)) {
      display.print("fn3");
    } else {
      display.setCursor(56, 8);
      display.setTextSize(2);
      display.print("Setup");
    }
  }

  display.display();
}

void doPlay() {
  static uint8_t last_note = 0;
  bool updateDisplay = false;

  if (pipe.silent) {
    Chanter.NoteOff();
  } else {
    // Calculate pitch, and glissando pitch
    uint16_t pitch = tuning.GetPitch(pipe.note);
    uint16_t glissandoPitch = tuning.GetPitch(pipe.glissandoNote);

    // Bend pitch if fewer than 3 half steps away
    if (abs(pipe.glissandoNote - pipe.note) < 3) {
      float diff = glissandoPitch - pitch;
      pitch += diff * pipe.glissandoOpenness;
    }

    // Apply a low shelf filter if this is the alternate fingering
    if (pipe.altFingering) {
      biquad1.setLowShelf(0, 2000, 0.2, 1);
    } else {
      biquad1.setHighShelf(0, 1000, 1.0, 1);
    }

    // We've figured out what pitch to play, now we can play it.
    if (Chanter.playing) {
      Chanter.SetPitch(pitch);
    } else {
      Chanter.NoteOn(pitch);
    }
  }

  // Look up the note name
  const char *note_name = NoteName(pipe.note);
  if (pipe.silent) {
    note_name = "--";
    updateDisplay = true;
  }

  if (pipe.note != last_note) {
    updateDisplay = true;
  }

  if (updateDisplay) {
    display.clearDisplay();

    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print(Chanter.patch->name);

    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print(note_name);

    display.setCursor(40, 0);
    display.setTextSize(1);
    display.print(pipe.kneeClosedness);

    display.display();
    last_note = pipe.note;
  }
}
