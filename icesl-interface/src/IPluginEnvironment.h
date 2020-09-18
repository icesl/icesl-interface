#pragma once

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
class IPluginEnvironment
{
public:
  IPluginEnvironment() = default;
  virtual ~IPluginEnvironment() = default;
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

  ImGuiContext* getImGuiContext() {   return m_ImGuiContext;  }

protected:
  ImGuiContext* m_ImGuiContext;
};

// ----------------------------------------------------------------

}
