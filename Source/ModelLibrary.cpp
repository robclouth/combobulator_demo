#include "./ModelLibrary.h"

ModelLibrary::ModelLibrary (juce::AudioProcessorValueTreeState& appState_) :
  appState (appState_)
{
  updateModelList();

  if (!appState.state.hasProperty (ModelLibrary::CurrentModelNameId))
  {
    auto modeListVar = appState.state.getProperty (ModelLibrary::ModelListId);
    DBG (modeListVar.getArray()->size());
    appState.state.setProperty (ModelLibrary::CurrentModelNameId, modeListVar[0], nullptr);
  }
}

void ModelLibrary::updateModelList()
{
  auto modelsPath = juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile).getParentDirectory().getChildFile ("Models");
  auto modelFolders = modelsPath.findChildFiles (juce::File::TypesOfFileToFind::findDirectories, true);

  juce::var modelListVar;

  for (auto folder : modelFolders)
  {
    auto modelFile = folder.getChildFile ("model.dm");
    auto metadataFile = folder.getChildFile ("metadata.json");

    if (modelFile.existsAsFile() && metadataFile.existsAsFile())
    {
      auto modelJsonVar = juce::JSON::parse (metadataFile.loadFileAsString());
      modelJsonVar.getDynamicObject()->setProperty ("model_path", modelFile.getFullPathName());

      modelListVar.append (juce::JSON::toString (modelJsonVar));
    }
  }

  appState.state.setProperty (ModelLibrary::ModelListId, modelListVar, nullptr);
}
