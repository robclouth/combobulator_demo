#pragma once

#include "../AudioBufferQueue.h"
#include "../WebUI/BufferCollector.h"
#include "./ProcessorBase.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <string>
#include <vector>

enum class LFOType {
  Sine,
  Triangle,
  Ramp,
  Square,
  Noise
};

enum class LFORateDivisions {
  FourBars,
  TwoBars,
  Bar,
  BarDotted,
  BarTriplet,
  HalfBar,
  HalfBarDotted,
  HalfBarTriplet,
  Beat,
  BeatDotted,
  BeatTriplet,
  HalfBeat,
  HalfBeatDotted,
  HalfBeatTriplet,
  QuarterBeat,
  QuarterBeatDotted,
  QuarterBeatTriplet,
  EighthBeat,
  EighthBeatDotted,
  EighthBeatTriplet,
  SixteenthBeat,
  SixteenthBeatDotted,
  SixteenthBeatTriplet,
  ThirthyTwothBeat,
  ThirthyTwothBeatDotted,
  ThirthyTwothBeatTriplet,
  SixtyFourthBeat
};

#define SCOPE_RATIO 128

class LFOProcessor : public ProcessorBase
{
public:
  LFOProcessor (AudioPluginAudioProcessor& pluginProcessor, int index);

  const juce::String getName() const override { return "LFO" + juce::String (index + 1); }

  void prepareToPlay (double sampleRate_, int samplesPerBlock) override
  {
    sampleRate = sampleRate_;
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 1 };

    outputBuffer.setSize (1, samplesPerBlock);
    scopeBuffer.setSize (1, samplesPerBlock);
  }

  void releaseResources() override
  {
  }

  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  juce::AudioBuffer<float>& getOutputBuffer();

  bool isEnabled()
  {
    return *enabledParameter;
  }

  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters (int index);

  inline static juce::String EnabledParamId = "lfo$Enabled";
  inline static juce::String TypeParamId = "lfo$Type";
  inline static juce::String RateParamId = "lfo$Rate";
  inline static juce::String BeatSyncRateParamId = "lfo$BeatSyncRate";
  inline static juce::String BeatSyncParamId = "lfo$BeatSync";
  inline static juce::String ScaleParamId = "lfo$Scale";
  inline static juce::String OffsetParamId = "lfo$Offset";
  inline static juce::String PhaseParamId = "lfo$Phase";
  inline static juce::String PulseWidthParamId = "lfo$PulseWidth";

  BufferCollector scopeCollector;

private:
  void updatePhasorIncrement();
  float calculateValue (float currPhase, int typeValue, float phaseOffsetValue, float scaleValue, float offsetValue, float pulseWidthValue);
  void updateSyncedBeatRate();

  static juce::StringArray lfoTypes, beatSyncRates;

  int index;

  int scopeIndex = 0;

  double phasorIncrement = 0;
  double sampleRate = 0;
  double phasor = 0;
  double syncedRateInBeats = 0;

  juce::AudioBuffer<float> outputBuffer;
  juce::AudioBuffer<float> scopeBuffer;

  std::atomic<float>* enabledParameter = nullptr;
  std::atomic<float>* typeParameter = nullptr;
  std::atomic<float>* rateParameter = nullptr;
  std::atomic<float>* beatSyncRateParameter = nullptr;
  std::atomic<float>* beatSyncParameter = nullptr;
  std::atomic<float>* scaleParameter = nullptr;
  std::atomic<float>* offsetParameter = nullptr;
  std::atomic<float>* phaseParameter = nullptr;
  std::atomic<float>* pulseWidthParameter = nullptr;
};
