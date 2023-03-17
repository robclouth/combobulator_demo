#pragma once
#include "FilterProcessor.h"
#include "ModelProcessor.h"
#include "PitchShiftProcessor.h"

#include "ProcessorBase.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <string>
#include <vector>

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;

#define MAX_DELAY_SECONDS 11

class DelayProcessor : public ProcessorBase
{
public:
  inline static juce::String FeedbackParamId = "delayFeedback";
  inline static juce::String DelayTimeParamId = "delayTime";
  inline static juce::String EffectsOrderParamId = "effectOrder";

  DelayProcessor (AudioPluginAudioProcessor& pluginProcessor);

  const juce::String getName() const override { return "Delay"; }

  void prepareToPlay (double sampleRate_, int samplesPerBlock) override
  {
    sampleRate = sampleRate_;

    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };

    delayLine.prepare (spec);
    delayLine.setMaximumDelayInSamples (MAX_DELAY_SECONDS * sampleRate);

    filterProcessor.prepareToPlay (sampleRate, samplesPerBlock);
    pitchShiftProcessor.prepareToPlay (sampleRate, samplesPerBlock);
    modelProcessor.prepareToPlay (sampleRate, samplesPerBlock);
  }

  void releaseResources() override
  {
    filterProcessor.releaseResources();
    pitchShiftProcessor.releaseResources();
    modelProcessor.releaseResources();
  }

  void reset() override
  {
    filterProcessor.reset();
    pitchShiftProcessor.reset();
    modelProcessor.reset();
  }

  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters();

private:
  static juce::StringArray effectsOrders;

  double sampleRate;

  juce::AudioParameterBool* muteInput;

  FilterProcessor filterProcessor;
  PitchShiftProcessor pitchShiftProcessor;
  ModelProcessor modelProcessor;

  juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
};
