#include "SampleInfillerPlugin.h"
#include "Path.h"
#include "PathTypeBitField.h"

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SampleInfillerPlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}
// ----

// -----------------------------------------------

void SampleInfiller::setNumSlices(int num)
{

}

// -----------------------------------------------

void SampleInfiller::setWorldSpace(v3f world_corner_mm, v3f extent_mm)
{

}

// -----------------------------------------------

void SampleInfiller::prepareInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfiller::terminateInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfiller::prepareInfillForSlice(int id, const AAB<2, int>& xy_slice_box, float height_mm, double thickness_mm, int brush)
{

}

// ----------------------------------------------- 

bool SampleInfiller::generateInfill(int id, float height_mm, double thickness_mm, int brush, const ClipperLib::Paths& surface, std::vector<std::unique_ptr<IceSLInterface::IPath> >& fills, bool& preserve_order)
{
  /// This sample generates a simple concentric fill.
  //
  // => Note that this is a naive approach resulting in overfill,
  //    just used here for illustration.
  //
  /// retrive extruder used for the infill by this brush
  int extruder_id = 0;
  {
    auto s = m_EnumerableSettings->getSettingByName("infill_extruder_" + std::to_string(brush));
    if (s == nullptr) {
      throw Fatal("This plugin is only for FDM");
    }
    s->getValue(extruder_id);
  }
  /// retrieve extruder nozzle diameter
  float nozzle_diameter = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("nozzle_diameter_mm_" + std::to_string(extruder_id));
    if (s == nullptr) {
      throw Fatal("This plugin is only for FDM");
    }
    s->getValue(nozzle_diameter);
  }
  /// retrieve processing scale factor
  float xy_mm_per_int = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("xy_mm_per_int");
    if (s == nullptr) {
      throw Fatal("Missing setting");
    }
    s->getValue(xy_mm_per_int);
  }
  /// retrieve line width
  float line_width_mm = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("line_width_mm_" + std::to_string(brush));
    if (s == nullptr) {
      throw Fatal("Missing setting");
    }
    s->getValue(line_width_mm);
  }
  /// generate paths
  float radius = line_width_mm / 2.0f;
  ClipperLib::Paths current = surface;
  while (1) {
    ClipperLib::ClipperOffset offseter;
    offseter.AddPaths(current, ClipperLib::jtMiter, ClipperLib::EndType::etClosedPolygon);
    ClipperLib::Paths contours;
    offseter.Execute(contours, - radius / xy_mm_per_int);
    // next uses nozzle diameter as spacing
    radius = line_width_mm;
    // if result of offset is empty stop
    if (contours.empty()) {
      break;
    }
    /// add paths to output vector
    ClipperLib::CleanPolygons(contours);
    for (const auto &cnt : contours) {
      if (cnt.size() > 0) { // some paths may be empty after ClipperLib::CleanPolygons, these have to be skipped
        fills.push_back(std::unique_ptr<IceSLInterface::IPath>(new IceSLInterface::Path()));
        fills.back()->createPath(cnt, true);
        fills.back()->setPathType(PathType::e_Infill);
        // customize flow
        int flow_idx = fills.back()->addPathAttribute("flow_multiplier");
        fills.back()->setPathAttributeValue(flow_idx, line_width_mm / nozzle_diameter);
      }
    }
    // next
    current = contours;
  }
  return true;
}

// -----------------------------------------------

bool SampleInfillerPlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  // allocates an array with one value per brush
  m_LineWidth_mm.resize(enumerable_settings.getNumberOfBrushes(),0.4f);
  // generates setting per-brush for this infiller
  for (int i = 0; i < (int)m_LineWidth_mm.size(); i++) {
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s = 
    enumerable_settings.addSettingFromPlugin(
      &m_LineWidth_mm[i],
      &m_LineWidth_min_mm,
      &m_LineWidth_max_mm,
      "line_width_mm_" + std::to_string(i),
      "Line width",
      "Brush_" + std::to_string(i),
      "Specifies the line width to be used during contouring. The flow is adjusted accordingly.",
      1000, // rank to order multiple settings, lower appears before
      [this, &enumerable_settings, i]() -> bool { // show setting only when the infiller is selected
        std::string infiller;
        enumerable_settings.getSettingByName("infill_type_" + std::to_string(i))->getValue(infiller);
        return infiller == this->name();
      }
    );
    if (s == nullptr) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------
