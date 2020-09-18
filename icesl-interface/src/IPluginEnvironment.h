#pragma once

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

}