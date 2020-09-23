#pragma once

#include "IPluginEnvironment.h"

#include "glux.h"

namespace IceSLInterface
{

static const std::string gInterfaceVersion = "0.2";

/**
 * @brief The IPlugin class is the common interface to all plugins, it contains
 * function for initialization and cleaning of resources.
 */
class IPlugin
{
public:
  IPlugin() = default;
  virtual ~IPlugin() = default;

  /// \brief initialize the plugin, return true if initialization is successful, false otherwise.
  /// If false, IceSL will not use the plugin.
  /// @warning the implementation of this function should call gluxInit() !
  virtual bool initialize(IPluginEnvironment &env) = 0;

  /// \brief Perform some cleanup here, if needed.
  virtual void dispose() = 0;

  /// \brief return the name of the plugin
  virtual std::string name() const = 0;
  /// \brief return the authors of the plugin
  virtual std::string author() const = 0;
  /// \brief return a description of the plugin
  virtual std::string comment() const = 0;

  const std::string  interfaceVersion() const { return gInterfaceVersion; }
};

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

// SP and CZ: 2019-09-27: 
// IMPORTANT!!!
// DO NOT use virtual inheritance (e.g., DerivedClass : public virtual BaseClass)
// when using DerivedClass from a DLL. This renders the DerivedClass virtual
// function table inconsistent and will most likely cause a segmentation fault.


}
