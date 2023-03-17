#include "ModelProcessor.h"
#include "../ModelLibrary.h"
#include "../PluginProcessor.h"

#include <algorithm>
#include <iostream>
#include <stdlib.h>

#define CUDA torch::kCUDA
#define CPU torch::kCPU

using namespace torch::indexing;

ModelProcessor::ModelProcessor (AudioPluginAudioProcessor& pluginProcessor_) :
  ProcessorBase (pluginProcessor_),
  isLoaded (false),
  isCudaAvailable (torch::cuda::is_available())
{
  appState.state.addListener (this);

  at::init_num_threads();
  if (isCudaAvailable)
    std::cout << "using cuda" << std::endl;
  else
    std::cout << "using cpu" << std::endl;

  // load ("C:/Users/Rob/Documents/Projects/Music/Tools/Combobulator/models/robclouth.ts");
}

void ModelProcessor::prepareToPlay (double sampleRate_, int samplesPerBlock_)
{
  sampleRate = sampleRate_;
  samplesPerBlock = samplesPerBlock_;
  juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
  inBuffer.setSize (1, samplesPerBlock * 2);
  paramsTensor = torch::zeros ({ 4, (int) samplesPerBlock });
  outRingBuffer.setSize (2, MAX_BUFFER);
  outFifo = juce::SingleThreadedAbstractFifo (MAX_BUFFER);

  if (isLoaded)
  {
    pluginProcessor.setLatencySamples (currModel.get_method ("calc_min_delay_samples") ({}).toInt());
  }
}

void ModelProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&)
{
  if (!isTorchInitialised)
  {
    at::init_num_threads();
    isTorchInitialised = true;
  }

  juce::dsp::AudioBlock<float> block (buffer);
  juce::dsp::ProcessContextReplacing<float> context (block);

  auto&& inBlock = context.getInputBlock();
  auto&& outBlock = context.getOutputBlock();

  jassert (inBlock.getNumChannels() == outBlock.getNumChannels());
  jassert (inBlock.getNumSamples() == outBlock.getNumSamples());

  auto len = inBlock.getNumSamples();
  auto numChannels = inBlock.getNumChannels();

  if (context.isBypassed || !isLoaded)
  {
    if (context.usesSeparateInputAndOutputBlocks())
      outBlock.copyFrom (inBlock);

    return;
  }

  currModel = nextModel;

  auto inBufferPtr = inBuffer.getWritePointer (0);

  juce::FloatVectorOperations::copy (inBufferPtr, inBlock.getChannelPointer (0), len);
  juce::FloatVectorOperations::copy (inBufferPtr + len, inBlock.getChannelPointer (1), len);

  c10::InferenceMode guard;

  auto inTensor = torch::from_blob (inBufferPtr, { 2, (int) len });

  auto scaleBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (ModelProcessor::ScaleParamId).getReadPointer (0);
  auto offsetBufferPtr = pluginProcessor.getModMatrix().getModulatedParameterValueBuffer (ModelProcessor::OffsetParamId).getReadPointer (0);

  paramsTensor.index_put_ ({ 0, Slice (0, None) }, scaleBufferPtr[0]);
  paramsTensor.index_put_ ({ 1, Slice (0, None) }, offsetBufferPtr[0]);

  if (isCudaAvailable)
    inTensor = inTensor.to (CUDA);

  try
  {
    auto outTensor = (*processMethod) ({ inTensor, paramsTensor }).toOptional<at::Tensor>();

    if (outTensor.has_value())
    {
      auto outTensorValue = outTensor.value();
      if (isCudaAvailable)
        outTensorValue = outTensorValue.to (CPU);

      auto outPtr = outTensorValue.contiguous().data_ptr<float>();

      auto outChannels = outTensorValue.size (0);
      auto outLength = outTensorValue.size (1);

      int start = 0;
      for (auto range : outFifo.write (outLength))
      {
        for (int ch = 0; ch < numChannels; ++ch)
        {
          outRingBuffer.copyFrom (ch, range.getStart(), outPtr + start + (ch % outChannels) * outLength, range.getLength());
        }
        start += range.getLength();
      }
    }
  } catch (const std::exception& e)
  {
    DBG (e.what());
  }

  // copy from ring buffer to output block
  if (outFifo.getNumReadable() > 0)
  {
    auto* outRingPtrs = outRingBuffer.getArrayOfWritePointers();

    int start = 0;
    for (auto range : outFifo.read (len))
    {
      for (int ch = 0; ch < numChannels; ++ch)
      {
        juce::FloatVectorOperations::copy (outBlock.getChannelPointer (ch) + start, outRingPtrs[ch] + range.getStart(), range.getLength());
      }

      start += range.getLength();
    }
  }
}

bool ModelProcessor::load (juce::String path)
{
  try
  {
    auto newModel = torch::jit::load (path.toStdString());
    newModel.eval();

    if (isCudaAvailable)
    {
      std::cout << "sending model to gpu" << std::endl;
      newModel.to (CUDA);
    }

    newModel.get_method ("set_daw_sample_rate_and_buffer_size") ({ c10::IValue ((int) sampleRate), c10::IValue (samplesPerBlock) }).toInt();

    pluginProcessor.setLatencySamples (newModel.get_method ("calc_min_delay_samples") ({}).toInt());

    isInputMono = newModel.get_method ("is_input_mono") ({}).toBool();
    isOutputMono = newModel.get_method ("is_output_mono") ({}).toBool();

    processMethod = std::make_unique<torch::jit::Method> (newModel.get_method ("forward_bt"));

    nextModel = newModel;
    return true;
  } catch (const std::exception& e)
  {
    DBG (e.what());
    return false;
  }
}

void ModelProcessor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
  if (property == ModelLibrary::CurrentModelNameId)
  {
    auto currentModelNameVar = treeWhosePropertyHasChanged.getProperty (property);
    auto modelListVar = treeWhosePropertyHasChanged.getProperty (ModelLibrary::ModelListId, juce::var());
    for (auto& modelVar : *(modelListVar.getArray()))
    {
      auto modelMetadataVar = juce::JSON::fromString (modelVar.toString());

      if (modelMetadataVar["model_name"].toString() == currentModelNameVar.toString())
      {
        isLoaded = load (modelMetadataVar["model_path"]);
        return;
      }
    }

    jassert (false);
  }
}

juce::Array<std::unique_ptr<juce::RangedAudioParameter>> ModelProcessor::getParameters()
{
  auto parameters = juce::Array<std::unique_ptr<juce::RangedAudioParameter>>();
  parameters.add (std::make_unique<juce::AudioParameterFloat> (ModelProcessor::ScaleParamId,
    "Latents scale",
    0,
    4,
    1));

  parameters.add (std::make_unique<juce::AudioParameterFloat> (ModelProcessor::OffsetParamId,
    "Latents offset",
    -2,
    2,
    0));

  return parameters;
}