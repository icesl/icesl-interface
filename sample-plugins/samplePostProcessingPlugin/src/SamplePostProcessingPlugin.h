#pragma once

#include "IPostProcessingPlugin.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();
extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin * plugin);

// -----------------------------------------------

class SamplePostProcessing : public IceSLInterface::IPostProcessingInterface
{
public:
  SamplePostProcessing(const IceSLInterface::EnumerableSettingsInterface* settings);

  virtual ~SamplePostProcessing() override = default;

  bool postProcessLayer(const int extruder, const int layerID, bool& spiral, const bool last_part, const v2i &start, float layer_height_mm, double layer_thickness_mm, std::vector<std::shared_ptr<IceSLInterface::IPath> > &_layer) override;

private:
  float m_flow_multiplier_perimeter = 0;
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
    ImGui::SetCurrentContext(env.getImGuiContext());
    gluxInit();
    return true;
  }

  void dispose() override {}

  void gui(bool postService) override {}

  std::string name()    const override { return "SamplePostProcessingPlugin"; }
  std::string author()  const override { return "Salim Perchy"; }
  std::string comment() const override { return "Example of a post-processing plugin that applies a flow multiplier to perimeters"; }

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IPostProcessingInterface> createPostProcessing(const IceSLInterface::EnumerableSettingsInterface* settings) override
  {
    return std::unique_ptr<IPostProcessingInterface>(new SamplePostProcessing(settings));
  }

private:

  float m_FlowMultiplierPerimeter     = 1.0f;
  float m_FlowMultiplierPerimeter_max = 2.0f;
  float m_FlowMultiplierPerimeter_min = 0.0f;
};
