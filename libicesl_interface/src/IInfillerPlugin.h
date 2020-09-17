#pragma once

#include "IPlugin.h"

#include <clipper.hpp>
#include "EnumerableSettingsInterface.h"

namespace IceSLInterface {

/**
 * @brief The IPluginInfiller class
 */
class IInfillerPlugin : public IPlugin
{
public:
  IInfillerPlugin() = default;
  ~IInfillerPlugin() override = default;

  bool initialize(IPluginEnvironment &env) override {}

  void setSettings(const EnumerableSettingsInterface* settings) { m_EnumerableSettings = settings; }

  virtual void setNumSlices(int num) = 0;
  virtual void setWorldSpace(v3f world_corner_mm, v3f extent_mm) = 0;

  // called once at start
  virtual void prepareInfill(int brush) = 0;

  // called once at end
  virtual void terminateInfill(int brush) = 0;

  // called before each new slice
  virtual void prepareInfillForSlice(int id, const AAB<2, int>& xy_slice_box, float height, int brush) = 0;

  // called to generate the infill in a toolPathTree
  virtual bool generateInfill(int id, float height_mm, int brush, const ClipperLib::Paths& surface, std::vector<ClipperLib::Paths>& fills) = 0;

private:
  //IceSLInterface:: @todo add namespace
  const EnumerableSettingsInterface* m_EnumerableSettings;
};

}
