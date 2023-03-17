#include "LFOProcessor.h"
#include "../Defines.h"
#include "../PluginEditor.h"
#include "../PluginProcessor.h"
#include "MathUtils.h"
#include "simplexnoise.h"

juce::StringArray LFOProcessor::lfoTypes = { "Sine", "Triangle", "Ramp", "Square", "Noise" };
juce::StringArray LFOProcessor::beatSyncRates = { "4 Bars",
  "2 Bars",
  "1 Bar",
  "1 Bar Dotted",
  "1 Bar Triplet",
  "1/2 Bar",
  "1/2 Bar Dotted",
  "1/2 Bar Triplet",
  "1 Beat",
  "1 Beat Dotted",
  "1 Beat Triplet",
  "1/2 Beat",
  "1/2 Beat Dotted",
  "1/2 Beat Triplet",
  "1/4 Beat",
  "1/4 Beat Dotted",
  "1/4 Beat Triplet",
  "1/8 Beat",
  "1/8 Beat Dotted",
  "1/8 Beat Triplet",
  "1/16 Beat",
  "1/16 Beat Dotted",
  "1/16 Beat Triplet",
  "1/32 Beat",
  "1/32 Beat Dotted",
  "1/32 Beat Triplet",
  "1/64 Beat" };

LFOProcessor::LFOProcessor (AudioPluginAudioProcessor& pluginProcessor, int index_) :
  ProcessorBase (pluginProcessor),
  scopeCollector ("LFO" + juce::String (index_ + 1) + "::lfoData"),
  index (index_)

{
  enabledParameter = appState.getRawParameterValue (LFOProcessor::EnabledParamId.replace ("$", juce::String (index + 1)));
  typeParameter = appState.getRawParameterValue (LFOProcessor::TypeParamId.replace ("$", juce::String (index + 1)));
  beatSyncParameter = appState.getRawParameterValue (LFOProcessor::BeatSyncParamId.replace ("$", juce::String (index + 1)));
  rateParameter = appState.getRawParameterValue (LFOProcessor::RateParamId.replace ("$", juce::String (index + 1)));
  beatSyncRateParameter = appState.getRawParameterValue (LFOProcessor::BeatSyncRateParamId.replace ("$", juce::String (index + 1)));
  scaleParameter = appState.getRawParameterValue (LFOProcessor::ScaleParamId.replace ("$", juce::String (index + 1)));
  offsetParameter = appState.getRawParameterValue (LFOProcessor::OffsetParamId.replace ("$", juce::String (index + 1)));
  phaseParameter = appState.getRawParameterValue (LFOProcessor::PhaseParamId.replace ("$", juce::String (index + 1)));
  pulseWidthParameter = appState.getRawParameterValue (LFOProcessor::PulseWidthParamId.replace ("$", juce::String (index + 1)));
}

void LFOProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
  auto len = buffer.getNumSamples();

  if (*enabledParameter < 0.5)
  {
    outputBuffer.clear();

    return;
  }

  updatePhasorIncrement();

  if (*beatSyncParameter > 0.5)
  {
    auto hostPhase = *(pluginProcessor.getPlayHead()->getPosition()->getPpqPosition());
    phasor =
      fmod ((float) hostPhase, syncedRateInBeats) / syncedRateInBeats;
  }

  auto outputBufferPtr = outputBuffer.getWritePointer (0);

  // TODO: make parameters modulatable
  int typeValue = (int) (*typeParameter);
  float phaseValue = *phaseParameter;
  float scaleValue = *scaleParameter;
  float offsetValue = *offsetParameter;
  float pulseWidthValue = pow (2, *pulseWidthParameter * 4);

  for (int i = 0; i < len; ++i)
  {
    auto value = calculateValue (phasor, typeValue, phaseValue, scaleValue, offsetValue, pulseWidthValue);
    outputBufferPtr[i] = value;
    if (scopeIndex > SCOPE_RATIO)
    {
      scopeIndex = 0;
      scopeCollector.push (outputBufferPtr + i, 1);
    }

    scopeIndex++;
    phasor += phasorIncrement;
  }
}

void LFOProcessor::updatePhasorIncrement()
{
  if (*beatSyncParameter < 0.5)
    phasorIncrement = (*rateParameter) / sampleRate;
  else
  {
    updateSyncedBeatRate();
    auto hostBpm = *(pluginProcessor.getPlayHead()->getPosition()->getBpm());
    double period = syncedRateInBeats / hostBpm * 60;
    phasorIncrement = period / sampleRate;
  }
}

float LFOProcessor::calculateValue (float currPhase, int typeValue, float phaseOffsetValue, float scaleValue, float offsetValue, float pulseWidthValue)
{
  float outValue = 0;
  float modPhase =
    MathUtils::fastclamp (MathUtils::fastmod1 (currPhase + phaseOffsetValue) * pulseWidthValue, 0, 1);

  if (typeValue == (int) LFOType::Sine)
    outValue = (MathUtils::fastsin (modPhase * 2 * juce::float_Pi));
  else if (typeValue == (int) LFOType::Triangle)
    outValue = abs (modPhase * 4 - 2) - 1;
  else if (typeValue == (int) LFOType::Ramp)
    outValue = modPhase * 2 - 1;
  else if (typeValue == (int) LFOType::Square)
    outValue = modPhase > 0.5 ? 1 : -1;
  else if (typeValue == (int) LFOType::Noise)
    outValue = raw_noise_2d (
      fastfloor ((currPhase + phaseOffsetValue) * (pulseWidthValue * 4) * 10) / (pulseWidthValue * 4), 0);

  return outValue * scaleValue + offsetValue;
}

