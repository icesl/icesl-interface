#pragma once

#include "IPlugin.h"
#include "IPath.h"

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

  bool initialize(IPluginEnvironment &env) override 
  {
    return true;
  }

  void setSettings(const EnumerableSettingsInterface* settings) { m_EnumerableSettings = settings; }

  virtual void setNumSlices(int num) = 0;
  virtual void setWorldSpace(v3f world_corner_mm, v3f extent_mm) = 0;

  // called once at start
  virtual void prepareInfill(int brush) = 0;

  // called once at end
  virtual void terminateInfill(int brush) = 0;

  // called before each new slice
  virtual void prepareInfillForSlice(int id, const AAB<2, int>& xy_slice_box, float height, int brush) = 0;

  // called to generate the infill in a surface
  /// TODO: add boolean to indicate if order is to be preserved or not
  /// TODO: clarify return boolean
  virtual bool generateInfill(int id, float height_mm, int brush, const ClipperLib::Paths& surface, std::vector<std::unique_ptr<IPath> >& fills) = 0;

protected:

  //IceSLInterface:: @todo add namespace
  const EnumerableSettingsInterface* m_EnumerableSettings;
};

}
