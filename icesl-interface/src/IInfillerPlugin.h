#pragma once

#include "IPlugin.h"
#include "IPath.h"

#include "EnumerableSettingsInterface.h"

namespace IceSLInterface
{

  /**
   * @brief The IPluginInfiller class
   */
  class IInfillerPlugin : public IPlugin
  {
  public:
    /// \brief Constructor
    IInfillerPlugin() = default;
    /// \brief Desstructor
    ~IInfillerPlugin() override = default;

    /// \brief Called upon plugin initialization
    bool initialize(IPluginEnvironment &env) override
    {
      return true;
    }

    /// \brief Caller sets the enumerable settings (all print settings can be accessed through this interface)
    void setSettings(const EnumerableSettingsInterface *settings) { m_EnumerableSettings = settings; }

    /// \brief Caller sets the total number of slices, before any path is generated
    virtual void setNumSlices(int num) = 0;

    /// \brief Caller sets the world space information, before any path is generated
    /// This allows to go from build space to world space, for intance to query per-layer or field parameters.
    virtual void setWorldSpace(v3f world_corner_mm, v3f extent_mm) = 0;

    /// \brief Called when the infill is about to be used for a brush, prior to path generation
    virtual void prepareInfill(int brush) = 0;

    /// \brief Called at the end of path generation for this brush
    virtual void terminateInfill(int brush) = 0;

    /// \brief Called when the path generation is about to run for a given slice
    virtual void prepareInfillForSlice(int id, const AAB<2, int> &xy_slice_box, float height, int brush) = 0;

    /// \brief Called on each surface within a slice, asking for the infill to be generated within
    /// \param id the slice id (starts at 0)
    /// \param height_mm altitude of the layer bottm in mm, build space (starts at 0.0)
    /// \param brush the brush for which the infiller is called
    /// \param surface the geometric outline of the area to be filled
    /// \param fills the vector to be filled with passes.
    ///
    /// Note that fills is a vector of Paths. Each vector entry correspond to a pass of printing infill
    /// paths. For instance, an infill producing a crossed hatch pattern will have two sets of Paths, the
    /// first for the horizontal pass, the second for the vertical pass. The first entry in the vector
    /// always prints before the second entry, and so on.
    ///
    /// The method returns true if a infill was produced, false if the infill should be replaced
    /// by a dense infill using IceSL built-in infiller.
    ///
    virtual bool generateInfill(
      int   id,
      float height_mm,
      int   brush,
      const ClipperLib::Paths &surface,
      std::vector<std::unique_ptr<IPath> > &fills) = 0;

  protected:

    //IceSLInterface:: @todo add namespace
    const EnumerableSettingsInterface *m_EnumerableSettings;
  };

}
