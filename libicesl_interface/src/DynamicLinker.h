#pragma once

#include <map>
#include <memory>
#include <string>

#include "IPlugin.h"

// ----------------------------------------------------------------------------

//class IPlugin;
class IPluginEnvironment;

// ----------------------------------------------------------------------------

/**
 * @brief The DynamicLinker class store IPlugin objects corresponding
 * to plugins loaded in the application. The latter are created after loading
 * dynamic libraries from the "plugins" directory.
 */
class DynamicLinker
{
public:
  /** Call to constructor will load the plugins */
  DynamicLinker(const std::string& appDataPath,
                std::unique_ptr<IPluginEnvironment> pluginEnv);
  ~DynamicLinker();

  bool loadPlugins();

  template<class TPlugin>
  bool addPlugins(const std::string& staticPluginName) {
    IPlugin* plugin = new TPlugin();
    if (plugin->initialize(*m_PluginEnvironment)) {
      m_Plugins[staticPluginName] = plugin;
      return true;
    } else {
      delete plugin;
//      std::cerr << "[DynamicLinker::loadModules] plugin '" << f << "' failed to initialize properly" << std::endl;
      return false;
    }
  }

  /** Call functor function on the plugins deriving from TPluginFilter
   */
  template<typename TPluginFilter, typename TFunctor>
  void processPlugins(TFunctor &&functor) const {
    for (auto & pPlugin : m_Plugins) {
      auto tPlugin = dynamic_cast<TPluginFilter*>(pPlugin.second);
      if(tPlugin) {
        functor(tPlugin);
      }
    }
  }

private:
  const static std::string s_PluginFolder;
  const std::string m_AppDataPath;

  std::unique_ptr<IPluginEnvironment> m_PluginEnvironment;
  std::map<std::string, /*std::shared_ptr<IPlugin>*/IPlugin* > m_Plugins;
  
};

// ----------------------------------------------------------------------------

