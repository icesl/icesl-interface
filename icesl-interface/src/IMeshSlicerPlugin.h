/*
 *  Copyright (c) 2023, Inria
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once

#include "IPlugin.h"
#include "ISlicingPlan.h"

#include <LibSL/LibSL.h>

#include <memory>

namespace IceSLInterface
{
  /// \brief Contour class
  ///        A contour is a closed 2D polygon that is the result
  ///        of slicing a closed shape at a certain height.
  /// 
  ///        IMPORTANT: Contours MUST BE in CLOCKWISE orientation
  class Contour
  {
  public:

    /// \brief Clockwise contour outline accessor
    std::vector<v2i>& outline() { return m_Outline; }
    const std::vector<v2i>& outline() const { return m_Outline; }

  private:
    std::vector<v2i> m_Outline;
  };


  /// \brief Interface to a mesh slicer
  class IMeshSlicerInterface
  {
  public:
    IMeshSlicerInterface() = default;

    /// \brief Destructor
    ///
    /// If the user cancels the slicing process through any UI element, the destructor is also called.
    /// The plugin should clean up any resources active at invocation of the constructor (e.g., memory, threads, etc)
    virtual ~IMeshSlicerInterface() = default;

    /// \brief Save the interface to access all printing settings
    void setSettings(const EnumerableSettingsInterface* settings) { m_EnumerableSettings = settings; }

    /// \brief Start the process of slicing a mesh with respect to a slicing plan
    /// \param slicing_plan slicing plan to use
    /// \param mesh mesh to slice 
    /// 
    /// Calling this method renders the object ready to commence a slicing process of a certain mesh.
    /// Subsequent calls to 'processSlicing' calculate progressively the resulting slices.
    /// Calling to 'slice' after this method nulifies it.
    /// 
    virtual void startSlicing(const ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh) = 0;

    /// \brief Advance the slicing process progressively
    /// \return true if slicing is done, false otherwise
    /// 
    /// Each call advances the slicing process up until completion when 'true' is returned.
    /// Calling to 'slice' after this method nulifies the progress.
    /// 
    virtual bool processSlicing() = 0;

    /// \brief Slice mesh with respecto to a slicing plan
    /// \param slicing_plan slicing plan to use
    /// \param mesh mesh to slice 
    /// 
    /// Executes until slicing is done. Results are stored inside the object and can be accessed with method 'contours'.
    /// One call to this method is sufficient to complete the slicing process. 
    /// 
    /// 
    virtual void slice(const ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh) = 0;

    /// \brief Queries result after slicing mesh
    /// \return Slices containing the resulting clockwise contours
    /// 
    /// First element of return data corresponds to the bottom slice. Each element might have more than one contour to represent disjointed shapes.
    /// Remember that contours are clockwise
    /// 
    virtual std::vector<std::vector<Contour>>& slices() = 0;
    virtual const std::vector<std::vector<Contour>>& slices() const = 0;

    /// \brief Queries the number of steps the whole slicing process takes
    /// \return total number of steps to completely slice a mesh
    virtual unsigned int getNumOfProcessingSteps() const = 0;

    /// \brief Queries the current step number the slicing process is in
    /// \return current step number
    /// 
    /// Only produces meaningful data when calling with 'progressSlicing'
    /// 
    virtual unsigned int getCurrentProcessingStep() const = 0;

    /// \brief Queries the normalize percentage progress of the current step the slicing process is in
    /// \return progress percentage normalized (i.e., 0 means %0 and 1 means %100)
    virtual float getCurrentProcessingStepProgress() const = 0;

    /// \brief Queries the current step description
    /// \return Human readable string describing the current step the slicing process is in
    virtual std::string getCurrentProcessingStepDescription() const = 0;

    /// \brief Accessor to scale of mesh units (mm) with respect to contour units (int)
    /// \return mm to int conversion factor
    /// 
    /// The lower this conversion factor is, the more detailed the contours output is.
    /// 
    virtual float& MMperInt() = 0;

  protected:

    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface* m_EnumerableSettings = nullptr;
  };

  /**
  * @brief The IMeshSlicerPlugin class
  */
  class IMeshSlicerPlugin : public IPlugin
  {
  public:

    IMeshSlicerPlugin() = default;
    ~IMeshSlicerPlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource management, etc)
    bool initialize(IPluginEnvironment& env) override { return true; }

    /// \brief Implement this function to return the implementation of the post-processing interface 
    virtual std::unique_ptr<IMeshSlicerInterface> createMeshSlicer() = 0;

  protected:

    /// \brief This should only be used as a buffer to pass the settings' interface to the plugin inteface.
    ///        It is supposed to be set to its real value in 'addPluginSettings' and passed to the slicer plugin in 'createMeshSlicer'
    const EnumerableSettingsInterface* m_EnumerableSettings = nullptr;
  };



}