#pragma once

#include "IPlugin.h"
#include "IMachineCodePlugin.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();

extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);

// -----------------------------------------------

class SampleMachineCode : public IceSLInterface::IMachineCodeInterface
{
public:

  SampleMachineCode(std::unique_ptr<IceSLInterface::ILayerSet> layers,
                    const IceSLInterface::EnumerableSettingsInterface *settings,
                    const std::set<int>&               used_brushes)
    : IceSLInterface::IMachineCodeInterface(std::move(layers), settings, used_brushes) {}
  virtual ~SampleMachineCode() = default;

  bool startWriting(const std::string& filename) override;
  bool stepWriting() override;
  void abortWriting() override;
  float writingProgress() const override;

private:
  int          m_WritingLayer = -1;
  int m_LastLayer;
  std::ofstream m_Output;
};

class SampleMachineCodePlugin : public IceSLInterface::IMachineCodePlugin
{
private:

public:

  using IMachineCodeInterface = IceSLInterface::IMachineCodeInterface;

  SampleMachineCodePlugin() = default;
  virtual ~SampleMachineCodePlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment &env) override {
    IMachineCodePlugin::initialize(env);
    gluxInit();
    return true;
  }
  void dispose() override {}

  std::string name()    const override { return "SampleMachineCode"; }
  std::string author()  const override { return "Cédric Zanni, Salim Perchy and Sylvain Lefebvre"; }
  std::string comment() const override { return "Example of an MachineCode plugin"; }

  std::unique_ptr<IMachineCodeInterface> createMachineCodeWriter(std::unique_ptr<IceSLInterface::ILayerSet> layers,
                                                                 const IceSLInterface::EnumerableSettingsInterface *settings,
                                                                 const std::set<int>&               used_brushes) const override
  {
    return std::unique_ptr<IMachineCodeInterface>(new SampleMachineCode(std::move(layers), settings, used_brushes));
  }

/*
  bool addMachineCodeSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IMachineCodeInterface> createMachineCode () const override
  {
     return std::unique_ptr<SampleMachineCode>(new SampleMachineCode());
  }
*/
};

// -----------------------------------------------
