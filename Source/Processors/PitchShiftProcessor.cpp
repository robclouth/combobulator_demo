#include "PitchShiftProcessor.h"
#include "../PluginProcessor.h"

PitchShiftProcessor::PitchShiftProcessor (AudioPluginAudioProcessor& pluginProcessor_) :
  ProcessorBase (pluginProcessor_)

{
  pitchOffsetParameter = appState.getRawParameterValue (PitchShiftProcessor::PitchOffsetParamId);
}

void PitchShiftProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
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

  //TODO: Make modulation sample accurate

  auto pitchOffsetBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (PitchShiftProcessor::PitchOffsetParamId).getReadPointer (0);
  stretch.setTransposeSemitones (pitchOffsetBufferPtr[0] / 100);

  float* inData[2] = { const_cast<float*> (inBlock.getChannelPointer (0)), const_cast<float*> (inBlock.getChannelPointer (1)) };
  float* outData[2] = { const_cast<float*> (outBlock.getChannelPointer (0)), const_cast<float*> (outBlock.getChannelPointer (1)) };

  stretch.process (inData, len, outData, len);

  // Inspect latency
  int totalLatency = stretch.inputLatency() + stretch.outputLatency();
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> PitchShiftProcessor::getParameters()
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();
  parameters.add (std::make_unique<juce::AudioParameterFloat> (PitchShiftProcessor::PitchOffsetParamId,
    "Pitch offset",
    -1200.f,
    1200.f,
    0.f));

  return parameters;
}
