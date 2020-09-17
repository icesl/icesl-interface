#pragma once

#include "IInterface.h"

#include "IPluginEnvironment.h"

#include "glux.h"

namespace IceSLInterface
{

class IPlugin : public IInterface
{
public:
  IPlugin() = default;
  virtual ~IPlugin() = default;

  // Return true if initialization is successful, false otherwise. If false, IceSL will not use the plugin.
  virtual bool initialize(IPluginEnvironment &env) = 0;

  // Perform some cleanup here, if needed.
  virtual void dispose() = 0;

  virtual std::string name() const = 0;
  virtual std::string author() const = 0;
  virtual std::string comment() const = 0;
};

#ifdef WIN32
#define ICESL_PLUGIN_API __declspec(dllexport)
#else
#define ICESL_PLUGIN_API __attribute__((visibility("default")))
#endif

/// Signature of the function required to create the IPlugin object in
/// the dynamic library providing the plugin
#ifdef WIN32
typedef IPlugin*(__cdecl * CREATEPLUGINFUNC)();
#else
typedef IPlugin*(* CREATEPLUGINFUNC)();
#endif

// SP and CZ: 2019-09-27: 
// IMPORTANT!!!
// DO NOT use virtual inheritance (e.g., DerivedClass : public virtual BaseClass)
// when using DerivedClass from a DLL. This renders the DerivedClass virtual
// function table inconsistent and will most likely cause a segmentation fault.

}
