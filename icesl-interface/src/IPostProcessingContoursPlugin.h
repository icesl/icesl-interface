/*
 *  IceSL plugin interface
 *
 *  Copyright (c) 2026, Inria
 *  See LICENSE file.
 *
 */

#pragma once

#include "IPlugin.h"

#include <LibSL/LibSL.h>

namespace IceSLInterface
{
  /// \brief Interface to post processing of sliced contours
  class IPostProcessingContoursInterface
  {
  public:

    IPostProcessingContoursInterface(const EnumerableSettingsInterface* settings) : m_EnumerableSettings(settings) {}
    virtual ~IPostProcessingContoursInterface() = default;

    /// \brief Post-process all contours. This function is called by IceSL with the output of the slicer after the slicing process is done
    /// \param contour_mm_per_int conversion factor between units used in contours to mm.
    /// \param layer_positions heights (bottom and top) of each layer. <layer_0_bottom, layer_0_top>, ..., <layer_n_bottom,layer_n_top>
    /// \param _contours Contours as output by the slicer. Spec: <layer> x <contours> x <brush, outline>.
    ///                 An outline is defined a list of 2D points
    ///                 A contour is defined as an outline and its associated brush
    ///                 A layer is defined as a list of contours
    ///                 A list of layers is the output of the slicer
    ///                
    ///                 Modify the contents of this parameter with the post-processed contours
    
    virtual bool postProcessContours(
      float                                                         contour_mm_per_int,
      std::vector<std::pair<double,double>>&                        layer_positions,
      std::vector<std::vector<std::pair<int, std::vector<v2i>>>>&  _contours ) = 0;

  protected:

    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface* m_EnumerableSettings;
    
  };

  /// \brief
  class IPostProcessingContoursPlugin : public IPlugin
  {
  public:
      
    IPostProcessingContoursPlugin() = default;
    ~IPostProcessingContoursPlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource management, etc)
    bool initialize(IPluginEnvironment& env) override { return true; }

    /// \brief Implement this function to create and return the post-processing of contours interface
    virtual std::unique_ptr<IPostProcessingContoursInterface> createPostProcessingContours(const IceSLInterface::EnumerableSettingsInterface* settings) = 0;

    /// \brief Called by IceSL to set into m_InternalName the printing setting's internal name assigned to this plugin (do not use)
    void setInternalName(const std::string& internal_name) { m_InternalName = internal_name; }

  protected: 
    /// \brief Printing setting's internal name assigned by IceSL to this plugin
    std::string m_InternalName;
  };

}