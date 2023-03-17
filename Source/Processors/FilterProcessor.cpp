#include "FilterProcessor.h"
#include "../PluginProcessor.h"
#include <math.h>

FilterProcessor::FilterProcessor (AudioPluginAudioProcessor& pluginProcessor_) :
  ProcessorBase (pluginProcessor_)

{
}

static double vsa = (1.0 / 4294967295.0);

void FilterProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
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

  auto lowGainValueBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (FilterProcessor::LowGainParamId).getReadPointer (0);
  auto midGainValueBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (FilterProcessor::MidGainParamId).getReadPointer (0);
  auto highGainValueBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (FilterProcessor::HighGainParamId).getReadPointer (0);

  for (int i = 0; i < len; ++i)
    for (int ch = 0; ch < numChannels; ++ch)
    {
      state[ch].lg = juce::Decibels::decibelsToGain<float> (lowGainValueBufferPtr[i]);
      state[ch].mg = juce::Decibels::decibelsToGain<float> (midGainValueBufferPtr[i]);
      state[ch].hg = juce::Decibels::decibelsToGain<float> (highGainValueBufferPtr[i]);

      const float* inPtr = inBlock.getChannelPointer (ch);
      float* outPtr = outBlock.getChannelPointer (ch);
      {
        outPtr[i] = do_3band (state[ch], inPtr[i]);
      }
    }
}

void FilterProcessor::init_3band_state (int lowfreq, int highfreq, int mixfreq)
{
  //reset state

  for (int ch = 0; ch < 2; ++ch)
  {
    // Filter #1 (Low band)
    state[ch].f1p0 = 0; // Poles ...
    state[ch].f1p1 = 0;
    state[ch].f1p2 = 0;
    state[ch].f1p3 = 0;

    // Filter #2 (High band)
    state[ch].f2p0 = 0; // Poles ...
    state[ch].f2p1 = 0;
    state[ch].f2p2 = 0;
    state[ch].f2p3 = 0;

    // Sample history buffer
    state[ch].sdm1 = 0; // Sample data minus 1
    state[ch].sdm2 = 0; //                   2
    state[ch].sdm3 = 0; //                   3

    // Set Low/Mid/High gains to unity
    state[ch].lg = 1.0;
    state[ch].mg = 1.0;
    state[ch].hg = 1.0;

    // Calculate filter cutoff frequencies
    state[ch].lf = 2 * sin (juce::double_Pi * ((double) lowfreq / (double) mixfreq));
    state[ch].hf = 2 * sin (juce::double_Pi * ((double) highfreq / (double) mixfreq));
  }
}

// ---------------
//| EQ one sample |
// ---------------

// - sample can be any range you like :)
//
// Note that the output will depend on the gain settings for each band
// (especially the bass) so may require clipping before output, but you
// knew that anyway :)

float FilterProcessor::do_3band (EQState& s, float sample)
{
  // Locals

  float l, m, h; // Low / Mid / High - Sample Values

  // Filter #1 (lowpass)

  s.f1p0 += (s.lf * (sample - s.f1p0)) + vsa;
  s.f1p1 += (s.lf * (s.f1p0 - s.f1p1));
  s.f1p2 += (s.lf * (s.f1p1 - s.f1p2));
  s.f1p3 += (s.lf * (s.f1p2 - s.f1p3));

  l = s.f1p3;

  // Filter #2 (highpass)

  s.f2p0 += (s.hf * (sample - s.f2p0)) + vsa;
  s.f2p1 += (s.hf * (s.f2p0 - s.f2p1));
  s.f2p2 += (s.hf * (s.f2p1 - s.f2p2));
  s.f2p3 += (s.hf * (s.f2p2 - s.f2p3));

  h = s.sdm3 - s.f2p3;

  // Calculate midrange (signal - (low + high))

  m = s.sdm3 - (h + l);

  // Scale, Combine and store

  l *= s.lg;
  m *= s.mg;
  h *= s.hg;

  // Shuffle history buffer

  s.sdm3 = s.sdm2;
  s.sdm2 = s.sdm1;
  s.sdm1 = sample;

  // Return result

  return (l + m + h);
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> FilterProcessor::getParameters()
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();
  parameters.add (std::make_unique<juce::AudioParameterFloat> (FilterProcessor::LowGainParamId,
    "Low gain",
    -24,
    24,
    0));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (FilterProcessor::MidGainParamId,
    "Mid gain",
    -24,
    24,
    0));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (FilterProcessor::HighGainParamId,
    "High gain",
    -24,
    24,
    0));

  return parameters;
}
