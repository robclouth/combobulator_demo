#pragma once

#include "../Defines.h"
#include "BrowserIntegration.h"
#include "BufferCollector.h"
#include <juce_core/juce_core.h>

class BufferSender : private juce::Timer
{
public:
  BufferSender (BufferCollector& collector_, BrowserIntegration& browserIntegration_) :
    browserIntegration (browserIntegration_),
    collector (collector_)
  {
    startTimerHz (SCOPE_RATE);
  }

private:
  void timerCallback()
  {
    while (!collector.isEmpty())
    {
      collector.pop (bufferData.data());
      juce::var data;
      for (int i = 0; i < collector.getBufferLength(); ++i)
        data.append (bufferData[i]);

      browserIntegration.sendEventToBrowser (collector.getEventType(), data, true);
    }
  }

  BrowserIntegration& browserIntegration;
  BufferCollector& collector;
  std::array<float, AudioBufferQueue<float>::bufferSize> bufferData;
};
