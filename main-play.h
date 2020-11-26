#pragma once

void playDrones() {
  for (int i = 0; i < NUM_DRONES; ++i) {
    float pitch = tuning.GetPitch(NOTE_D3);
    pitch /= 1 << i;          // Take down the appropriate number of octaves
    pitch *= (i - 1) / 1000;  // Detune ever so   AudioProcessorUsageMaxReset();
    Drones[i].NoteOn(pitch);
  }
}

void doPlay(bool forceDisplayUpdate) {
  static Note last_note = NOTE_ZERO;
  bool updateDisplay = forceDisplayUpdate;

  if (updateDisplay) {
    display.clearDisplay();
    display.fillRect(0, 0, 2, 2, SSD1306_WHITE);
    display.display();
  }

  if (pipe.silent) {
    Chanter.NoteOff();
  } else {
    // Calculate pitch, and glissando pitch
    float pitch = tuning.GetPitch(pipe.note);
    float glissandoPitch = tuning.GetPitch(pipe.glissandoNote);

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

  if (pipe.note != last_note) {
    updateDisplay = true;
  }

#if 0
  if (updateDisplay) {
    // Look up the note name
    const char *note_name = NoteName(pipe.note);
    if (pipe.silent) {
      note_name = "--";
      updateDisplay = true;
    }

    display.clearDisplay();
    display.setFont(&FreeSans9pt7b);

    display.setCursor(0, 16);
    display.print(note_name);

    display.display();
    last_note = pipe.note;
  }
#endif
}
