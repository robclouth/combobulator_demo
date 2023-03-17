#include "DelayProcessor.h"
#include "../PluginProcessor.h"
#include "FilterProcessor.h"

juce::StringArray DelayProcessor::effectsOrders = { "Filter - Pitch shift", "Pitch shift - Filter" };

DelayProcessor::DelayProcessor (AudioPluginAudioProcessor& pluginProcessor_) :
  ProcessorBase (pluginProcessor_),
  filterProcessor (pluginProcessor_),
  pitchShiftProcessor (pluginProcessor_),
  modelProcessor (pluginProcessor_)

{
}

void DelayProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
  juce::dsp::AudioBlock<float> block (buffer);
  juce::dsp::ProcessContextReplacing<float> context (block);

  auto&& inBlock = context.getInputBlock();
  auto&& outBlock = context.getOutputBlock();

  jassert (inBlock.getNumChannels() == outBlock.getNumChannels());
  jassert (inBlock.getNumSamples() == outBlock.getNumSamples());

  auto len = inBlock.getNumSamples();
  auto numChannels = inBlock.getNumChannels();

  if (context.isBypassed)
  {
    if (context.usesSeparateInputAndOutputBlocks())
      outBlock.copyFrom (inBlock);

    return;
  }

  auto delayTimeBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (DelayProcessor::DelayTimeParamId).getReadPointer (0);
  auto feedbackBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (DelayProcessor::FeedbackParamId).getReadPointer (0);

  for (int i = 0; i < len; i++)
  {
    for (int ch = 0; ch < numChannels; ++ch)
    {
      auto* inData = inBlock.getChannelPointer (ch);
      auto* outData = outBlock.getChannelPointer (ch);

      auto delayedSample = delayLine.popSample (ch, delayTimeBufferPtr[i] * sampleRate);
      outData[i] = inData[i] + delayedSample * feedbackBufferPtr[i];
    }
  }

  auto effectsOrder = (int) appState.getRawParameterValue (DelayProcessor::EffectsOrderParamId)->load();
  if (effectsOrder == 0)
  {
    filterProcessor.processBlock (buffer, midiMessages);
    pitchShiftProcessor.processBlock (buffer, midiMessages);
  }
  else if (effectsOrder == 0)
  {
    pitchShiftProcessor.processBlock (buffer, midiMessages);
    filterProcessor.processBlock (buffer, midiMessages);
  }

  modelProcessor.processBlock (buffer, midiMessages);

  for (int i = 0; i < len; i++)
  {
    for (int ch = 0; ch < numChannels; ++ch)
    {
      auto* outData = outBlock.getChannelPointer (ch);
      delayLine.pushSample (ch, outData[i]);
    }
  }
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> DelayProcessor::getParameters()
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();

  for (auto& parameter : PitchShiftProcessor::getParameters())
  {
    parameters.add (std::move (parameter));
  }

  for (auto& parameter : FilterProcessor::getParameters())
  {
    parameters.add (std::move (parameter));
  }

  for (auto& parameter : ModelProcessor::getParameters())
  {
    parameters.add (std::move (parameter));
  }

  parameters.add (std::make_unique<juce::AudioParameterFloat> (DelayProcessor::FeedbackParamId,
    "Feedback",
    0.0f,
    2.0f,
    0.8f));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (DelayProcessor::DelayTimeParamId,
    "Delay time",
    0.0f,
    10.f,
    0.5f));

  parameters.add (std::make_unique<juce::AudioParameterChoice> (DelayProcessor::EffectsOrderParamId,
    "Effects order",
    effectsOrders,
    0));

  return parameters;
}
