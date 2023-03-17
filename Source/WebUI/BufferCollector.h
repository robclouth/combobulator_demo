#pragma once

#include "../AudioBufferQueue.h"
#include <juce_core/juce_core.h>

class BufferCollector
{
public:
  BufferCollector (juce::String eventType_) :
    eventType (eventType_)
  {
  }

  int getBufferLength()
  {
    return bufferLength;
  }

  juce::String getEventType()
  {
    return eventType;
  }

  AudioBufferQueue<float>& getQueue()
  {
    return queue;
  }

  void push (float* data, int bufferLength_)
  {
    bufferLength = bufferLength_;
    queue.push (data, bufferLength);
  }
  
  void pop (float* data)
  {
    queue.pop (data, bufferLength);
  }


  bool isEmpty()
  {
    return queue.getNumReady() == 0;
  }

private:
  juce::String eventType;
  int bufferLength;
  AudioBufferQueue<float> queue;
};