/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

// SL 2020-09-07
#pragma once

#include "IPath.h"

namespace IceSLInterface
{

  /// \brief Interface to a layer
  /**
     Currently only used as const by plugins.
     Future plugins may be allowed to change and re-order the set of paths.

     A layer represents a planar thick slab of a part, covered with paths.
     The slab is in Z interval [ zTopBuildSpace , zTopBuildSpace + thickness ].

     The layer uses two coordinate systems:
     - build space is in mm, starts on bed at 0.0
     - world space is in mm, corresponds to the input part coordinate system

     The path vertices XY coordinate have to be translated into mm multiplying
     by the integer grid scale factor xy_mm_per_int. After this, the coordinates
     are in build space.

     IceSL provides the following attributes:
     - "is_spiral", != 0.0 if the layer is a spiral layer (auto-spiralize feature)
  */
  class ILayer
  {
  public:

    /// \brief creates an empty layer
    ILayer() = default;

    /// \brief destructor
    virtual ~ILayer() = default;

    /// \brief returns the position of the layer top in build space
    virtual double zTopBuildSpace() const = 0;

    /// \brief returns the position of the layer top in world space
    virtual double zTopWorldSpace() const = 0;

    /// \brief returns the thickness of the layer
    virtual double thickness() const = 0;

    /// \brief returns the number of paths in the layer
    virtual int  getNumPaths() const = 0;

    /// \brief returns the i-th path as a generic path interface
    virtual const IPath &getPath(int) const = 0;

    /// \brief returns the layer attributes names
    virtual void getLayerAttributes(std::vector<std::string> &) const = 0;

    /// \brief returns the index of the layer attributes, -1 if unknown
    virtual int  getLayerAttributeIndex(const std::string &) const = 0;

    /// \brief gets the value of the layer attribute
    virtual void getLayerAttributeValue(int, double &) const = 0;

  };

}
