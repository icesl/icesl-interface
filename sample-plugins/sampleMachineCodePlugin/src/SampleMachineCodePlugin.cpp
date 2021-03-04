#include "SampleMachineCodePlugin.h"
#include "Path.h"
#include "PathTypeBitField.h"

#include <LibSL/System/System.h>
#include <iomanip>

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SampleMachineCodePlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}
// ----

std::unordered_map<std::string, v3f> SampleMachineCode::m_StrokeColors =
{
  {"Cover"    , v3f(0.354f, 0.737f, 0.770f)},
  {"Shell"    , v3f(0.239f, 0.480f, 0.858f)},
  {"Infill"   , v3f(0.708f, 0.938f, 1.000f)},
  {"Travel"   , v3f(1.000f, 0.933f, 0.345f)},
  {"Retract"  , v3f(0.937f, 0.325f, 0.314f)},
  {"Brim"     , v3f(0.553f, 0.819f, 0.453f)},
  {"Raft"     , v3f(0.553f, 0.819f, 0.453f)},
  {"Spiral"   , v3f(0.970f, 0.710f, 0.370f)},
  {"Seam"     , v3f(1.000f, 0.000f, 1.000f)},
  {"Bridge"   , v3f(0.500f, 0.400f, 0.300f)},
  {"Support"  , v3f(0.934f, 1.000f, 0.996f)},
  {"Ironing"  , v3f(1.000f, 0.700f, 0.300f)},
  {"Perimeter", v3f(0.354f, 0.737f, 0.770f)},
  {"Tower"    , v3f(0.600f, 0.600f, 0.600f)},
  {"Undefined", v3f(0.000f, 0.000f, 0.000f)}
};

static std::string pathTypetoStr(uint type)
{
  if       (type & static_cast<uint>(PathType::e_Perimeter))  return "Perimeter";
  else if  (type & static_cast<uint>(PathType::e_Infill))     return "Infill";
  else if  (type & static_cast<uint>(PathType::e_Shell))      return "Shell";
  else if  (type & static_cast<uint>(PathType::e_Support))    return "Support";
  else if  (type & static_cast<uint>(PathType::e_Bridge))     return "Bridge";
  else if  (type & static_cast<uint>(PathType::e_Raft))       return "Raft";
  else if  (type & static_cast<uint>(PathType::e_Brim))       return "Brim";
  else if  (type & static_cast<uint>(PathType::e_Tower))      return "Tower";
  else if  (type & static_cast<uint>(PathType::e_Cover))      return "Cover";
  else if  (type & static_cast<uint>(PathType::e_FreeZipper)) return "Seam";
  else if  (type & static_cast<uint>(PathType::e_Ironing))    return "Ironing";
  else if ((type & static_cast<uint>(PathType::e_Travel)) && 
           (type & static_cast<uint>(PathType::e_NoRetract))) return "Travel";
  else if  (type & static_cast<uint>(PathType::e_Travel))     return "Retract";
  else                                                        return "Undefined";
}

// -----------------------------------------------

bool SampleMachineCode::startWriting(const std::string& filename)
{
  try {

    // Get appropriate layer subset
    bool layer_subset;
    m_EnumerableSettings->getSettingByName("save_layer_subset")->getValue(layer_subset);
    if (layer_subset) {
      m_EnumerableSettings->getSettingByName("first_layer_saved")->getValue(m_FirstLayer);
      m_EnumerableSettings->getSettingByName("number_layers_saved")->getValue(m_LastLayer);
      m_LastLayer += m_FirstLayer;
    }

    // Check bounds
    if (m_FirstLayer >= m_LayerSet->getNumLayers() || m_LastLayer > m_LayerSet->getNumLayers()) {
      throw Fatal("[SampleMachineCode::startWriting] Out of bounds!\n1st layer: %d\nLast layer: %d\nNo. of layers: %d\n", m_FirstLayer, m_LastLayer, m_LayerSet->getNumLayers());
    }

    // Calculate tightbox
    float nozzleDiameter;
    for (int l = m_FirstLayer; l < m_LastLayer; l++) {
      const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(l);
      AAB<2> bsquare;
      for (int p = 0; p < layer.getNumPaths(); p++) {
        const IceSLInterface::IPath& path = layer.getPath(p);
        float nozzle;
        m_EnumerableSettings->getSettingByName("nozzle_diameter_mm_" + std::to_string(path.getTool()))->getValue(nozzle);
        nozzleDiameter = std::max(nozzle, nozzleDiameter);
        for (int v = 0; v < path.getNumVertices(); v++) {
          v2f point = m_LayerSet->xyBuildSpace(path.getVertexPos(v)); // build position
          point = v2f(LibSL::Math::quatf(v3f(1, 0, 0), (float)M_PI).toMatrix() * v4f(point[0], point[1], 0, 1)); // Rotate 180 in X to accomodate for SVG coordinate system (Y is flipped)
          m_TightSquare.addPoint(point);
        }
      }
    }
    m_TightSquare.enlarge(nozzleDiameter / 2.0f);

    m_WritingLayer = m_FirstLayer;
    m_OutputDirectory = filename;
    m_EnumerableSettings->getSettingByName("svg_machine_code_output_travel")->getValue(m_OutputTravel);

    m_EnumerableSettings->getSettingByName("bed_size_x_mm")->getValue(m_BedSize[0]);
    m_EnumerableSettings->getSettingByName("bed_size_y_mm")->getValue(m_BedSize[1]);
   
  } catch (Fatal& f) {
    std::cerr << f.message() << std::endl;
    return false;
  }

  return true;
}

