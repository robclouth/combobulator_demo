#include "ModMatrix.h"
#include "../PluginProcessor.h"

ModMatrix::ModMatrix (AudioPluginAudioProcessor& pluginProcessor) :
  ProcessorBase (pluginProcessor),
  lfos { LFOProcessor (pluginProcessor, 0), LFOProcessor (pluginProcessor, 1), LFOProcessor (pluginProcessor, 2), LFOProcessor (pluginProcessor, 3), LFOProcessor (pluginProcessor, 4) }
{
  for (auto& parameter : ModMatrix::getAllModulatableParameters())
  {
    parameterMap[parameter->getParameterID()] = Parameter { juce::AudioBuffer<float>(), juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>() };
  }
}

void ModMatrix::prepareToPlay (double sampleRate_, int samplesPerBlock_)
{
  sampleRate = sampleRate_;
  samplesPerBlock = samplesPerBlock_;

  juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 1 };

  for (auto& it : parameterMap)
  {
    auto& buffer = parameterMap.at (it.first).buffer;
    buffer.setSize (1, samplesPerBlock);

    auto& smoothedValue = parameterMap.at (it.first).smoothedValue;
    smoothedValue.reset (sampleRate, smoothTimeSeconds);
  }

  for (int i = 0; i < NUM_LFOS; ++i)
  {
    lfos[i].prepareToPlay (sampleRate, samplesPerBlock);
  }
}

void ModMatrix::releaseResources()
{
  for (int i = 0; i < NUM_LFOS; ++i)
  {
    lfos[i].releaseResources();
  }
}

void ModMatrix::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
  for (int i = 0; i < NUM_LFOS; ++i)
  {
    lfos[i].processBlock (buffer, midiMessages);
  }
}

juce::AudioBuffer<float>& ModMatrix::getModulatedParameterValueBuffer (juce::String parameterID)
{
  auto parameterRange = appState.getParameterRange (parameterID);
  auto halfRange = (parameterRange.end - parameterRange.start) / 2;

  auto& parameter = parameterMap.at (parameterID);

  auto& valueBuffer = parameter.buffer;
  auto valueBufferPtr = valueBuffer.getWritePointer (0);

  auto currValue = appState.getRawParameterValue (parameterID);
  auto& smoothedValue = parameter.smoothedValue;
  smoothedValue.setTargetValue (*currValue);

  for (int i = 0; i < valueBuffer.getNumSamples(); ++i)
  {
    valueBufferPtr[i] = smoothedValue.getNextValue();
  }

  for (int i = 0; i < NUM_LFOS; ++i)
  {
    auto modAmountValue = appState.getRawParameterValue (ModMatrix::getLFOModAmountParameterName (i, parameterID));

    if (*modAmountValue != 0 && lfos[i].isEnabled())
    {
      juce::FloatVectorOperations::addWithMultiply (valueBufferPtr, lfos[i].getOutputBuffer().getReadPointer (0), halfRange * (*modAmountValue), valueBuffer.getNumSamples());
    }
  }

  return valueBuffer;
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> ModMatrix::getAllModulatableParameters()
{
  juce::Array<std::unique_ptr<juce::RangedAudioParameter>> modulatableParameters;

  for (auto& parameter : AudioPluginAudioProcessor::getParameters())
  {
    if (!parameter->isDiscrete())
      modulatableParameters.add (std::move (parameter));
  }

  for (int i = 0; i < NUM_LFOS; ++i)
  {
    for (auto& parameter : LFOProcessor::getParameters (i))
    {
      if (!parameter->isDiscrete())
        modulatableParameters.add (std::move (parameter));
    }
  }

  for (auto& parameter : DelayProcessor::getParameters())
  {
    if (!parameter->isDiscrete())
      modulatableParameters.add (std::move (parameter));
  }

  return modulatableParameters;
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> ModMatrix::getParameters (juce::String suffix)
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();

  auto modulatableParameters = ModMatrix::getAllModulatableParameters();

  for (int i = 0; i < NUM_LFOS; ++i)
  {
    for (auto& parameter : LFOProcessor::getParameters (i))
    {
      parameters.add (std::move (parameter));
    }

    for (auto& modulatableParameter : modulatableParameters)
    {
      parameters.add (std::make_unique<juce::AudioParameterFloat> (ModMatrix::getLFOModAmountParameterName (i, modulatableParameter->getParameterID()),
        "Mod amount " + ModMatrix::ModAmountParamId + "LFO " + juce::String (i + 1) + " - " + modulatableParameter->getParameterID(),
        -1,
        1,
        0));
    }
  }

  return parameters;
}