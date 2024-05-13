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
#include "IPath.h"

#include "EnumerableSettingsInterface.h"

namespace IceSLInterface
{
  /// \brief Intialization of the interface to post processing of sliced paths
  class IPostProcessingInterface
  {
  public:

    /// \brief Initialization of the interface
    /// \param layers set of layers to post-process
    IPostProcessingInterface(const EnumerableSettingsInterface* settings) : m_EnumerableSettings(settings) {}

    virtual ~IPostProcessingInterface() = default;

    /// \brief PostProcess layer on a per-extruder basis. This function is called by IceSL for each layer (top to bottom) for each extruder that deposits material.
    /// \param extruder extruder used in subset '_layer' of the whole layer
    /// \param layerID layer number
    /// \param spiral if layer is to be spiralized
    /// \param last_part on few cases, IceSL separates in different parts an extruder's deposition paths in a layer (e.g., support structure). This boolean indicates if '_layers' is the last part of extruder 'extruder' in layer 'layerID'
    /// \param start starting point of paths in _layer
    /// \param layer_height_mm altitude of the layer's bottom in mm, build space (starts at 0.0)
    /// \param layer_thickness_mm thickness of the layer in mm
    /// \param layer subset of layer to post-process (add paths + reorder existing paths)
    /// \result true post-processing succesful
    virtual bool postProcessLayer(
      const int extruder,
      const int layerID,
      bool& spiral,
      const bool last_part,
      const v2i& start,
      float layer_height_mm,
      double layer_thickness_mm,
      std::vector<std::shared_ptr<IceSLInterface::IPath>>& _layer) = 0;

  protected:

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
    virtual std::unique_ptr<IPostProcessingInterface> createPostProcessing(const IceSLInterface::EnumerableSettingsInterface* settings) = 0;

    /// \brief Called by IceSL to set into m_InternalName the printing setting's internal name assigned to this plugin (do not use)
    void setInternalName(const std::string& internal_name) { m_InternalName = internal_name; }

  protected:
    /// \brief Printing setting's internal name assigned by IceSL to this plugin 
    std::string m_InternalName;
  };


}
