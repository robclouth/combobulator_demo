#pragma once

#include <juce_core/juce_core.h>

//==============================================================================
template <typename SampleType>
class AudioBufferQueue
{
public:
  //==============================================================================
  static constexpr size_t order = 9;
  static constexpr size_t bufferSize = 1U << order;
  static constexpr size_t numBuffers = 100;

  //==============================================================================
  void push (const SampleType* dataToPush, size_t numSamples)
  {
    jassert (numSamples <= bufferSize);

    int start1, size1, start2, size2;
    abstractFifo.prepareToWrite (1, start1, size1, start2, size2);

    jassert (size1 <= 1);
    jassert (size2 == 0);

    if (size1 > 0)
      juce::FloatVectorOperations::copy (buffers[(size_t) start1].data(), dataToPush, (int) juce::jmin (bufferSize, numSamples));

    abstractFifo.finishedWrite (size1);
  }

  //==============================================================================
  void pop (SampleType* outputBuffer, size_t numSamples)
  {
    jassert (numSamples <= bufferSize);

    int start1, size1, start2, size2;
    abstractFifo.prepareToRead (1, start1, size1, start2, size2);

    jassert (size1 <= 1);
    jassert (size2 == 0);

    if (size1 > 0)
      juce::FloatVectorOperations::copy (outputBuffer, buffers[(size_t) start1].data(), (int) numSamples);

    abstractFifo.finishedRead (size1);
  }

  int getNumReady()
  {
    return abstractFifo.getNumReady();
  }

private:
  //==============================================================================
  juce::AbstractFifo abstractFifo { numBuffers };
  std::array<std::array<SampleType, bufferSize>, numBuffers> buffers;
};