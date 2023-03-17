#pragma once

#include "ProcessorBase.h"
#include "../PluginProcessor.h"

ProcessorBase::ProcessorBase (AudioPluginAudioProcessor& pluginProcessor_) :
  AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo()).withOutput ("Output", juce::AudioChannelSet::stereo())), pluginProcessor (pluginProcessor_), appState (pluginProcessor.appState)
{
}
