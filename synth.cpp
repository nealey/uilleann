#include "synth.h"
#include "synth_waveform.h"

void FMVoiceLoadPatch(FMVoice *v, FMPatch *p) {
  for (int i=0; i<NUM_OPERATORS; i++) {
    FMOperator op = p->operators[i];

    v->oscillators[i].begin(op.waveform);
    v->envelopes[i].delay(op.delayTime);
    v->envelopes[i].attack(op.attackTime);
    v->oscillators[i].amplitude(op.holdAmplitude);
    v->envelopes[i].hold(op.holdTime);
    v->envelopes[i].decay(op.decayTime);
    v->envelopes[i].sustain(op.sustainAmplitude / op.holdAmplitude);
    v->envelopes[i].release(op.releaseTime);

    // This feels wasteful 🙁
    for (int j=0; j<NUM_OPERATORS; j++) {
      v->mixers[i].gain(j, p->gains[i][j]);
    }
    v->outputMixer.gain(i, p->gains[i][NUM_OPERATORS]);
  }
  v->patch = p;
}

void FMVoiceSetPitch(FMVoice *v, float freq) {
  for (int i=0; i<4; i++) {
    FMOperator op = v->patch->operators[i];
    v->oscillators[i].frequency(op.offset + (freq * op.multiplier));
  }
}

void FMVoiceNoteOn(FMVoice *v, float freq) {
  FMVoiceSetPitch(v, freq);
  for (int i=0; i<4; i++) {
    v->envelopes[i].noteOn();
  }
}

void FMVoiceNoteOff(FMVoice *v) {
  for (int i=0; i<4; i++) {
    v->envelopes[i].noteOff();
  }
}
