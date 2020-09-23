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
     * @brief Slicing settings types, units and flags.
     */
    enum e_Type { e_Empty, e_Float, e_Int, e_Bool, e_Text, e_Select, e_Ratios, e_Field, e_Button, e_OverhangPointList };
    enum e_Units { e_NoUnit, e_MM, e_CDegrees, e_MMperSec, e_Percent, e_Zoning, e_Angle };
    enum e_Flags {
      e_NoFlag = 0,
      e_CanBePerLayer = 1, // can switch to a per-layer setting
      e_Internal = 2,      // never shown in interface
      e_CanBeField = 4,    // can switch to a field setting
      e_HasParameters = 8  // function triggered by button has parameters
    };

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

      /// \brief get the setting's type, unit and flags
      virtual e_Type  getType()  const = 0;
      virtual e_Units getUnits() const = 0;
      virtual e_Flags getFlags() const = 0;

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

    /// \brief add a slicing setting to IceSL that can be set by the user through UI or script
    ///        only settings of types e_Int, e_Float or e_Bool can be added through this function
    template<typename T>
    std::unique_ptr<SettingInterface> addSettingFromInterface(
      T& rawptr, T& rawptr_min, T& rawptr_max, // pointer to the current value of the setting as well as its minimum and maximum value
      const std::string& internalName,         // internal name (also used in setting it through script). Should not collapse with IceSL's settings (see website mentioned above)
      const std::string& displayName,          // UI name
      const std::string& group,                // group whom the setting belongs to. Look for possible groups in the website mentioned above in the setting called 'groups'
      const std::string& description,          // UI description (i.e., tooltip)
      int rank,                                // UI rank within the group. The lower it is, the sooner it is listed in the group section in the UI's parameter panel
      std::function<bool()> active_if          // lambda function to decide whether to show or hide the setting in the UI (e.g., dependent on another parameter). It is executed every frame
      = std::function<bool()>(),
      e_Units units = e_NoUnit,                // setting's units
      e_Flags flags = e_NoFlag                 // setting's flags
    ) const
    {
      return addTypedSettingFromInterface((e_Type)TypeAsEnumeration<T>::v, 
        static_cast<void*>(&rawptr), static_cast<void*>(&rawptr_min), static_cast<void*>(&rawptr_max),
        internalName, displayName, group, description, rank, active_if, units, flags);
    }
    /// @TODO: add abstraction of SettingOptimizer class in order to include it when adding a setting

    /// \brief add a selection setting (i.e., e_Select) similar to a pop-up menu to IceSL. It can be set through UI or script as well
    virtual std::unique_ptr<SettingInterface> addSelectionFromInterface(
      int* rawptr,                                  // pointer to index of parameter 'selections'. Represents the selection currently chosen
      const std::string& internalName,              // internal name (also used in setting it through script). Should not collapse with IceSL's settings (see website mentioned above)
      const std::vector<std::string>& selections,   // possible selections for this settings
      const std::string& displayName,               // UI name
      const std::string& group,                     // group whom the setting belongs to. Look for possible groups in the website mentioned above in the setting called 'groups'
      const std::string& description,               // UI description (i.e., tooltip)
      int rank,                                     // UI rank within the group. The lower it is, the sooner it is listed in the group section in the UI's parameter panel
      void(*selection_callback)(void*) = nullptr,   // callback function whenever the selection is changed
      void* userdata = nullptr,                     // user data passed as parameter to the callback function
      std::function<bool()> active_if               // lambda function to decide whether to show or hide the setting in the UI (e.g., dependent on another parameter). It is executed every frame
       = std::function<bool()>(),
      e_Units units = e_NoUnit,                     // setting's units
      e_Flags flags = e_NoFlag                      // setting's flags
    ) = 0;

  private:

    /// \brief abstain from using
    virtual std::unique_ptr<SettingInterface> addTypedSettingFromInterface(
      e_Type type, void* rawptr, void* rawptr_min, void* rawptr_max,
      const std::string& internalName, const std::string& displayName, const std::string& group, const std::string& description,
      int rank, std::function<bool()> active_if, e_Units units, e_Flags flags ) = 0;

    template<typename T> class TypeAsEnumeration;
  };

  template<> class EnumerableSettingsInterface::TypeAsEnumeration<float> { public: enum { v = e_Float }; };
  template<> class EnumerableSettingsInterface::TypeAsEnumeration<int>   { public: enum { v = e_Int   }; };
  template<> class EnumerableSettingsInterface::TypeAsEnumeration<bool>  { public: enum { v = e_Bool  }; };
}
