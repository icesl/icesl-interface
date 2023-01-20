/*
 *  Copyright (c) 2023, Inria
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once

#include <utility>

namespace IceSLInterface
{
  /// \brief Interface to a slicing plan
  class ISlicingPlan
  {
  public:
    ISlicingPlan() = default;
    virtual ~ISlicingPlan() = default;

    /// \brief Queries the number of slices in the slicing plan
    /// \return number of slices
    virtual unsigned int getNumOfSlices() const = 0;

    /// \brief Queries the start and end height points of a given slice in the slicing plan
    /// \param slice id of slice. 0 means the bottom-most slice
    /// \return <height of bottom, height of top>. Both values are in millimeters
    virtual std::pair<double, double> getHeightRangeOfSlice(unsigned int slice) const = 0;

    /// \brief Queries the interval of slices that cover a certain height range in the slicing plan
    /// \param from_mm bottom (in millimeters) of height range
    /// \param to_mm top (in millimeters) of height range
    /// \return <bottom slice id, top slice id>
    virtual std::pair<unsigned int, unsigned int> getSlicesOfHeightRange(double from_mm, double to_mm) const = 0;
  };

}