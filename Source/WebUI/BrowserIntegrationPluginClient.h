/**
 BrowserIntegrationPluginClient is intended as a base class for a JUCE
 PluginEditor which wants to communicate with a web UI inside a
 BrowserComponent.

 It takes a reference to the plugin's AudioProcessorValueTreeState, and
 takes care of synchronising state changes to JS and handling state changes
 from JS.
 */
#pragma once

#include "BrowserIntegration.h"
#include "BrowserIntegrationClient.h"
#include "BrowserValueTreeSynchroniser.h"

#include <juce_audio_processors/juce_audio_processors.h>

class BrowserIntegrationPluginClient : public BrowserIntegrationClient
{
public:
  BrowserIntegrationPluginClient (BrowserIntegration& browserIntegration,
    juce::AudioProcessorValueTreeState& parameterValueTree,
    juce::String pluginEditorFilePath,
    juce::String clientName = "Plugin");

  void setupBrowserPluginIntegration();

  std::function<void()> onInitialised;

protected:
  juce::AudioProcessorValueTreeState& parameterValueTree;
  BrowserValueTreeSynchroniser valueTreeSynchroniser;

  juce::File pluginEditorSourceFile;

  void sendParameterMetadata();
  void writeParameterConfigForTs (juce::Array<juce::var> parameterInfos);
};
