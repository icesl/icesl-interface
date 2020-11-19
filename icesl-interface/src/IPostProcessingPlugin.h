/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once 

#include "IPlugin.h"
#include "ILayerSet.h"

#include "EnumerableSettingsInterface.h"

namespace IceSLInterface
{
  /// \brief Intialization of the interface to post processing of sliced paths
  class IPostProcessingInterface
  {
  public:

    /// \brief Initialization of the interface
    /// \param layers set of layers to post-process
    IPostProcessingInterface(
      std::unique_ptr<ILayerSet>         layers,
      const EnumerableSettingsInterface* settings)
      : m_LayerSet(std::move(layers)), m_EnumerableSettings(settings)
    {}

    virtual ~IPostProcessingInterface() = default;

    /// \brief PostProcess layer.
    /// \param layers
    /// \result true if done processing all layers (i.e., interface won't be called anymore)
    virtual bool postProcessLayers() = 0;

    /// \brief Progess of post-processing all layers
    /// \result decimal in the interval [0-1] as a normalized percentage of progess
    virtual float postProcessingProgress() const = 0;

  protected:

    /// \brief Set of layers to post-process
    std::unique_ptr<ILayerSet> m_LayerSet;

    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface* m_EnumerableSettings;
  };


  /// \brief 
  class IPostProcessingPlugin : public IPlugin
  {
  public:

    IPostProcessingPlugin() = default;
    ~IPostProcessingPlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource management, etc)
    bool initialize(IPluginEnvironment& env) override { return true; }

    /// \brief Implement this function to return the implementation of the post-processing interface 
    virtual std::unique_ptr<IPostProcessingInterface> createPostProcessing(
      std::unique_ptr<ILayerSet> layers,
      const IceSLInterface::EnumerableSettingsInterface* settings) const = 0;

    /// \brief Called by IceSL to set into m_InternalName the printing setting's internal name assigned to this plugin (do not use)
    void setInternalName(const std::string& internal_name) { m_InternalName = internal_name; }

  protected:
    /// \brief Printing setting's internal name assigned by IceSL to this plugin 
    std::string m_InternalName;
  };


}