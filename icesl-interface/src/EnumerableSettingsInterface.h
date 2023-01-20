/*
 *  Copyright (c) 2023, Inria
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

// SP and CZ 2020-09-16
#pragma once

#include <LibSL/LibSL.h>
#include <string>
#include <functional>

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
    enum e_Type { e_Empty, e_Float, e_Int, e_Bool, e_Text, e_Select, e_Ratios, e_Field, e_Button, e_OverhangPointList, e_Frame };
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
      virtual bool isFieldSetting()    const = 0;

      /// \brief get constant setting value based on type, undefined behavior if the setting
      /// does not have the right type.
      virtual void getValue(bool& value)        const = 0;
      virtual void getValue(int& value)         const = 0;
      virtual void getValue(float& value)       const = 0;
      virtual void getValue(std::string& value) const = 0;

      /// \brief set constant setting value based on type, undefined behavior if the setting
      /// does not have the right type.
      virtual void setValue(bool value)        = 0;
      virtual void setValue(int value)         = 0;
      virtual void setValue(float value)       = 0;
      virtual void setValue(std::string value) = 0;

      /// \brief get the setting's type, unit and flags
      virtual e_Type  getType()  const = 0;
      virtual e_Units getUnits() const = 0;
      virtual e_Flags getFlags() const = 0;

      /// \brief get setting value at specified height (h),
      /// require setting to be of type 'float' with Constant or PerLayer variation
      virtual double valueAtHeight(double h)  const = 0;
      /// \brief get setting value at specified position (pos)
      /// require setting to be of type 'float' with Field variation
      virtual double valueAtPosition(v3f pos) const = 0;
    };

  public:
    virtual ~EnumerableSettingsInterface() = default;

    /// \brief get a setting from its internal name, see https://icesl.loria.fr/parameters/ for the list of internal names.
    /// A null pointer is returned if the setting does not exist.
    virtual std::unique_ptr<SettingInterface> getSettingByName(const std::string& internalName) const = 0;

    /// \brief Add a slicing setting to IceSL that can be set by the user through UI or script.
    ///        Only settings of types e_Int, e_Float or e_Bool can be added through this function.
    ///        The setting's value as well as its minimum and maximum bounds are kept (by reference)
    ///        by IceSL, thus the plugin coder is responsable for keeping their used memory alive during
    ///        IceSL's execution
    /// \param value pointer to the current value of the setting
    /// \param value_min pointer to the minimum value of the setting
    /// \param value_max pointer to the maximum value of the setting
    /// \param internalName internal name (also used in setting it through script). Should not collapse with IceSL's settings (see website mentioned above)
    /// \param displayName UI name
    /// \param group group whom the setting belongs to. Look for possible groups in the website mentioned above in the setting called 'groups'
    /// \param description UI description (i.e., tooltip)
    /// \param rank UI rank within the group. The lower it is, the sooner it is listed in the group section in the UI's parameter panel
    /// \param active_if lambda function to decide whether to show or hide the setting in the UI (e.g., dependent on another parameter). It is executed every frame
    /// \param units setting's units
    /// \param flags setting's flags
    template<typename T>
    std::unique_ptr<SettingInterface> addSettingFromPlugin(
      T* value, T* value_min, T* value_max,
      const std::string& internalName,
      const std::string& displayName,
      const std::string& group,
      const std::string& description,
      int rank,
      std::function<bool()> active_if = std::function<bool()>(),
      e_Units units = e_NoUnit,
      e_Flags flags = e_NoFlag 
    ) {
      return addTypedSettingFromPlugin((e_Type)TypeAsEnumeration<T>::v,
        static_cast<void*>(value), static_cast<void*>(value_min), static_cast<void*>(value_max),
        internalName, displayName, group, description, rank, active_if, units, flags);
    }

    /// @TODO: add abstraction of SettingOptimizer class in order to include it when adding a setting

    /// \brief add a selection setting (i.e., e_Select) similar to a pop-up menu to IceSL. It can be set through UI or script as well
    /// \param pointer to index of parameter 'selections'. Represents the selection currently chosen
    /// \param internalName internal name (also used in setting it through script). Should not collapse with IceSL's settings (see website mentioned above)
    /// \param displayName UI name
    /// \param group group whom the setting belongs to. Look for possible groups in the website mentioned above in the setting called 'groups'
    /// \param description UI description (i.e., tooltip)
    /// \param rank UI rank within the group. The lower it is, the sooner it is listed in the group section in the UI's parameter panel
    /// \param active_if lambda function to decide whether to show or hide the setting in the UI (e.g., dependent on another parameter). It is executed every frame
    /// \param units setting's units
    /// \param flags setting's flags
    virtual std::unique_ptr<SettingInterface> addSelectionFromPlugin(
      int* value,
      const std::string& internalName,
      const std::vector<std::string>& selections,
      const std::string& displayName,
      const std::string& group,
      const std::string& description,
      int rank,
      void(*selection_callback)(void*) = nullptr,
      void* userdata = nullptr,
      std::function<bool()> active_if = std::function<bool()>(),
      e_Units units = e_NoUnit,
      e_Flags flags = e_NoFlag
    ) = 0;

    /// \brief queries the number of available brushes
    virtual int getNumberOfBrushes()   const { return 0; }
    /// \brief queries the number of available extruders (when available)
    virtual int getNumberOfExtruders() const { return 0; }

  private:

    /// \brief abstain from using
    virtual std::unique_ptr<SettingInterface> addTypedSettingFromPlugin(
      e_Type type, void* rawptr, void* rawptr_min, void* rawptr_max,
      const std::string& internalName, const std::string& displayName, const std::string& group, const std::string& description,
      int rank, std::function<bool()> active_if, e_Units units, e_Flags flags ) = 0;

    template<typename T> class TypeAsEnumeration;
  };

  template<> class EnumerableSettingsInterface::TypeAsEnumeration<float> { public: enum { v = e_Float }; };
  template<> class EnumerableSettingsInterface::TypeAsEnumeration<int>   { public: enum { v = e_Int   }; };
  template<> class EnumerableSettingsInterface::TypeAsEnumeration<bool>  { public: enum { v = e_Bool  }; };
}
