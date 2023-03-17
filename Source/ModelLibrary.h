#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class ModelLibrary
{
public:
  static inline juce::Identifier ModelListId = juce::Identifier ("modelList");
  static inline juce::Identifier CurrentModelNameId = juce::Identifier ("currentModel");

  ModelLibrary (juce::AudioProcessorValueTreeState& appState_);

private:
  juce::AudioProcessorValueTreeState& appState;

  void updateModelList();
};