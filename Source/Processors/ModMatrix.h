#pragma once

#include "../Defines.h"
#include "LFOProcessor.h"
#include "ProcessorBase.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class AudioPluginAudioProcessor;

class ModMatrix : public ProcessorBase
{
public:
  struct Parameter
  {
    juce::AudioBuffer<float> buffer;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValue;
  };

  ModMatrix (AudioPluginAudioProcessor& pluginProcessor_);

  void prepareToPlay (double sampleRate_, int samplesPerBlock_) override;

  void releaseResources() override;

  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  juce::AudioBuffer<float>& getModulatedParameterValueBuffer (juce::String parameterID);

  inline static juce::String ModAmountParamId = "modAmount";
  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters (juce::String suffix = "");

  LFOProcessor lfos[NUM_LFOS];

private:
  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getAllModulatableParameters();

  static juce::String getLFOModAmountParameterName (int index, juce::String parameterID)
  {
    return ModMatrix::ModAmountParamId + "_LFO" + juce::String (index + 1) + "_" + parameterID;
  }

  std::unordered_map<juce::String, Parameter> parameterMap;

  double sampleRate;
  int samplesPerBlock;

  double smoothTimeSeconds = 0.05;
};