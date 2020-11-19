#include "SamplePostProcessingPlugin.h"
#include "Path.h"
#include "PathTypeBitField.h"

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SamplePostProcessingPlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}
// ----

// -----------------------------------------------

SamplePostProcessing::SamplePostProcessing(std::unique_ptr<IceSLInterface::ILayerSet> layers, const IceSLInterface::EnumerableSettingsInterface* settings)
: IceSLInterface::IPostProcessingInterface(std::move(layers), settings)
{
  float shield_distance_mm, xy_mm_per_int;
  m_EnumerableSettings->getSettingByName("shield_distance_to_print_mm")->getValue(shield_distance_mm);
  m_EnumerableSettings->getSettingByName("xy_mm_per_int")->getValue(xy_mm_per_int);
  m_LastPosition = ClipperLib::IntPoint(0, 0);
  // Calculate bounding square for the layer
  ForIndex(l, m_LayerSet->getNumLayers()) {
    ForIndex(p, m_LayerSet->getLayer(l).getNumPaths()) {
      ForIndex(v, m_LayerSet->getLayer(l).getPath(p).getNumVertices()) {
        m_Bbox.addPoint(v2i(
          static_cast<int>(m_LayerSet->getLayer(l).getPath(p).getVertexPos(v).X),
          static_cast<int>(m_LayerSet->getLayer(l).getPath(p).getVertexPos(v).Y)));
      }
    }
  }
  m_Bbox = m_Bbox.enlarge(v2i(static_cast<int>(shield_distance_mm / xy_mm_per_int))); // enlarge it to apply distance to shield
}

// -----------------------------------------------

bool SamplePostProcessing::postProcessLayers()
{
  AutoPtr<IceSLInterface::IPath> travel_start = AutoPtr<IceSLInterface::IPath>(new IceSLInterface::Path()); // From extruder position to min corner of bounding square
  AutoPtr<IceSLInterface::IPath> travel_end   = AutoPtr<IceSLInterface::IPath>(new IceSLInterface::Path()); // From min corner of bounding square to extruder position
  AutoPtr<IceSLInterface::IPath> shield       = AutoPtr<IceSLInterface::IPath>(new IceSLInterface::Path()); // Bounding square
  // Path types
  travel_start->setPathType(PathType::e_Travel);
  travel_end->setPathType(PathType::e_Travel);
  shield->setPathType(PathType::e_Brim); // Bounding square is a type of brim
  shield->setTool(0);
  // Path calculation
  static ClipperLib::IntPoint firstPosition;
  if (m_LayerSet->getLayer(m_ProcessedLayers).getNumPaths() != 0) {
    firstPosition = m_LayerSet->getLayer(m_ProcessedLayers).getPath(0).getVertexPos(0);
  }
  ClipperLib::Path path;
  path.emplace_back(firstPosition); // Extruder position
  path.emplace_back(m_Bbox.minCorner()[0], m_Bbox.minCorner()[1]); // Min corner of bounding square
  travel_start->createPath(path, false);
  std::reverse(path.begin(), path.end()); // Min corner of bounding square -> etruder position
  travel_start->createPath(path, false);
  path.clear();
  path.emplace_back(m_Bbox.minCorner()[0], m_Bbox.minCorner()[1]);
  path.emplace_back(m_Bbox.minCorner()[0], m_Bbox.maxCorner()[1]);
  path.emplace_back(m_Bbox.maxCorner()[0], m_Bbox.maxCorner()[1]);
  path.emplace_back(m_Bbox.maxCorner()[0], m_Bbox.minCorner()[1]);
  shield->createPath(path, true); // Bounding square
  // Insert new paths
  m_LayerSet->getLayer(m_ProcessedLayers).insertPath(0, travel_end);
  m_LayerSet->getLayer(m_ProcessedLayers).insertPath(0, shield);
  m_LayerSet->getLayer(m_ProcessedLayers).insertPath(0, travel_start);
  // done?
  return ++m_ProcessedLayers == m_LayerSet->getNumLayers();
}

// -----------------------------------------------

float SamplePostProcessing::postProcessingProgress() const
{
  return static_cast<float>(m_ProcessedLayers) / static_cast<float>(m_LayerSet->getNumLayers());
}

// -----------------------------------------------

bool SamplePostProcessingPlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  auto s = enumerable_settings.addSettingFromPlugin(
    &m_ShieldDistance_mm, &m_ShieldDistance_min_mm, static_cast<float*>(nullptr),
    "shield_distance_to_print_mm", "Distance from shield to print (mm)", "Processing",
    "Specifies the distance in millimeters (mm) between\nthe square shield and the print silhouette.",
    1000,
    [this, &enumerable_settings]() -> bool {
      bool plugin_activated;
      enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
      return plugin_activated;
    }
  );
  return s != nullptr;
}

// -----------------------------------------------