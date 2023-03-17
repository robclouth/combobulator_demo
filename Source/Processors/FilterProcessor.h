#pragma once
#include "./ProcessorBase.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <string>
#include <vector>

// NOTES :
//
// - Original filter code by Paul Kellet (musicdsp.pdf)
//
// - Uses 4 first order filters in series, should give 24dB per octave
//
// - Now with P4 Denormal fix :)

// Recommended frequencies are ...
//
//  lowfreq  = 880  Hz
//  highfreq = 5000 Hz
//
// Set mixfreq to whatever rate your system is using (eg 48Khz)

class FilterProcessor : public ProcessorBase
{
public:
  inline static juce::String LowGainParamId = "filterLowGain";
  inline static juce::String MidGainParamId = "filterMidGain";
  inline static juce::String HighGainParamId = "filterHighGain";

  FilterProcessor (AudioPluginAudioProcessor& pluginProcessor);

  const juce::String getName() const override { return "Filter"; }

  void prepareToPlay (double sampleRate, int samplesPerBlock) override
  {
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };

    init_3band_state (880, 5000, (int) sampleRate);
  }

  void releaseResources() override
  {
  }

  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters();

private:
  typedef struct
  {
    // Filter #1 (Low band)

    double lf = 0; // Frequency
    double f1p0 = 0; // Poles ...
    double f1p1 = 0;
    double f1p2 = 0;
    double f1p3 = 0;

    // Filter #2 (High band)

    double hf = 0; // Frequency
    double f2p0 = 0; // Poles ...
    double f2p1 = 0;
    double f2p2 = 0;
    double f2p3 = 0;

    // Sample history buffer

    double sdm1 = 0; // Sample data minus 1
    double sdm2 = 0; //                   2
    double sdm3 = 0; //                   3

    // Gain Controls

    double lg = 0; // low  gain
    double mg = 0; // mid  gain
    double hg = 0; // high gain

  } EQState;

  // ---------
  //| Exports |
  // ---------

  void init_3band_state (int lowfreq, int highfreq, int mixfreq);
  float do_3band (EQState& state, float sample);

  EQState state[2];
};
