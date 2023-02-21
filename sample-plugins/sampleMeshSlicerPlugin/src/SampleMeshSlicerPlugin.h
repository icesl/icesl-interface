#pragma once

#include "IMeshSlicerPlugin.h"

#include <LibSL/LibSL_gl4core.h>

#include "sh_simplerender.h"

// -----------------------------------------------

extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();

extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin * plugin);

// -----------------------------------------------

class SampleMeshSlicer : public IceSLInterface::IMeshSlicerInterface
{
public:

  SampleMeshSlicer() = default;
  virtual ~SampleMeshSlicer();

  void startSlicing(const IceSLInterface::ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh) override;

  bool processSlicing() override;

  void slice(const IceSLInterface::ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh) override;

  std::vector<std::vector<IceSLInterface::Contour>>&       slices()       { return m_Slices; }
  const std::vector<std::vector<IceSLInterface::Contour>>& slices() const { return m_Slices; }

  unsigned int getNumOfProcessingSteps()            const override;

  unsigned int getCurrentProcessingStep()           const override;

  float       getCurrentProcessingStepProgress()    const override;

  std::string getCurrentProcessingStepDescription() const override;

  float& MMperInt() { return m_MMperInt; }

private:

  enum class e_Step { e_Rendering, e_Vectorizing, size }; // slicing steps

  e_Step       m_CurrentStep;           // current slicing step
  unsigned int m_CurrentLayerProcessed; // current layer being sliced
  unsigned int m_NumOfLayers;           // total number of layers to slice

  const IceSLInterface::ISlicingPlan* m_SlicingPlan = nullptr; // slicing plan
  TriangleMesh_Ptr& m_Mesh = TriangleMesh_Ptr();               // mesh to slice

  float m_MMperInt;                     // resolution input from IceSL (we don't use it to avoid long calculation times)
  float m_MMperIntPlugin = 0.1f;        // resolution of sliced output. The lower, the more quality and hence the more resources. 

  using t_VertexFormat = MVF1(mvf_position_3f);
  using t_BufferFormat = GPUMesh_VertexBuffer<t_VertexFormat>;

  using RTFormat  = RenderTarget2DLum;
  using IMGFormat = ImageL8;

  std::unique_ptr<RTFormat>                         m_RenderTarget; // render target to rasterize layer
  std::unique_ptr<AutoBindShader::sh_simplerender>  m_Shader;       // simple rendering shader
  std::unique_ptr<MeshRenderer<t_VertexFormat>>     m_MeshRenderer; // mesh renderer
  std::unique_ptr<t_BufferFormat>                   m_VAO;          // vertex array of simple quad


  std::vector<std::shared_ptr<IMGFormat>>           m_Images;       // rasterized layers
  std::vector<std::vector<IceSLInterface::Contour>> m_Slices;       // vectorized layers

  void rasterizedLayer(unsigned int layer_id);                      // rasterize individual layer
  void vectorizeLayer(unsigned int layer_id);                       // vectorize individual layer

};

class SampleMeshSlicerPlugin : public IceSLInterface::IMeshSlicerPlugin
{
public:

  using IMeshSlicerInterface = IceSLInterface::IMeshSlicerInterface;

  SampleMeshSlicerPlugin() = default;
  virtual ~SampleMeshSlicerPlugin() = default;

  bool initialize(IceSLInterface::IPluginEnvironment& env) override
  {
    IMeshSlicerPlugin::initialize(env);
    ImGui::SetCurrentContext(env.getImGuiContext());
    gluxInit();
    return true;
  }

  void dispose() override {}

  void gui(bool postService) override {}

  std::string name()    const override { return "SampleMeshSlicer"; }
  std::string author()  const override { return "Salim Perchy"; }
  std::string comment() const override { return "Example of a mesh slicer"; }

  bool addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings) override;

  std::unique_ptr<IMeshSlicerInterface> createMeshSlicer() override;

};