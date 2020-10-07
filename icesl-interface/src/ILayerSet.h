/*
 *  Copyright (c) 2020, INRIA
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
  public:

    /// \brief creates an empty layer
    ILayerSet() = default;

    /// \brief destructor
    virtual ~ILayerSet() = default;

    /// \brief returns the number of layers in the print
    virtual int  getNumLayer() const = 0;

    /// \brief returns the i-th layer as a generic layer interface
    virtual const ILayer &getLayer(int) const = 0;
  };

}
