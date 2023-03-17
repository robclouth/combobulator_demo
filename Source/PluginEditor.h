#pragma once

#include "WebUI/BrowserComponent.h"
#include "WebUI/BrowserIntegration.h"
#include "WebUI/BrowserIntegrationPluginClient.h"
#include "WebUI/BufferSender.h"

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public BrowserIntegrationPluginClient
{
public:
  explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  //==============================================================================
  void paint (juce::Graphics&) override;
  void resized() override;

  void setScaleFactor (float newScale) override;

  BrowserIntegration& getBrowserIntegration()
  {
    return browserIntegration;
  }

private:
  AudioPluginAudioProcessor& processorRef;

  BrowserComponent browser;
  BrowserIntegration browserIntegration { browser };

  juce::OwnedArray<BufferSender> lfoScopes;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
