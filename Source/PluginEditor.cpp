#include "PluginEditor.h"
#include "Defines.h"
#include "PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p) :
  AudioProcessorEditor (&p),
  BrowserIntegrationPluginClient (browserIntegration, p.appState, __FILE__),
  processorRef (p)

{
  juce::ignoreUnused (processorRef);

  setSize (1500, 800);
  // setResizable (true, false);
  // setResizeLimits (1024, 800, 99999, 99999);

  addAndMakeVisible (browser);

  onInitialised = [this]() {
    for (int i = 0; i < NUM_LFOS; ++i)
    {
      lfoScopes.add (new BufferSender (processorRef.getModMatrix().lfos[i].scopeCollector, browserIntegration));
    }
  };

  setupBrowserPluginIntegration();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::setScaleFactor (float newScale)
{
  browser.setTransform (juce::AffineTransform::scale (newScale));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll (juce::Colours::white);
}

void AudioPluginAudioProcessorEditor::resized()
{
  browser.setBounds (getBounds());
}
