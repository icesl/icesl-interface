#include "SampleMachineCodePlugin.h"
#include "Path.h"

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
    for (int l = m_FirstLayer; l < m_LastLayer; l++) {
      const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(l);
      for (int p = 0; p < layer.getNumPaths(); p++) {
        const IceSLInterface::IPath& path = layer.getPath(p);
        for (int v = 0; v < path.getNumVertices(); v++) {
          m_TightSquare.addPoint(m_LayerSet->xyBuildSpace(path.getVertexPos(v)));
        }
      }
    }

    m_WritingLayer = m_FirstLayer;
    m_EnumerableSettings->getSettingByName("svg_machine_code_stroke_width_mm")->getValue(m_StrokeWidth);
   
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

      // Output SCG per layer
      char filename[256];
      std::sprintf(filename, m_OuputFileFormat.c_str(), m_OutputDirectory, m_WritingLayer);
      LibSL::SvgHelpers::Svg svg(filename, m_TightSquare.enlarge(m_StrokeWidth), LibSL::Math::m4x4f::identity(), "mm");
      for (int l = m_FirstLayer; l < m_LastLayer; l++) {
        const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(l);
        for (int p = 0; p < layer.getNumPaths(); p++) {
          const IceSLInterface::IPath& path = layer.getPath(p);
          if (path.isCyclic()) {
            svg.startPolygon();
            svg.startPolygonPath();
          } else {
            svg.startPath();
          }
          for (int v = 0; v < path.getNumVertices(); v++) {
            v2f point = m_LayerSet->xyBuildSpace(path.getVertexPos(v));
            svg.addPoint(point[0], point[1]);
          }
          if (path.isCyclic()) {
            svg.endPolygonPath();
            svg.endPolygon();
          } else {
            svg.endPath();
          }
        }
      }
      m_WritingLayer++;

    } else {
      // done
      return false;
    }

  } catch (Fatal& f) {
    std::cerr << f.message() << std::endl;
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
    std::sprintf(filename, m_OuputFileFormat.c_str(), m_OutputDirectory, l);
    std::remove(filename);
  }
}

// -----------------------------------------------

float SampleMachineCode::writingProgress() const
{
  return static_cast<float>(m_WritingLayer) / (static_cast<float>(m_LayerSet->getNumLayers()) - 1.0f);
}

// -----------------------------------------------

bool SampleMachineCodePlugin::addMachineCodeSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  auto s = enumerable_settings.addSettingFromPlugin(
    &m_StrokeWidth_mm, &m_StrokeWidth_min_mm, &m_StrokeWidth_max_mm,
    "svg_machine_code_stroke_width_mm", "SVG output stroke width (mm)", "Slicing", "Stroke width in millimeters for paths drawn in the SVG output",
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