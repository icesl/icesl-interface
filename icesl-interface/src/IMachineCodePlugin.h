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

#include <memory>

namespace IceSLInterface
{
  /// \brief General interface for implementing a machine code exporter in a plugin
  class IMachineCodeInterface
  {
  protected:
    /// \brief Initialize with the interface to access all printing settings
    IMachineCodeInterface(std::unique_ptr<ILayerSet> layers,
                          const EnumerableSettingsInterface *settings,
                          const std::set<int>&               used_brushes)
      : m_LayerSet(std::move(layers)), m_EnumerableSettings(settings) {}

  public:
    virtual ~IMachineCodeInterface() = default;

    /// \brief
    virtual bool startWriting(const std::string& filename) = 0;

    /// \brief
    virtual bool stepWriting() = 0;

    /// \brief
    virtual void abortWriting() = 0;

    /// \brief
    virtual float writingProgress() const = 0;

  protected:
    /// \brief Set of layers to be exported
    std::unique_ptr<ILayerSet> m_LayerSet;
    /// \brief Use this interface to access or add printing settings as well as query # of brushes or extruders (where applicable)
    const EnumerableSettingsInterface *m_EnumerableSettings;
  };

  /**
   * @brief The IMachineCodePlugin class
   */
  class IMachineCodePlugin : public IPlugin
  {
  public:

    IMachineCodePlugin() = default;
    ~IMachineCodePlugin() override = default;

    /// \brief Called upon plugin initialization (e.g., resource managment, etc)
    bool initialize(IPluginEnvironment &env) override
    {
      return true;
    }

    /// \brief Implement this function to add printing settings specific to the infiller using the EnumerableSettingsInterface interface
    virtual bool addMachineCodeSettings(EnumerableSettingsInterface& enumerable_settings) = 0;

    /// \brief Implement this function to return the machine code writer's implementation of the IMachineCodeInterface interface
    virtual std::unique_ptr<IMachineCodeInterface> createMachineCodeWriter(std::unique_ptr<IceSLInterface::ILayerSet> layers,
                                                                           const EnumerableSettingsInterface *settings,
                                                                           const std::set<int>&               used_brushes) const = 0;
  };

}