// -----------------------------------------------

static std::string toHexString(v3f color)
{
  int hexColor = ((int)(color[0] * 255) << 16) | ((int)(color[1] * 255) << 8) | ((int)(color[2] * 255) << 0);
  // Courtesy of https://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c
  std::stringstream hexStream;
  hexStream << "#" << std::setfill('0') << std::setw(6) << std::hex << hexColor;
  return hexStream.str();
}

bool SampleMachineCode::stepWriting()
{
  try {

    if (m_WritingLayer < m_LastLayer) {

      // Output SVG per layer
      char filename[256];
      std::sprintf(filename, m_OuputFileFormat.c_str(), m_OutputDirectory.c_str(), m_WritingLayer);
      LibSL::SvgHelpers::Svg svg(filename, m_TightSquare, LibSL::Math::m4x4f::identity(), "mm");
      const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(m_WritingLayer);
      bool spiral = layer.isSpiral();
      std::vector<const IceSLInterface::IPath*> travelPaths;
      for (int p = 0; p < layer.getNumPaths(); p++) {
        const IceSLInterface::IPath& path = layer.getPath(p);
        std::string pathType = (spiral ? "Spiral" : pathTypetoStr(path.getPathType()));
        if (pathType == "Travel" || pathType == "Retract") {
          if (!m_OutputTravel) continue; // No travel output
          travelPaths.push_back(&path);
          continue;
        }
        float strokeWidth = 0;
        m_EnumerableSettings->getSettingByName("nozzle_diameter_mm_" + std::to_string(path.getTool()))->getValue(strokeWidth);
        v3f strokeColor = m_StrokeColors.at(pathType);
        svg.setProperties(toHexString(strokeColor), "none", strokeWidth);
        svg.startPath();
        for (int v = 0; v < path.getNumVertices(); v++) {
          v2f point = m_LayerSet->xyBuildSpace(path.getVertexPos(v)); // build position
          point = v2f(LibSL::Math::quatf(v3f(1, 0, 0), (float)M_PI).toMatrix() * v4f(point[0], point[1], 0, 1)); // Rotate 180 in X to accomodate for SVG coordinate system (Y is flipped)
          svg.addPoint(point[0], point[1]);
        }
        svg.endPath(true);
      }
      // Travel paths drawn later so they are always visible
      for (const IceSLInterface::IPath* path : travelPaths)  {
        sl_assert(path != nullptr);
        v3f strokeColor = m_StrokeColors.at(pathTypetoStr(path->getPathType()));
        svg.setProperties(toHexString(strokeColor), "none", 0.05f);
        svg.startPath();
        for (int v = 0; v < path->getNumVertices(); v++) {
          v2f point = m_LayerSet->xyBuildSpace(path->getVertexPos(v)); // build position
          point = v2f(LibSL::Math::quatf(v3f(1, 0, 0), (float)M_PI).toMatrix() * v4f(point[0], point[1], 0, 1)); // Rotate 180 in X to accomodate for SVG coordinate system (Y is flipped)
          svg.addPoint(point[0], point[1]);
        }
        svg.endPath(true);
      }

      // Prepare the OpenGL render target for this layer
      travelPaths.clear();
      GLint currentViewport[4];
      glGetIntegerv(GL_VIEWPORT, currentViewport); // save viewport
      m_SlicesRT.emplace_back(new RenderTarget2DRGBA(static_cast<uint>(m_BedSize[0]), static_cast<uint>(m_BedSize[1])));
      RenderTarget2DRGBA_Ptr RT = m_SlicesRT.back();
      RT->bind();
      glViewport(0, 0, RT->w(), RT->h());
      LibSL::GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER, 1.0f, 1.0f, 1.0f, 1.0f);
      m_Shader->begin();
      m_Shader->t_matrix.set(m4x4f::identity());
      m_Shader->p_matrix.set(orthoMatrixGL(0.0f, m_BedSize[0], -m_BedSize[1], 0.0f, -1.0f, 1.0f));
      // Render the layer unto the render target
      for (int p = 0; p < layer.getNumPaths(); p++) {
        const IceSLInterface::IPath& path = layer.getPath(p);
        std::string pathType = (spiral ? "Spiral" : pathTypetoStr(path.getPathType()));
        if (pathType == "Travel" || pathType == "Retract") {
          if (!m_OutputTravel) continue; // No travel output
          travelPaths.push_back(&path);
          continue;
        }
        v3f strokeColor = m_StrokeColors.at(pathType);
        m_Shader->u_color.set(v4f(strokeColor, 1.0f));
        m_VAO->begin(GPUMESH_LINESTRIP);
        for (int v = 0; v < path.getNumVertices(); v++) {
          v2f point = m_LayerSet->xyBuildSpace(path.getVertexPos(v)); // build position
          point = v2f(LibSL::Math::quatf(v3f(1, 0, 0), (float)M_PI).toMatrix() * v4f(point[0], point[1], 0, 1)); // Rotate 180 in X to (Y is flipped)
          m_VAO->vertex_3(point[0], point[1], 0.0f);
        }
        m_VAO->end();
        m_VAO->render();
      }
      //  Render the travel unto the render target
      for (const IceSLInterface::IPath* path : travelPaths) {
        v3f strokeColor = m_StrokeColors.at(pathTypetoStr(path->getPathType()));
        m_Shader->u_color.set(v4f(strokeColor, 1.0f));
        m_VAO->begin(GPUMESH_LINESTRIP);
        for (int v = 0; v < path->getNumVertices(); v++) {
          v2f point = m_LayerSet->xyBuildSpace(path->getVertexPos(v)); // build position
          point = v2f(LibSL::Math::quatf(v3f(1, 0, 0), (float)M_PI).toMatrix() * v4f(point[0], point[1], 0, 1)); // Rotate 180 in X to (Y is flipped)
          m_VAO->vertex_3(point[0], point[1], 0.0f);
        }
        m_VAO->end();
        m_VAO->render();
      }
      // --
      m_Shader->end();
      RT->unbind();
      glViewport(currentViewport[0], currentViewport[1], currentViewport[2], currentViewport[3]); // restore viewport

      // Next layer
      m_WritingLayer++;

    } else {
      // done
      m_DoneSlicesRT = true;
      return false;
    }

  } catch (Fatal& f) {
    std::cerr << Console::red << f.message() << Console::gray << std::endl;
    abortWriting();
    return false;
  }

  return true;
}

