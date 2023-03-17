#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor() :
  AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
  #if !JucePlugin_IsSynth
                    .withInput ("Input", juce::AudioChannelSet::stereo(), true)
  #endif
                    .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
  appState (*this, nullptr, juce::Identifier ("Combobulator"), AudioPluginAudioProcessor::createParameters()),
  modMatrix (*this),
  modelLibrary (appState),
  delayProcessor (*this)
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
  juce::AudioProcessorValueTreeState::ParameterLayout params;

  for (auto& parameter : AudioPluginAudioProcessor::getParameters())
  {
    params.add (std::move (parameter));
  }

  for (auto& parameter : DelayProcessor::getParameters())
  {
    params.add (std::move (parameter));
  }

  for (auto& parameter : ModMatrix::getParameters())
  {
    params.add (std::move (parameter));
  }

  return params;
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> AudioPluginAudioProcessor::getParameters()
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();
  parameters.add (std::make_unique<juce::AudioParameterFloat> (AudioPluginAudioProcessor::GainParamId,
    "Output gain",
    0.0f,
    1.0f,
    0.8f));

  return parameters;
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
  return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
  return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
  juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
  juce::ignoreUnused (index);
  return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
  juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  modMatrix.prepareToPlay (sampleRate, samplesPerBlock);
  delayProcessor.prepareToPlay (sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
  modMatrix.releaseResources();
  delayProcessor.releaseResources();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused (layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
      && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
  #if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
  #endif

  return true;
#endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
  juce::MidiBuffer& midiMessages)
{
  juce::ignoreUnused (midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear (i, 0, buffer.getNumSamples());

  modMatrix.processBlock (buffer, midiMessages);

  if (wrapperType == WrapperType::wrapperType_Standalone)
  {
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    auto&& inBlock = context.getInputBlock();
    auto&& outBlock = context.getOutputBlock();

    auto len = inBlock.getNumSamples();
    auto numChannels = inBlock.getNumChannels();

    for (int i = 0; i < len; ++i)
    {
      if (noiseBurst % 20000 < 5000)
        outBlock.getChannelPointer (0)[i] = outBlock.getChannelPointer (1)[i] = (random.nextFloat() * 2.f - 1.f) * 0.01f;
      noiseBurst++;
    }
  }

  delayProcessor.processBlock (buffer, midiMessages);

  auto gainBufferPtr = getModMatrix().getModulatedParameterValueBuffer (AudioPluginAudioProcessor::GainParamId).getReadPointer (0);

  for (int ch = 0; ch < totalNumOutputChannels; ++ch)
  {
    juce::FloatVectorOperations::multiply (buffer.getWritePointer (ch), gainBufferPtr, buffer.getNumSamples());
  }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
  return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
  auto state = appState.copyState();
  std::unique_ptr<juce::XmlElement> xml (state.createXml());
  copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

  // if (xmlState.get() != nullptr)
  //     if (xmlState->hasTagName (appState.state.getType()))
  //         appState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new AudioPluginAudioProcessor();
}
