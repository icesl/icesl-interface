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

SamplePostProcessing::SamplePostProcessing(const IceSLInterface::EnumerableSettingsInterface* settings)
: IceSLInterface::IPostProcessingInterface(settings)
{
  m_EnumerableSettings->getSettingByName("flow_multiplier_perimeter")->getValue(m_flow_multiplier_perimeter);
}

// -----------------------------------------------

bool SamplePostProcessing::postProcessLayer(const int, const int, const bool, const bool, const v2i&, std::vector<std::shared_ptr<IceSLInterface::IPath>>& _layer)
{
  for (size_t i = 0; i < _layer.size(); i++) { // go through layer
    if (_layer[i]->getPathType() & PathType::e_Perimeter) { // only perimeters
      std::shared_ptr<IceSLInterface::IPath> path(new IceSLInterface::Path(*_layer[i])); // clone path (IceSL created paths can't be modified)
      int attr_idx = path->getPathAttributeIndex("flow_multiplier"); // flow multiplier already exists
      if (attr_idx < 0) {
        return false; // flow multiplier doesn't exist!
      }
      path->setPathAttributeValue(attr_idx, m_flow_multiplier_perimeter); // customize it
      _layer[i] = path; // replace obsolete IceSL path with out new perimeter
    }
  }
  return true; // succesfull
}

// -----------------------------------------------

bool SamplePostProcessingPlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  auto s = enumerable_settings.addSettingFromPlugin(
    &m_FlowMultiplierPerimeter, &m_FlowMultiplierPerimeter_min, &m_FlowMultiplierPerimeter,
    "flow_multiplier_perimeter", "Flow multiplier (perimeters)", "Processing",
    "Specifies a flow multiplier that only applies for perimeters.",
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
