/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once

#include "IPluginEnvironment.h"
#include "EnumerableSettingsInterface.h"
#include "glux.h"

namespace IceSLInterface
{
  extern const std::string InterfaceVersion;

/**
 * @brief The IPlugin class is the common interface to all plugins, it contains
 * functionality for initialization and cleaning of resources.
 */
class IPlugin
{
public:
  IPlugin() = default;
  virtual ~IPlugin() = default;

  /// \brief Initialize the plugin, return true if initialization is successful, false otherwise.
  /// If false, IceSL will omit the plugin.
  /// @warning the implementation of this function should call gluxInit() !
  virtual bool initialize(IPluginEnvironment &env) = 0;

  /// \brief Perform cleanup here when applicable
  virtual void dispose() = 0;

  /// \brief Add plugin settings to IceSL
  virtual bool addPluginSettings(EnumerableSettingsInterface& enumerable_settings) = 0;

  /// \brief Return the name of the plugin
  virtual std::string name() const = 0;
  /// \brief Return the authors of the plugin
  virtual std::string author() const = 0;
  /// \brief Return a description of the plugin
  virtual std::string comment() const = 0;
  /// \brief Return the plugin's GUID. Abstain from using
  virtual std::string guid() const { return std::string(); }

  /// \brief Return the interface version used in the plugin
  const std::string& interfaceVersion() { return m_InterfaceVersion; }

private:
  /// \brief Internal. Abstain from using
  const std::string m_InterfaceVersion = InterfaceVersion;
};

/// Signatures of the functions required to create and destroy
/// the IPlugin object from the host application, typically defined as
///    MyPlugin.h
///       extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();
///       extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);
///    MyPlugin.cpp
///       IceSLInterface::IPlugin* createPlugin()
///       {
///         return new MyPlugin();
///       }
///       void destroyPlugin(IceSLInterface::IPlugin* plugin)
///       {
///         delete plugin;
///       }

#ifdef WIN32
#define ICESL_PLUGIN_API __declspec(dllexport)
#else
#define ICESL_PLUGIN_API __attribute__((visibility("default")))
#endif

#ifdef WIN32
typedef IPlugin*(__cdecl * CREATEPLUGINFUNC)();
typedef void (__cdecl * DESTROYPLUGINFUNC)(IPlugin*);
#else
typedef IPlugin*(* CREATEPLUGINFUNC)();
typedef void (* DESTROYPLUGINFUNC)(IPlugin*);
#endif


// SP and CZ: 2019-09-27: 
// IMPORTANT!!!
// DO NOT use virtual inheritance (e.g., DerivedClass : public virtual BaseClass)
// when using DerivedClass from a DLL. This renders the DerivedClass virtual
// function table inconsistent and will most likely cause a segmentation fault.


}
