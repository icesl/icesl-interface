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
  /// \brief General interface for implementing an infill in a plugin
  class IInfillerInterface
  {
  public:
    IInfillerInterface() = default;
    virtual ~IInfillerInterface() = default;

    /// \brief Save the interface to access all printing settings
    void setSettings(const EnumerableSettingsInterface *settings) { m_EnumerableSettings = settings; }

    /// \brief Sets the total number of slices, before any path is generated
    virtual void setNumSlices(int num) = 0;

    /// \brief Sets the world space information, before any path is generated
    /// This allows to go from build space to world space, for intance to query per-layer or field parameters.
    virtual void setWorldSpace(v3f world_corner_mm, v3f extent_mm) = 0;

    /// \brief Called when the infill is about to be used for a brush, prior to path generation
    virtual void prepareInfill(int brush) = 0;

    /// \brief Called at the end of path generation for this brush
    virtual void terminateInfill(int brush) = 0;

    /// \brief Called when the path generation is about to run for a given slice
    virtual void prepareInfillForSlice(int id, const AAB<2, int> &xy_slice_box, float layer_height_mm, double layer_thickness_mm, int brush) = 0;

    /// \brief Called on each surface within a slice, asking for the infill to be generated within
    /// \param id the slice id (starts at 0)
    /// \param layer_height_mm altitude of the layer's bottom in mm, build space (starts at 0.0)
    /// \param layer_thickness_mm thickness of the layer in mm
    /// \param brush the brush from which the infiller is called
    /// \param surface the geometric outline of the area to be filled
    /// \param fills the vector to be filled with paths.
    /// \param preserve_order set to true if the order is to be preserved (defaults to false)
    ///
    /// The method returns true if an infill was produced, false if the infill should be replaced
    /// by a dense infill using IceSL's built-in infiller.
    ///
    virtual bool generateInfill(
      int    id,
      float  layer_height_mm,
      double layer_thickness_mm,
      int    brush,
      const ClipperLib::Paths &surface,
      std::vector<std::unique_ptr<IPath> > &fills,
      bool                                 &preserve_order,
      ClipperLib::Paths &fallback_surface) = 0;

  protected:

    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface* m_EnumerableSettings;
  };

  /**
   * @brief The IPluginInfiller class
   */
  class IInfillerPlugin : public IPlugin
  {
  public:

    IInfillerPlugin() = default;
    ~IInfillerPlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource management, etc)
    bool initialize(IPluginEnvironment &env) override { return true; }

    /// \brief Implement this function to return the implementation of the infiller interface
    virtual std::unique_ptr<IInfillerInterface> createInfiller() = 0;
  };

}
