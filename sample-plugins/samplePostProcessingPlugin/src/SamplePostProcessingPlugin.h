#pragma once

#include "IPostProcessingPlugin.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();
extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin * plugin);

// -----------------------------------------------

class SamplePostProcessing : public IceSLInterface::IPostProcessingInterface
{
public:
  SamplePostProcessing(
    std::unique_ptr<IceSLInterface::ILayerSet>         layers,
    const IceSLInterface::EnumerableSettingsInterface* settings);

  virtual ~SamplePostProcessing() override = default;

  bool postProcessLayers() override;

  float postProcessingProgress() const override;

private:
  int   m_ProcessedLayers = 0;

  AAB<2, int>          m_Bbox;
  ClipperLib::IntPoint m_LastPosition;
};

class SamplePostProcessingPlugin : public IceSLInterface::IPostProcessingPlugin
{
public:

  using IPostProcessingInterface = IceSLInterface::IPostProcessingInterface;

  SamplePostProcessingPlugin() = default;
  virtual ~SamplePostProcessingPlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment& env) override
  {
    IPostProcessingPlugin::initialize(env);
    gluxInit();
    return true;
  }

  void dispose() override {}

  std::string name()    const override { return "SamplePostProcessingPlugin"; }
  std::string author()  const override { return "Salim Perchy"; }
  std::string comment() const override { return "Example of a post-processing plugin that creates a squared shield around the printed part"; }

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IPostProcessingInterface> createPostProcessing(
    std::unique_ptr<IceSLInterface::ILayerSet>         layers,
    const IceSLInterface::EnumerableSettingsInterface* settings) const override
  {
    return std::unique_ptr<IPostProcessingInterface>(new SamplePostProcessing(std::move(layers), settings));
  }

  bool insertTravelPaths() const override { return s_IceSLTravelCalculation; }

private:

  float m_ShieldDistance_mm     = 5.0f;
  float m_ShieldDistance_min_mm = 1.0f;

public:

  static const bool s_IceSLTravelCalculation; // Should IceSL should take care of traveling?
};