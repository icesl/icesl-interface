// SP and CZ 2020-09-16
#pragma once

#include <LibSL/LibSL.h>
#include <string>

/**
 * @brief The EnumerableSettingsInterface class is used
 * to access printing settings from plugins.
 */
class EnumerableSettingsInterface
{
public:
  /**
   * @brief The SettingInterface class encapsulates a single printing setting
   * @todo add possibility to access the actual type of a setting
   * @todo the way accessing the spatial variation of parameter in space might change
   */
  class SettingInterface
  {
  public:
    virtual ~SettingInterface() = default;

    bool isConstantSetting() const
    {
      return !(isPerLayerSetting() || isFieldSetting());
    }
    virtual bool isPerLayerSetting() const = 0;
    virtual bool isFieldSetting() const = 0;

    /// \brief get constant setting value based on type
    /// @todo interface need improvements
    virtual void getValue(bool& value) const = 0;
    virtual void getValue(int& value) const = 0;
    virtual void getValue(float& value) const = 0;
    virtual void getValue(std::string& value) const = 0;

    /// \brief get setting value at specified height (h),
    /// require setting to be of type 'double', and Constant or PerLayer
    virtual double valueAtHeight(double h) const = 0;
    /// \brief get setting value at specified position (pos)
    /// require setting to be of type 'double'
    virtual double valueAtPosition(v3f pos) const = 0;
  };

public:
  virtual ~EnumerableSettingsInterface() = default;

  /// \brief get a setting from its internal name, see @todo for the list of internal names.
  /// A null pointer is returned if the setting does not exist.
  virtual std::unique_ptr<SettingInterface> getSettingByName(const std::string& internalName) const = 0;
};
