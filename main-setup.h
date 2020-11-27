#pragma once

#define QUELL_DURATION 200
#define ADJ_TYPEMATIC_DELAY 500
#define ADJ_TYPEMATIC_REPEAT 33

#define VOLUME_INITIAL 0.8

const char *settingNames[4] = {"c", "r", "d", "*"};

// quellUntil can be set to give the user time to get their fingers off the continuous adjustment buttons.
unsigned long quellUntil = 0;
void quell(unsigned long ms) {
  quellUntil = millis() + ms;
}
void quell() {
  quell(QUELL_DURATION);
}

void setupVolume() {
  Adjust volAdjust = pipe.ReadAdjust(2, 3, 0, ADJ_TYPEMATIC_REPEAT);
  Adjust patchAdjust = pipe.ReadAdjust(0, 1, 0, 500);

  for (int i = 0; i < 3; i++) {
    int16_t x = 1;
    int16_t y = i * 8;

    display.setCursor(x, y);
    if (pipe.Pressed(6 - i)) {
      display.fillRect(x - 1, y, 8, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      switch (volAdjust) {
        case ADJUST_BOTH:
          volume[i] = VOLUME_INITIAL;
          quell();
          break;
        case ADJUST_UP:
        case ADJUST_DOWN:
          {
            float vol = volume[i] + float(volAdjust)*0.02;
            volume[i] = min(max(vol, 1.0), 0.0);
          }
          break;
        default:
          break;
      }
      switch (patchAdjust) {
        case ADJUST_BOTH:
          patch[i] = 0;
          quell();
          loadPatch(i);
          break;
        case ADJUST_UP:
        case ADJUST_DOWN:
          patch[i] = (patch[i] + PATCH_MAX + int(patchAdjust)) % PATCH_MAX;
          loadPatch(i);
          break;
        default:
          break;
      }
      mixL.gain(i, volume[i]);
      mixR.gain(i, volume[i]);
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
    display.print(patch[i]);
    display.print(" ");
    display.print(Bank[patch[i]].name);
  }
}

void setupTuning() {
  Adjust noteAdjust = pipe.ReadAdjust(2, 3, ADJ_TYPEMATIC_DELAY, ADJ_TYPEMATIC_REPEAT);
  Adjust pitchAdjust = pipe.ReadAdjust(0, 1, 0, ADJ_TYPEMATIC_REPEAT);
  TuningSystem system = tuning.GetTuningSystem();
  float freq = tuning.GetPitch(NOTE_D4);
  Note note = NearestNote(freq);

  if (noteAdjust != ADJUST_NONE) {
    // Set up even temperament to pick a concert pitch
    tuning.Setup(NOTE_A4, PITCH_CONCERT_A4, TUNINGSYSTEM_EQUAL);
    switch (noteAdjust) {
      case ADJUST_BOTH:
        ++system;
        break;
      case ADJUST_UP:
      case ADJUST_DOWN:
        note += int(noteAdjust);
        freq = tuning.GetPitch(note);
        break;
      default:
        break;
    }
    // Now retune
    tuning.Setup(NOTE_D4, freq, system);
  }

  if (pitchAdjust != ADJUST_NONE) {
    switch (pitchAdjust) {
      case ADJUST_BOTH:
        freq = PITCH_CONCERT_D4;
        quell();
        break;
      case ADJUST_UP:
        freq *= 1.001;
        break;
      case ADJUST_DOWN:
        freq /= 1.001;
        break;
      default:
        break;
    }
    tuning.Setup(NOTE_D4, freq);
    note = NearestNote(freq);
  }

  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 12);
  display.print(NoteName(note));
  display.setCursor(24, 12);
  display.print(NoteOctave(note));
  display.setCursor(48, 12);
  display.print(freq);

  display.setCursor(0, 27);
  display.print(TuningSystemName(system));
}

void setupInfo() {
  display.setFont(&FreeSans9pt7b);
  display.setCursor(64, 18);
  display.print("Setup");

  display.setFont();
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print("FC-1");
  display.setCursor(0, 24);
  display.print(buildDate);
  display.setCursor(0, 0);
  display.print("M:");
  display.print(AudioMemoryUsageMax());
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
  if (millis() < quellUntil) {
    return;
  }

  // Draw setup indicator bar
  display.clearDisplay();
  display.fillRect(126, 0, 2, 32, SSD1306_WHITE);
  display.setFont(0);
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (pipe.Pressed(7)) {  // Volume
    setupVolume();
  } else if (pipe.Pressed(4)) {
    display.print("fn3");
  } else if (pipe.Pressed(5)) {
    display.print("fn2");
  } else if (pipe.Pressed(6)) {  // Tuning
    setupTuning();
  } else {
    setupInfo();
  }

  display.display();
}
