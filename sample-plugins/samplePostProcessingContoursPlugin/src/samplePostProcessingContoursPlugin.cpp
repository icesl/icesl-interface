#include "samplePostProcessingContoursPlugin.h"

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SamplePostProcessingContoursPlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}
// ----

// -----------------------------------------------

SamplePostProcessingContours::SamplePostProcessingContours(const IceSLInterface::EnumerableSettingsInterface* settings)
  : IceSLInterface::IPostProcessingContoursInterface(settings)
{

}

// -----------------------------------------------

bool SamplePostProcessingContours::postProcessContours(float contour_mm_per_int, std::vector<std::pair<double, double>>& layer_positions, std::vector<std::vector<std::pair<int, std::vector<v2i>>>>& _contours)
{
  // get layer thickness
  float layer_tickness_mm = 0.0f;
  // twist contours
  int layer = 0;
  // calculate contour center
  AAB<2> contour_box;
  for (auto& contours_layer : _contours) {
    for (auto& contour : contours_layer) {
      for (const auto& point_contour : contour.second) {
        contour_box.addPoint(v2f(static_cast<float>(point_contour[0]), static_cast<float>(point_contour[1])));
      }
    }
  }
  for (auto& contours_layer : _contours) {
    for (auto& contour : contours_layer) {
      // contour brush
      int brush_contour = contour.first;
      // get torsion angle at correct brush and height
      const auto& torsion_angle_setting = m_EnumerableSettings->getSettingByName("torsion_angle_"  + std::to_string(brush_contour));
      const auto& torsion_angle_axis_x  = m_EnumerableSettings->getSettingByName("torsion_axis_x_" + std::to_string(brush_contour));
      const auto& torsion_angle_axis_y  = m_EnumerableSettings->getSettingByName("torsion_axis_y_" + std::to_string(brush_contour));
      float axis_x, axis_y;
      torsion_angle_axis_x->getValue(axis_x);
      torsion_angle_axis_y->getValue(axis_y);
      v2f contours_center = contour_box.center() + v2f(axis_x / contour_mm_per_int, axis_y / contour_mm_per_int);
      float torsion_angle = 0.0f;
      if (torsion_angle_setting->isPerLayerSetting()) { // per-layer
        float layer_height = static_cast<float>(layer_positions.at(layer).first); // bottom height of layer
        torsion_angle = static_cast<float>(torsion_angle_setting->valueAtHeight(layer_height));
      } else { // constant 
        torsion_angle_setting->getValue(torsion_angle);
      }
      // torsion points in contour
      std::vector<v2i> points_processed;
      for (const auto& point_contour : contour.second) {
        v2f point_pos(static_cast<float>(point_contour[0]), static_cast<float>(point_contour[1]));
        v2f point_centered = point_pos - contours_center;
        v4f point_rotated = LibSL::Math::quatf(v3f(0, 0, 1), static_cast<float>(M_PI) * torsion_angle / 180.0f) * v4f(point_centered[0], point_centered[1], 0.0f, 1.0f);
        v2f point_rotated_uncentered = v2f(point_rotated[0], point_rotated[1]) + contours_center;
        points_processed.emplace_back(static_cast<int>(point_rotated_uncentered[0]), static_cast<int>(point_rotated_uncentered[1]));
      }
      contour.second = points_processed;
    }
    ++layer;
  }
  return true;
}

// -----------------------------------------------

bool SamplePostProcessingContoursPlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  // allocate memory for settings
  m_TorsionAngle.resize(enumerable_settings.getNumberOfBrushes(), 0);
  m_TorsionAxis.resize(enumerable_settings.getNumberOfBrushes(), 0);
  // generate per-brush torsion angle setting
  for (unsigned int b = 0; b < m_TorsionAngle.size(); ++b) {
    // torsion angle
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s0 =
      enumerable_settings.addSettingFromPlugin(
        &m_TorsionAngle.at(b), &m_TorsionAngle_min, &m_TorsionAngle_max,
        std::string("torsion_angle_") + std::to_string(b),
        std::string("Torsion angle (degrees)"),
        "Brush_" + std::to_string(b),
        "Torsion angle in degrees",
        1000,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit,
        IceSLInterface::EnumerableSettingsInterface::e_CanBePerLayer
      );
    if (s0 == nullptr) {
      return false;
    }
    // torsion axis -- X component
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s1 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TorsionAxis.at(b)[0]), (float*)nullptr, (float*)nullptr,
        std::string("torsion_axis_x_") + std::to_string(b),
        std::string("Torsion axis X (mm)"),
        "Brush_" + std::to_string(b),
        "Torsion axis (mm) -- X component. Relative to center of geometry",
        1001,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit,
          IceSLInterface::EnumerableSettingsInterface::e_CanBePerLayer
          );
    if (s1 == nullptr) {
      return false;
    }
    // torsion axis -- Y component
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s2 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TorsionAxis.at(b)[1]), (float*)nullptr, (float*)nullptr,
        std::string("torsion_axis_y_") + std::to_string(b),
        std::string("Torsion axis Y (mm)"),
        "Brush_" + std::to_string(b),
        "Torsion axis (mm) -- Y component. Relative to center of geometry",
        1002,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit,
          IceSLInterface::EnumerableSettingsInterface::e_CanBePerLayer
          );
    if (s2 == nullptr) {
      return false;
    }
    
  }
  return true;
}

// -----------------------------------------------