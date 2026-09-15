/*
 *  IceSL plugin interface
 *
 *  Copyright (c) 2026, Inria
 *  See LICENSE file.
 *
 */

#pragma once

#include "IPlugin.h"
#include "ILayerSet.h"

#include "EnumerableSettingsInterface.h"

#include <memory>

namespace IceSLInterface
{
  /// \brief General interface for implementing a machine code format exporter as a plugin
  class IMachineCodeInterface
  {
  public:
    /// \brief Initialization of the interface
    /// \param layers set of layers to output
    /// \param settings interface to IceSL slicing settings
    /// \param used_brushes brushes used in the geometry before slicing
    IMachineCodeInterface(
      std::unique_ptr<const ILayerSet>   layers,
      const EnumerableSettingsInterface* settings,
      const std::set<int>&               used_brushes)
      : m_LayerSet(std::move(layers)), m_EnumerableSettings(settings)
    {}

    virtual ~IMachineCodeInterface() = default;

    /// \brief 
    /// \param filename file name if output is a single file, folder name otherwise
    virtual bool startWriting(const std::string& filename) = 0;

    /// \brief Implement incremental steps to write output to file
    virtual bool stepWriting() = 0;

    /// \brief Abort the writing process and clean up
    virtual void abortWriting() = 0;

    /// \brief Queries the writing progress percentage. Output float in the interval [0-1]
    virtual float writingProgress() const = 0;

  protected:
    /// \brief Set of layers to output
    std::unique_ptr<const ILayerSet> m_LayerSet;

    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface* m_EnumerableSettings;
  };

  /**
   * @brief The IMachineCodePlugin class
   */
  class IMachineCodePlugin : public IPlugin
  {
  public:

    IMachineCodePlugin() = default;
    ~IMachineCodePlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource management, etc)
    bool initialize(IPluginEnvironment &env) override { return true; }

    /// \brief Output file extension
    virtual std::string extension() const = 0;

    /// \brief Implement this function to return the machine code writer's implementation of the IMachineCodeInterface interface
    virtual std::unique_ptr<IMachineCodeInterface> createMachineCode(
      std::unique_ptr<const ILayerSet>    layers,
      const EnumerableSettingsInterface*  settings,
      const std::set<int>&                used_brushes) = 0;
  };

}
