#pragma once

#include "IMachineCodePlugin.h"
#include <LibSL/LibSL_gl4core.h>
#include "sh_simplerender.h"
#include <unordered_map>
#include <imgui.h>

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();

extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);

// -----------------------------------------------

class SampleMachineCode : public IceSLInterface::IMachineCodeInterface
{
public:

  using mvf_simplegeometry = GPUMESH_MVF1(mvf_vertex_3f);
  using t_SimpleGeometry   = GPUMesh_VertexBuffer<mvf_simplegeometry>;

  SampleMachineCode(
    std::unique_ptr<const IceSLInterface::ILayerSet>   layers,
    const IceSLInterface::EnumerableSettingsInterface* settings,
    const std::set<int>&                               used_brushes,
    std::vector<RenderTarget2DRGBA_Ptr>&               slices_RT
    )
    : IceSLInterface::IMachineCodeInterface(std::move(layers), settings, used_brushes), m_SlicesRT(slices_RT)
  {
    std::string nOfDigits = std::to_string(std::to_string(m_LayerSet->getNumLayers()).size());
    m_OuputFileFormat = "%s/layer_%0" + nOfDigits + "d.svg"; // [output_folder]/layer_[layer_number].svg
    m_FirstLayer   = 0;
    m_LastLayer    = m_LayerSet->getNumLayers();
    m_WritingLayer = -1;
    m_VAO = std::unique_ptr<t_SimpleGeometry>(new t_SimpleGeometry());
    m_Shader = std::unique_ptr<AutoBindShader::sh_simplerender>(new AutoBindShader::sh_simplerender());
    m_Shader->init();
    m_SlicesRT.clear();
  }

  virtual ~SampleMachineCode() override
  {
    m_Shader->terminate();
  }

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

  // Members to render slices unto GPU textures
  bool m_DoneSlicesRT = false;
  v2f m_BedSize;
  std::vector<RenderTarget2DRGBA_Ptr>&             m_SlicesRT;
  std::unique_ptr<t_SimpleGeometry>                m_VAO;
  std::unique_ptr<AutoBindShader::sh_simplerender> m_Shader;
};

class SampleMachineCodePlugin : public IceSLInterface::IMachineCodePlugin
{
public:

  using IMachineCodeInterface = IceSLInterface::IMachineCodeInterface;

  SampleMachineCodePlugin() = default;
  virtual ~SampleMachineCodePlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment& env) override
  {
    IMachineCodePlugin::initialize(env);
    ImGui::SetCurrentContext(env.getImGuiContext());
    gluxInit();
    return true;
  }

  void dispose() override {}

  void gui(bool postService) override;

  std::string name()      const override { return "SampleMachineCode"; }
  std::string author()    const override { return "Salim Perchy"; }
  std::string comment()   const override { return "Example of a MachineCode plugin that outputs paths into SVG files. Appropriate for debuging sliced output"; }
  std::string extension() const override { return "DIR"; } // Each layer gets an SVG file. Output to a directory

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IMachineCodeInterface> createMachineCode(
    std::unique_ptr<const IceSLInterface::ILayerSet>         layers,
    const IceSLInterface::EnumerableSettingsInterface*       settings,
    const std::set<int>&                                     used_brushes) override
  {
    m_ShowGui = true;
    return std::unique_ptr<IMachineCodeInterface>(new SampleMachineCode(std::move(layers), settings, used_brushes, m_SlicesRT));
  }

private:
  bool m_ShowGui = true;
  bool m_OutputTravel = false;
  std::vector<RenderTarget2DRGBA_Ptr> m_SlicesRT;
};

// -----------------------------------------------