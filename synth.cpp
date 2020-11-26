#include "synth.h"
#include "synth_waveform.h"

void FMVoice::LoadPatch(FMPatch *p) {
  bool playing = this->playing;
  float pitch = this->pitch;

  NoteOff();
  for (int i=0; i<NUM_OPERATORS; i++) {
    FMOperator op = p->operators[i];

    this->oscillators[i].frequencyModulation(1);
    this->oscillators[i].begin(op.waveform);
    this->envelopes[i].delay(op.delayTime);
    this->envelopes[i].attack(op.attackTime);
    this->oscillators[i].amplitude(op.holdAmplitude);
    this->envelopes[i].hold(op.holdTime);
    this->envelopes[i].decay(op.decayTime);
    this->envelopes[i].sustain(op.sustainAmplitude / op.holdAmplitude);
    this->envelopes[i].release(op.releaseTime);

    // This feels wasteful 🙁
    for (int j=0; j<NUM_OPERATORS; j++) {
      this->mixers[i].gain(j, p->gains[i][j]);
    }
    this->outputMixer.gain(i, p->gains[i][NUM_OPERATORS]);
  }
  this->patch = p;
  if (playing) {
    NoteOn(pitch);
  }
}

void FMVoice::SetPitch(float freq) {
  for (int i=0; i<4; i++) {
    FMOperator op = this->patch->operators[i];
    this->oscillators[i].frequency(op.offset + (freq * op.multiplier));
  }
  this->pitch = freq;
}

void FMVoice::NoteOn(float freq) {
  SetPitch(freq);
  for (int i=0; i<4; i++) {
    this->envelopes[i].noteOn();
  }
  this->playing = true;
}

void FMVoice::NoteOff() {
  for (int i=0; i<4; i++) {
    this->envelopes[i].noteOff();
  }
  this->playing = false;
}
