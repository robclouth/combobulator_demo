#pragma once
#include "./ProcessorBase.h"
#include "signalsmith-stretch/signalsmith-stretch.h";
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <string>
#include <vector>

using Stretch = signalsmith::stretch::SignalsmithStretch<float>;

class PitchShiftProcessor : public ProcessorBase
{
public:
  inline static juce::String PitchOffsetParamId = "pitchOffset";

  static constexpr int blockSize = 2048;

  PitchShiftProcessor (AudioPluginAudioProcessor& pluginProcessor);

  const juce::String getName() const override { return "Pitch shift"; }

  void prepareToPlay (double sampleRate, int samplesPerBlock) override
  {
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };

    stretch.configure (2, blockSize, blockSize / 4);
  }

  void releaseResources() override
  {
  }

  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters();

private:
  Stretch stretch;

  std::atomic<float>* pitchOffsetParameter = nullptr;
};
