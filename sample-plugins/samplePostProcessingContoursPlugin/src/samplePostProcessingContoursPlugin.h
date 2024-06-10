#pragma once

#include "IPostProcessingContoursPlugin.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();
extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin * plugin);

// -----------------------------------------------

class SamplePostProcessingContours : public IceSLInterface::IPostProcessingContoursInterface
{
public:
  SamplePostProcessingContours(const IceSLInterface::EnumerableSettingsInterface* settings);

  virtual ~SamplePostProcessingContours() override = default;

  bool postProcessContours(
    float                                                         contour_mm_per_int,
    std::vector<std::pair<double, double>>&                       layer_positions,
    std::vector<std::vector<std::pair<int, std::vector<v2i>>>>&  _contours) override;

private:

};

class SamplePostProcessingContoursPlugin : public IceSLInterface::IPostProcessingContoursPlugin
{
public:
  
  using IPostProcessingContoursInterface = IceSLInterface::IPostProcessingContoursInterface;

  SamplePostProcessingContoursPlugin() = default;
  virtual ~SamplePostProcessingContoursPlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment& env) override
  {
    IPostProcessingContoursPlugin::initialize(env);
    ImGui::SetCurrentContext(env.getImGuiContext());
    gluxInit();
    return true;
  }

  void dispose() override {}

  void gui(bool postService) override {}

  std::string name()    const override { return "SamplePostProcessingContoursPlugin"; }
  std::string author()  const override { return "Salim Perchy"; }
  std::string comment() const override { return "Example of a post-processing plugin that applies a torsion angle to contours"; }

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IPostProcessingContoursInterface> createPostProcessingContours(const IceSLInterface::EnumerableSettingsInterface* settings) override
  {
    return std::unique_ptr<IPostProcessingContoursInterface>(new SamplePostProcessingContours(settings));
  }


private:

  std::vector<float> m_TorsionAngle;
  float m_TorsionAngle_min = 0.0f;
  float m_TorsionAngle_max = 360.0f;
};