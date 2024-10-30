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
      // get parameters at correct brush and height
      float xy_mm_per_int;
      m_EnumerableSettings->getSettingByName("xy_mm_per_int")->getValue(xy_mm_per_int);
      const auto& torsion_angle_setting = m_EnumerableSettings->getSettingByName("torsion_angle_"  + std::to_string(brush_contour));
      const auto& torsion_angle_axis_x  = m_EnumerableSettings->getSettingByName("torsion_axis_x_" + std::to_string(brush_contour));
      const auto& torsion_angle_axis_y  = m_EnumerableSettings->getSettingByName("torsion_axis_y_" + std::to_string(brush_contour));
      const auto& scale_factor_axis_x   = m_EnumerableSettings->getSettingByName("scale_factor_x_" + std::to_string(brush_contour));
      const auto& scale_factor_axis_y   = m_EnumerableSettings->getSettingByName("scale_factor_y_" + std::to_string(brush_contour));
      const auto& translate_distance_x  = m_EnumerableSettings->getSettingByName("translate_distance_x_" + std::to_string(brush_contour));
      const auto& translate_distance_y  = m_EnumerableSettings->getSettingByName("translate_distance_y_" + std::to_string(brush_contour));
      // torsion
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
      // scale
      float scale_x = 1.0f;
      if (scale_factor_axis_x->isPerLayerSetting()) { // per-layer
        float layer_height = static_cast<float>(layer_positions.at(layer).first); // bottom height of layer
        scale_x = static_cast<float>(scale_factor_axis_x->valueAtHeight(layer_height));
      } else { // constant 
        scale_factor_axis_x->getValue(scale_x);
      }
      float scale_y = 1.0f;
      if (scale_factor_axis_y->isPerLayerSetting()) { // per-layer
        float layer_height = static_cast<float>(layer_positions.at(layer).first); // bottom height of layer
        scale_y = static_cast<float>(scale_factor_axis_y->valueAtHeight(layer_height));
      } else { // constant 
        scale_factor_axis_y->getValue(scale_y);
      }
      // translate
      float translation_x = 0.0f;
      if (translate_distance_x->isPerLayerSetting()) { // per-layer
        float layer_height = static_cast<float>(layer_positions.at(layer).first); // bottom height of layer
        translation_x = static_cast<float>(translate_distance_x->valueAtHeight(layer_height));
      } else { // constant 
        translate_distance_x->getValue(translation_x);
      }
      translation_x /= xy_mm_per_int;
      float translation_y = 0.0f;
      if (translate_distance_y->isPerLayerSetting()) { // per-layer
        float layer_height = static_cast<float>(layer_positions.at(layer).first); // bottom height of layer
        translation_y = static_cast<float>(translate_distance_y->valueAtHeight(layer_height));
      } else { // constant 
        translate_distance_y->getValue(translation_y);
      }
      translation_y /= xy_mm_per_int;
      
      // transform points in contour
      std::vector<v2i> points_processed;
      for (const auto& point_contour : contour.second) {
        v2f point_pos(static_cast<float>(point_contour[0]), static_cast<float>(point_contour[1]));
        v2f point_centered               = point_pos - contours_center;
        v2f point_scaled                 = point_centered * v2f(scale_x, scale_y);
        v4f point_rotated                = LibSL::Math::quatf(v3f(0, 0, 1), static_cast<float>(M_PI) * torsion_angle / 180.0f) * v4f(point_scaled[0], point_scaled[1], 0.0f, 1.0f);
        v2f point_translated             = v2f(point_rotated[0], point_rotated[1]) + v2f(translation_x, translation_y);
        v2f point_transformed_uncentered = v2f(point_translated[0], point_translated[1]) + contours_center;
        points_processed.emplace_back(static_cast<int>(point_transformed_uncentered[0]), static_cast<int>(point_transformed_uncentered[1]));
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
  m_TorsionAxis.resize(enumerable_settings.getNumberOfBrushes(), v2f(0));
  m_ScaleFactor.resize(enumerable_settings.getNumberOfBrushes(), v2f(1));
  m_TranslateDistance.resize(enumerable_settings.getNumberOfBrushes(), v2f(0));
  // generate per-brush torsion angle setting
  for (unsigned int b = 0; b < m_TorsionAngle.size(); ++b) {

    // scale factor X
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s0 =
      enumerable_settings.addSettingFromPlugin(
        &(m_ScaleFactor.at(b)[0]), &m_ScaleFactor_min, (float*)nullptr,
        std::string("scale_factor_x_") + std::to_string(b),
        std::string("Scale factor in X"),
        "Brush_" + std::to_string(b),
        "Scale factor in the X axis",
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
    // scale factor Y
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s1 =
      enumerable_settings.addSettingFromPlugin(
        &(m_ScaleFactor.at(b)[1]), &m_ScaleFactor_min, (float*)nullptr,
        std::string("scale_factor_y_") + std::to_string(b),
        std::string("Scale factor in Y"),
        "Brush_" + std::to_string(b),
        "Scale factor in the Y axis",
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

    // torsion angle
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s2 =
      enumerable_settings.addSettingFromPlugin(
        &m_TorsionAngle.at(b), &m_TorsionAngle_min, &m_TorsionAngle_max,
        std::string("torsion_angle_") + std::to_string(b),
        std::string("Torsion angle (degrees)"),
        "Brush_" + std::to_string(b),
        "Torsion angle in degrees",
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
    // torsion axis -- X component
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s3 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TorsionAxis.at(b)[0]), (float*)nullptr, (float*)nullptr,
        std::string("torsion_axis_x_") + std::to_string(b),
        std::string("Torsion axis X (mm)"),
        "Brush_" + std::to_string(b),
        "Torsion axis (mm) -- X component. Relative to center of geometry",
        1003,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit
      );
    if (s3 == nullptr) {
      return false;
    }
    // torsion axis -- Y component
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s4 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TorsionAxis.at(b)[1]), (float*)nullptr, (float*)nullptr,
        std::string("torsion_axis_y_") + std::to_string(b),
        std::string("Torsion axis Y (mm)"),
        "Brush_" + std::to_string(b),
        "Torsion axis (mm) -- Y component. Relative to center of geometry",
        1004,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit
      );
    if (s4 == nullptr) {
      return false;
    }

    
    // translation in X
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s5 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TranslateDistance.at(b)[0]), (float*)nullptr, (float*)nullptr,
        std::string("translate_distance_x_") + std::to_string(b),
        std::string("Translation distance in X (mm)"),
        "Brush_" + std::to_string(b),
        "Translation distance in the X axis in millimeters.",
        1005,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit,
        IceSLInterface::EnumerableSettingsInterface::e_CanBePerLayer
      );
    if (s5 == nullptr) {
      return false;
    }
    // translation in Y
    std::unique_ptr<IceSLInterface::EnumerableSettingsInterface::SettingInterface> s6 =
      enumerable_settings.addSettingFromPlugin(
        &(m_TranslateDistance.at(b)[1]), (float*)nullptr, (float*)nullptr,
        std::string("translate_distance_y_") + std::to_string(b),
        std::string("Translation distance in Y (mm)"),
        "Brush_" + std::to_string(b),
        "Translation distance in the Y axis in millimeters.",
        1006,
        [this, &enumerable_settings]() -> bool {
          bool plugin_activated;
          enumerable_settings.getSettingByName(m_InternalName)->getValue(plugin_activated);
          return plugin_activated;
        },
        IceSLInterface::EnumerableSettingsInterface::e_NoUnit,
        IceSLInterface::EnumerableSettingsInterface::e_CanBePerLayer
      );
    if (s6 == nullptr) {
      return false;
    }
    
  }
  return true;
}

// -----------------------------------------------