void LFOProcessor::updateSyncedBeatRate()
{
  int division = (int) *beatSyncRateParameter;

  switch ((LFORateDivisions) division)
  {
    case LFORateDivisions::FourBars:
      syncedRateInBeats = 16;
      break;
    case LFORateDivisions::TwoBars:
      syncedRateInBeats = 8;
      break;
    case LFORateDivisions::Bar:
      syncedRateInBeats = 4;
      break;
    case LFORateDivisions::BarDotted:
      syncedRateInBeats = 4 * 1.5;
      break;
    case LFORateDivisions::BarTriplet:
      syncedRateInBeats = 4 * 1 / 3.;
      break;
    case LFORateDivisions::HalfBar:
      syncedRateInBeats = 2;
      break;
    case LFORateDivisions::HalfBarDotted:
      syncedRateInBeats = 2 * 1.5;
      break;
    case LFORateDivisions::HalfBarTriplet:
      syncedRateInBeats = 2 * 1 / 3.;
      break;
    case LFORateDivisions::Beat:
      syncedRateInBeats = 1;
      break;
    case LFORateDivisions::BeatDotted:
      syncedRateInBeats = 1 * 1.5;
      break;
    case LFORateDivisions::BeatTriplet:
      syncedRateInBeats = 1 * 1 / 3.;
      break;
    case LFORateDivisions::HalfBeat:
      syncedRateInBeats = 1 / 2.;
      break;
    case LFORateDivisions::HalfBeatDotted:
      syncedRateInBeats = 1 / 2. * 1.5;
      break;
    case LFORateDivisions::HalfBeatTriplet:
      syncedRateInBeats = 1 / 2. * 1 / 3.;
      break;
    case LFORateDivisions::QuarterBeat:
      syncedRateInBeats = 1 / 4.;
      break;
    case LFORateDivisions::QuarterBeatDotted:
      syncedRateInBeats = 1 / 4. * 1.5;
      break;
    case LFORateDivisions::QuarterBeatTriplet:
      syncedRateInBeats = 1 / 4. * 1 / 3.;
      break;
    case LFORateDivisions::EighthBeat:
      syncedRateInBeats = 1 / 8.;
      break;
    case LFORateDivisions::EighthBeatDotted:
      syncedRateInBeats = 1 / 8. * 1.5;
      break;
    case LFORateDivisions::EighthBeatTriplet:
      syncedRateInBeats = 1 / 8. * 1 / 3.;
      break;
    case LFORateDivisions::SixteenthBeat:
      syncedRateInBeats = 1 / 16.;
      break;
    case LFORateDivisions::SixteenthBeatDotted:
      syncedRateInBeats = 1 / 16. * 1.5;
      break;
    case LFORateDivisions::SixteenthBeatTriplet:
      syncedRateInBeats = 1 / 16. * 1 / 3.;
      break;
    case LFORateDivisions::ThirthyTwothBeat:
      syncedRateInBeats = 1 / 32.;
      break;
    case LFORateDivisions::ThirthyTwothBeatDotted:
      syncedRateInBeats = 1 / 32. * 1.5;
      break;
    case LFORateDivisions::ThirthyTwothBeatTriplet:
      syncedRateInBeats = 1 / 32. * 1 / 3.;
      break;
    case LFORateDivisions::SixtyFourthBeat:
      syncedRateInBeats = 1 / 64.;
      break;
    default:
      syncedRateInBeats = 1;
      break;
  }
}

juce::AudioBuffer<float>& LFOProcessor::getOutputBuffer()
{
  return outputBuffer;
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> LFOProcessor::getParameters (int index)
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();

  parameters.add (std::make_unique<juce::AudioParameterBool> (LFOProcessor::EnabledParamId.replace ("$", juce::String (index + 1)),
    "Enabled",
    index == 0));

  parameters.add (std::make_unique<juce::AudioParameterChoice> (LFOProcessor::TypeParamId.replace ("$", juce::String (index + 1)),
    "Type",
    lfoTypes,
    0));

  parameters.add (std::make_unique<juce::AudioParameterBool> (LFOProcessor::BeatSyncParamId.replace ("$", juce::String (index + 1)),
    "Beat sync",
    false));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (LFOProcessor::RateParamId.replace ("$", juce::String (index + 1)),
    "Rate",
    0.01,
    40,
    1));

  parameters.add (std::make_unique<juce::AudioParameterChoice> (LFOProcessor::BeatSyncRateParamId.replace ("$", juce::String (index + 1)),
    "Beat sync rate",
    beatSyncRates,
    0));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (LFOProcessor::ScaleParamId.replace ("$", juce::String (index + 1)),
    "Scale",
    -1,
    1,
    1));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (LFOProcessor::OffsetParamId.replace ("$", juce::String (index + 1)),
    "Offset",
    -1,
    1,
    0));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (LFOProcessor::PhaseParamId.replace ("$", juce::String (index + 1)),
    "Phase",
    0,
    1,
    0));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (LFOProcessor::PulseWidthParamId.replace ("$", juce::String (index + 1)),
    "Pulse width",
    0,
    1,
    0));
  return parameters;
}
