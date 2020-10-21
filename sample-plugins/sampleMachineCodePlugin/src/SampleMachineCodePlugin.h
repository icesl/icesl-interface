#pragma once

#include "IPlugin.h"
#include "IMachineCodePlugin.h"

#include <unordered_map>

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();

extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);

// -----------------------------------------------

class SampleMachineCode : public IceSLInterface::IMachineCodeInterface
{
public:

  SampleMachineCode(
    std::unique_ptr<IceSLInterface::ILayerSet>         layers,
    const IceSLInterface::EnumerableSettingsInterface* settings,
    const std::set<int>&                               used_brushes)
    : IceSLInterface::IMachineCodeInterface(std::move(layers), settings, used_brushes)
  {
    std::string nOfDigits = std::to_string(std::to_string(m_LayerSet->getNumLayers()).size());
    m_OuputFileFormat = "%s/layer_%0" + nOfDigits + "d.svg"; // [output_folder]/layer_[layer_number].svg
    m_FirstLayer   = 0;
    m_LastLayer    = m_LayerSet->getNumLayers();
    m_WritingLayer = -1;
  }

  virtual ~SampleMachineCode() override = default;

  bool startWriting(const std::string& filename) override;
  bool stepWriting() override;
  void abortWriting() override;
  float writingProgress() const override;

private:
  std::string m_OuputFileFormat;
  std::string m_OutputDirectory;
  int m_FirstLayer;
  int m_LastLayer;
  int m_WritingLayer;

  AAB<2>    m_TightSquare;
  bool      m_OutputTravel;
  static std::unordered_map<std::string, v3f> m_StrokeColors;
};

class SampleMachineCodePlugin : public IceSLInterface::IMachineCodePlugin
{
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

  std::string name()      const override { return "SampleMachineCode"; }
  std::string author()    const override { return "Salim Perchy"; }
  std::string comment()   const override { return "Example of a MachineCode plugin that outputs paths into SVG files. Appropriate for debuging sliced output"; }
  std::string extension() const override { return "DIR"; } // Each layer gets an SVG file. Output to a directory

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IMachineCodeInterface> createMachineCode(
    std::unique_ptr<IceSLInterface::ILayerSet>         layers,
    const IceSLInterface::EnumerableSettingsInterface* settings,
    const std::set<int>&                               used_brushes) const override
  {
    return std::unique_ptr<IMachineCodeInterface>(new SampleMachineCode(std::move(layers), settings, used_brushes));
  }

private:

  bool m_OutputTravel = false;
};

// -----------------------------------------------