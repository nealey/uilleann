#pragma once
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "synth_waveform.h"

#define NUM_OPERATORS 4

/** FMOperator defines all settable paramaters to an operator.
 *
 * An FM operator consists of:
 * - An input
 * - An oscillator
 * - An envelope generator
 *
 * Frequency Modulation happens by chaining oscillators together,
 * using the output of one to modulate the frequency of the next.
 *
 * Oscillators generate waveforms in a shape defined by
 * `synth_waveform.h`. WAVEFORM_SINE is a good one to start with.
 * Other sensible options are SAWTOOTH, SQUARE, and TRIANGLE.
 *
 * Frequency for an oscillator is calculated with:
 *     offset + (voiceFrequency × multiplier)
 *
 * Oscillator frequency is then modulated by the level obtained
 * by the input mixer: level of 1.0 shifts frequency up by
 * 8 octaves, level of -1.0 shifts frequency down by 8 octaves.
 *
 * The envelope modifies amplitude of the oscillator output,
 * using the following rules:
 * - stay at 0 until Note On
 * - stay at 0 for `delayTime` milliseconds
 * - linear increase to `holdAmplitude` for `attackTime` milliseconds
 * - stay at `holdAmplitude` for `holdTime` milliseconds
 * - linear decrease to `sustainAmplitude` for `decayTime` milliseconds
 * - stay at `sustainAmplitude` until Note Off
 * - linear decrease to 0 for `releaseTime` milliseconds
 */
typedef struct FMOperator {
  // Oscillator
  short waveform;
  float offset;
  float multiplier;

  // Envelope
  float delayTime;
  float attackTime;
  float holdAmplitude;
  float holdTime;
  float decayTime;
  float sustainAmplitude;
  float releaseTime;
} FMOperator;

/** FMPatch defines all parameters to a voice patch.
 *
 * This defines the "sound" of an FM voice,
 * just like a "Patch" does in a hardware synthesizer.
 * I think of a "patch" being the physical cables that
 * connect oscillators together, and to the output mixer.
 *
 * Each operator has NUM_OPERATORS input gains,
 * one output gain (to the voice output mixer),
 * and NUM_OPERATORS operators.
 *
 * Historical FM synthisizers,
 * such as the DX7 or DX9,
 * used "algorithms" to patch operators into one another:
 * this is done with 0.0 or 1.0 values to the gains.
 * The "feedback" on operator 4 of the DX9
 * can be accomplished by patching an operator into itself.
 */
typedef struct FMPatch {
  char *name;
  float gains[NUM_OPERATORS][NUM_OPERATORS+1];
  FMOperator operators[NUM_OPERATORS];
} FMPatch;

/** FMVoice sets up all the Audio objects for a voice.
 */
class FMVoice {
  public:
    /** LoadPatch loads a patch into a voice.
     */
    void LoadPatch(FMPatch *p);

    /** SetPitch sets the pitch (Hz) of a voice.
     *
     * This does not signal the envelope in any way.
     * You would use this for a glissando, portamento, or pitch bend.
     * In my bagpipe, this prevents "reed noise" when changing notes.
     */
    void SetPitch(float pitch);

    /** SetModulation sets the modulation amount of a voice.
     * 
     * What this means depends on the loaded patch.
     * For  a "normal" bagpipe patch, this would adjust the intensity of
     * of a filter, or set the level of an oscillator.
     * In an old-school keyboard patch, this would set the
     * intensity of a Low Frequency Oscillator to set a vibrato.
     */
    void setModulation(float level);

    /** NoteOn sets the pitch (Hz) of a voice, and starts in playing.
     *
     * This tells the envelope generators to begin.
     * On a piano, this is what you would use when a key is pressed.
     * In my bagpipe, this triggers "reed noise".
     */
    void NoteOn(float pitch);

    /** NoteOff stops a note from playing.
     *
     * This turns the voice "off" by shutting down all the envelope generators.
     * On a piano, this is what you would use when a key is released.
     * In my bagpipe, this corresponds to all holes being closed.
     */
    void NoteOff();


    AudioMixer4 mixers[NUM_OPERATORS];
    AudioSynthWaveformModulated oscillators[NUM_OPERATORS];
    AudioEffectEnvelope envelopes[NUM_OPERATORS];
    AudioMixer4 outputMixer;
    FMPatch *patch;
    bool playing;
};

/** FMOperatorWiring outputs AudioConnection initializers to wire one FM Operator
 */
#define FMOperatorWiring(name, i) \
  {name.mixers[i], 0, name.oscillators[i], 0}, \
  {name.oscillators[i], 0, name.envelopes[i], 0}, \
  {name.envelopes[i], 0, name.mixers[0], i}, \
  {name.envelopes[i], 0, name.mixers[1], i}, \
  {name.envelopes[i], 0, name.mixers[2], i}, \
  {name.envelopes[i], 0, name.mixers[3], i}, \
  {name.envelopes[i], 0, name.outputMixer, i}

/** FMVoiceWiring outputs AudioConnection initializer to wire one FMVoice
 */
#define FMVoiceWiring(name) \
  FMOperatorWiring(name, 0), \
  FMOperatorWiring(name, 1), \
  FMOperatorWiring(name, 2), \
  FMOperatorWiring(name, 3)


