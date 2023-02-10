/*
 *  Copyright (c) 2023, Inria
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */
#pragma once

#include "ILayer.h"

namespace IceSLInterface
{

  /// \brief Interface to a layer set
  /**
     Currently only used as const by plugins.
     Future plugins may be allowed to change and re-order the set of paths.
  */
  class ILayerSet
  {
  protected:
    /// \brief creates an empty layer
    ILayerSet() = default;
  public:
    /// \brief destructor
    virtual ~ILayerSet() = default;

    /// \brief returns the number of layers in the print
    virtual int  getNumLayers() const = 0;

    /// \brief returns the i-th layer as a generic layer interface
    virtual const ILayer &getLayer(int) const = 0;

    /// \brief returns the i-th layer as a generic layer interface
    virtual ILayer& getLayer(int) = 0;

    /// \brief convert internal coordinates (integer) to build space coordinates
    virtual v2f xyBuildSpace(ClipperLib::IntPoint) const = 0;

    /// \brief convert internal coordinates (integer) to world space coordinates
    virtual v2f xyWorldSpace(ClipperLib::IntPoint) const = 0;

  };

}
