#pragma once
#include "./ProcessorBase.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <string>
#include <torch/script.h>
#include <torch/torch.h>
#include <vector>

#define MAX_BUFFER 16384
#define MAX_NUM_PARAMS 4

class ModelProcessor : public ProcessorBase, public juce::ValueTree::Listener
{
public:
  inline static juce::String ScaleParamId = "latentsScale";
  inline static juce::String OffsetParamId = "latentsOffset";

  ModelProcessor (AudioPluginAudioProcessor& pluginProcessor);

  const juce::String getName() const override { return "Model"; }

  void prepareToPlay (double sampleRate_, int samplesPerBlock_) override;
  void releaseResources() override {}
  void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  bool load (juce::String path);

  bool isCudaAvailable;
  torch::jit::script::Module getModel() { return currModel; }

  static juce::Array<std::unique_ptr<juce::RangedAudioParameter>> getParameters();

private:
  void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

  at::Tensor paramsTensor;

  std::unique_ptr<torch::jit::Method> processMethod;
  torch::jit::script::Module currModel, nextModel;
  bool isLoaded;
  bool isInputMono, isOutputMono;
  bool isTorchInitialised = false;

  double sampleRate;
  int samplesPerBlock;

  std::string methodName = "forward";
  int modelBlockSize = 0;
  int numBatches = 1;
  juce::AudioBuffer<float> inBuffer, outRingBuffer;

  juce::SingleThreadedAbstractFifo outFifo;
};