// -----------------------------------------------

void SampleMachineCode::abortWriting()
{
  char filename[256];
  for (int l = m_FirstLayer; l < m_LastLayer; l++) {
    std::sprintf(filename, m_OuputFileFormat.c_str(), m_OutputDirectory.c_str(), l);
    std::remove(filename);
  }
}

// -----------------------------------------------

float SampleMachineCode::writingProgress() const
{
  return static_cast<float>(m_WritingLayer) / (static_cast<float>(m_LastLayer) - 1.0f);
}

// -----------------------------------------------

bool SampleMachineCodePlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  auto s = enumerable_settings.addSettingFromPlugin(
    &m_OutputTravel, static_cast<bool*>(nullptr), static_cast<bool*>(nullptr),
    "svg_machine_code_output_travel", "SVG show travel", "Slicing", "Draw travel lines in SVG output",
    2,
    [this, &enumerable_settings]() -> bool {
      std::string machine_code;
      enumerable_settings.getSettingByName("output_machine_code")->getValue(machine_code);
      return machine_code == this->name();
    }
  );
  return s != nullptr;
}

// -----------------------------------------------

void SampleMachineCodePlugin::gui(bool)
{
  if (!m_ShowGui) {
    // SP 2021-01-25: IMPORTANT!
    // OpenGL resources used by ImGui should be released before being reference by any ImGui call (e.g., ImGui::Image)
    // so that when ImGui renders, it won't rerefence deleted textures. Hence why clearing m_SlicesRT is before creating the GUI
    m_SlicesRT.clear();
  }
  if (!m_SlicesRT.empty()) {
    static int shown_slice = 0;
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() * 2), ImGuiCond_Once);
    ImGui::Begin("SampleMachineCodePlugin", &m_ShowGui);
    RenderTarget2DRGBA_Ptr SliceTex = m_SlicesRT.at(shown_slice);
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(SliceTex->texture())), ImVec2(static_cast<float>(SliceTex->w()), static_cast<float>(SliceTex->h())));
    ImGui::Separator();
    ImGui::SliderInt("Layer", &shown_slice, 0, static_cast<int>(m_SlicesRT.size()) - 1);
    ImGui::End();
  }
}

// -----------------------------------------------
