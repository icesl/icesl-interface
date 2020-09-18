// SP and CZ 2020-09-16
#pragma once

#include <LibSL/LibSL.h>
#include <string>

namespace IceSLInterface
{

/**
 * @brief The EnumerableSettingsInterface class is used to access
 * printing settings from plugins.
 */
class EnumerableSettingsInterface
{
public:
  /**
   * @brief The SettingInterface class encapsulates a single printing setting
   */
  class SettingInterface
  {
  public:
    virtual ~SettingInterface() = default;

    /// \brief check is the setting is constant in the printing volume
    bool isConstantSetting() const
    {
      return !(isPerLayerSetting() || isFieldSetting());
    }
    /// \brief check is the setting varies per layer within the printing volume
    virtual bool isPerLayerSetting() const = 0;
    /// \brief check if the setting is described by a volumetric field in the printing volume
    virtual bool isFieldSetting() const = 0;

    /// \brief get constant setting value based on type, undefined behavior if the setting
    /// does not have the right type.
    virtual void getValue(bool& value) const = 0;
    virtual void getValue(int& value) const = 0;
    virtual void getValue(float& value) const = 0;
    virtual void getValue(std::string& value) const = 0;

    /// \brief get setting value at specified height (h),
    /// require setting to be of type 'float' with Constant or PerLayer variation
    virtual double valueAtHeight(double h) const = 0;
    /// \brief get setting value at specified position (pos)
    /// require setting to be of type 'float' with Field variation
    virtual double valueAtPosition(v3f pos) const = 0;
  };

public:
  virtual ~EnumerableSettingsInterface() = default;

  /// \brief get a setting from its internal name, see https://icesl.loria.fr/parameters/ for the list of internal names.
  /// A null pointer is returned if the setting does not exist.
  virtual std::unique_ptr<SettingInterface> getSettingByName(const std::string& internalName) const = 0;
};

}
