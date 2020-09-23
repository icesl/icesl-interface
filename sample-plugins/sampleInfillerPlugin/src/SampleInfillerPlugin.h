#pragma once

#include "IPlugin.h"
#include "IInfillerPlugin.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();

extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);

// -----------------------------------------------

class SampleInfiller : public IceSLInterface::IInfillerInterface
{
public:

  SampleInfiller() = default;
  virtual ~SampleInfiller() = default;

  void setNumSlices(int num) override;
  void setWorldSpace(v3f world_corner_mm, v3f extent_mm) override;

  // called once at start
  void prepareInfill(int brush) override;

  // called once at end
  void terminateInfill(int brush) override;

  // called before each new slice
  void prepareInfillForSlice(int id, const AAB<2, int> &xy_slice_box, float height, int brush) override;

  // called to generate the infill in a surface
  bool generateInfill(int id, float slice_height_mm, int brush, const ClipperLib::Paths &surface, std::vector<std::unique_ptr<IceSLInterface::IPath> > &fills,bool &preserve_order) override;

};

class SampleInfillerPlugin : public IceSLInterface::IInfillerPlugin
{
private:

  std::vector<float> m_LineWidth_mm;
  float m_LineWidth_min_mm = 0.01f;
  float m_LineWidth_max_mm = 1.0f;

public:

  using IInfillerInterface = IceSLInterface::IInfillerInterface;

  SampleInfillerPlugin() = default;
  virtual ~SampleInfillerPlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment &env) override {
    IInfillerPlugin::initialize(env);
    gluxInit();
    return true;
  }
  void dispose() override {}

  std::string name() const override { return "SampleInfiller"; }
  std::string author() const override { return "Cédric Zanni, Salim Perchy and Sylvain Lefebvre"; }
  std::string comment() const override { return "Example for infiller plugins"; }

  bool addInfillerSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IInfillerInterface> createInfiller () const override
  {
     return std::unique_ptr<SampleInfiller>(new SampleInfiller());
  }
};

// -----------------------------------------------
