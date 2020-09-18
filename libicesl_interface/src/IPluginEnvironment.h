#pragma once

#include "IInterface.h"

#include "Localization.h"
#include "imgui.h"

namespace IceSLInterface
{

// ----------------------------------------------------------------

/**
 * @brief The IPluginEnvironment class is used to share environment for plugins.
 * This is typically used to provide acces to global static variable in the DLL scope.
 * This can includes localization informations, access to ImGUI for GUI management, ...
 */
class IPluginEnvironment : public IInterface
{
public:
  IPluginEnvironment() = default;
  ~IPluginEnvironment() override = default;

  /// @todo if IInterface is kept, implement
  IInterface *cast(const UniqueID *id) override {  return nullptr;  }
};

// ----------------------------------------------------------------

/**
 * \brief Plugin environment for managing translation.
 */
class ILocalizationEnvironment : public virtual IPluginEnvironment
{
public:
  ILocalizationEnvironment(Localization& localization)
    : m_Localization(&localization)
  {}

  ~ILocalizationEnvironment() override = default;

  /// @todo if IInterface is kept, implement
  IInterface *cast(const UniqueID *id) override {  return nullptr;  }

protected:
  Localization* m_Localization;
};

/** \brief Plugin environment for managing GUI through IMGUI library.
 */
class IImGuiEnvironment : public virtual IPluginEnvironment
{
public:
  IImGuiEnvironment()
    : m_ImGuiContext(ImGui::GetCurrentContext())
  {}

  ~IImGuiEnvironment() override = default;

  /// @todo if IInterface is kept, implement
  IInterface *cast(const UniqueID *id) override {  return nullptr;  }

  ImGuiContext* getImGuiContext() {   return m_ImGuiContext;  }

protected:
  ImGuiContext* m_ImGuiContext;
};

// ----------------------------------------------------------------

}
