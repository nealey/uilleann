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
#include "pipe.h"

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
#include <Adafruit_NeoTrellisM4.h>
Adafruit_NeoTrellisM4 trellis;// = Adafruit_NeoTrellisM4();
#endif

#define DRONES
#define DEBUG false

Pipe pipe;
Adafruit_SSD1306 display(128, 32, &Wire, -1);
int currentPatch = 0;

FMVoice Chanter;
FMVoice Drones[3];
FMVoice Regulators[3];

AudioFilterBiquad    biquad1;
AudioMixer4          mixDrones;
AudioMixer4          mixRegulators;
AudioMixer4          mixL;
AudioMixer4          mixR;
AudioSynthNoiseWhite noise;

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
AudioOutputAnalogStereo out1;
#else
AudioOutputI2S       out1;
#endif

AudioControlSGTL5000 sgtl5000;

AudioConnection FMVoicePatchCords[] = {
  //{0, 0, 0, 0}, // For some reason, the first one is ignored

//  {noise, 0, mixDrones, 3},
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
    if (! forever) break;
  }
}

void setup() {
  // Wire.begin needs a moment, so let's do some math.
  setupJustPitches(NOTE_D4, PITCH_D4);

  Wire.begin();

  // PREPARE TO BLINK
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    blink(true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Starting");
  display.display();

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
  trellis.begin();
#endif

  while (!pipe.Init()) {
     display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Pipe?");
    display.display();
    blink(false);
  }

  // Set aside some memory for the audio library  
  AudioMemory(20);

  // Set up the SGTL5000 using I2C
  sgtl5000.enable();
  sgtl5000.volume(0.3);

  // initialize tunables
  updateTunables(3, 0);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  // Turn on drones
  for (int i=0; i<3; i++) {
    Drones[i].LoadPatch(&Bank[0]);
    Drones[i].NoteOn(JustPitches[NOTE_D4 - 12*i] + i);
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

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Done!");
  display.display();
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
    Chanter.LoadPatch(p);
  }
}


void loop() {
  static uint8_t last_note = 0;
  bool updateDisplay = false;
  bool setupMode = false;

  pipe.Update();

#if defined(ADAFRUIT_TRELLIS_M4_EXPRESS)
  trellis.tick();

  trellis.setPixelColor(1, trellis.ColorHSV(millis(), 255, 120));
  trellis.setPixelColor(0, trellis.ColorHSV(64*pipe.kneeClosedness, 255, 120));
#endif
  
  if (false) {
    static uint16_t loopno = 0;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    // display.print("mem: ");
    // display.print(AudioMemoryUsageMax());
    // display.print(" prx: ");
    // display.print(pipe.kneeClosedness);
    // display.setCursor(0, 24);
    // display.print("Note: ");
    // display.print(pipe.note);
    // display.print(" n: ");
    display.print(loopno++);
    display.display();
  }

// If we're infinitely (for the sensor) off the knee,
// go into setup mode!
if (pipe.kneeClosedness == 0) {
  setupMode = true;
  updateDisplay = true;
}

  if (pipe.silent) {
    Chanter.NoteOff();
  } else {
    // Calculate pitch, and glissando pitch
    uint16_t pitch = JustPitches[pipe.note];
    uint16_t glissandoPitch = JustPitches[pipe.glissandoNote];

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

  // // Look up the note name
    const char *note_name = NoteNames[pipe.note % 12];
    if (pipe.silent) {
      note_name = "--";
      updateDisplay = true;
    }

  if (pipe.note != last_note) {
    updateDisplay = true;
  }

  if (updateDisplay) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(Chanter.patch->name);
 
    if (setupMode) {
      // THE SETUP DONUT
      display.fillCircle(128-8, 16+8, 4, SSD1306_WHITE);
      display.fillCircle(128-8, 16+8, 2, SSD1306_BLACK);
    } else {
      display.setCursor(0, 8);
      display.print(note_name);
    }

    display.display();
   last_note = pipe.note;
  }
}
