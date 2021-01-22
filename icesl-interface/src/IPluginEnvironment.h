/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once

#include <imgui.h>

namespace IceSLInterface
{

  // ----------------------------------------------------------------

  /**
   * @brief The IPluginEnvironment class is used to share environment for plugins.
   * This is typically used to provide acces to global static variable in the DLL scope.
   * This may include localization information, access to ImGUI for GUI management, etc.
   */
  class IPluginEnvironment
  {
    ImGuiContext* m_ImGuiContext = nullptr;
  public:
    IPluginEnvironment() = default;
    virtual ~IPluginEnvironment() = default;

    void setImGuiContext(ImGuiContext* ctx) { m_ImGuiContext = ctx; }
    ImGuiContext* getImGuiContext() const { return m_ImGuiContext; }
  };

  // ----------------------------------------------------------------

}