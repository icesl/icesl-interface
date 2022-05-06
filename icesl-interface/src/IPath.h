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

#include <LibSL/LibSL.h>
#include <clipper.hpp>

namespace IceSLInterface
{

  /// \brief Interface to a path
  /**

   NOTE: to simplify processing, cylic paths must have first point == last point
         this is different from ClipperLib where this last segment is implicit

   IceSL understands the following attributes:
   
   - Path attributes
     - "flow_multiplier"
     - "speed_multiplier"

   - Per-vertex attributes
     - "flow_multiplier"
     - "zoffset" (up/down from standard position)

  */
  class IPath
  {
  public:

    /// \brief creates an empty path
    IPath() = default;

    /// \brief destructor
    virtual ~IPath() = default;

    /// \brief returns true if the path is cyclic
    virtual bool isCyclic() const = 0;

    /// \brief return if the path comes from a plugin
    virtual bool isCustom() const = 0;

    /// \brief return the extruder used for the path
    virtual int getTool() const = 0;

    /// \brief sets the extruder used for the path
    virtual void setTool(int t) = 0;

    /// \brief returns the path type bitfield. The output of this function is enumerated in file 'PathTypeBitField.h'
    virtual uint getPathType() const = 0;

    /// \brief sets the path type bitfield
    virtual void setPathType(uint) = 0;

    /// \brief initialize a path, where cyclic indicates if the path is open or closed
    /// must be called before any per-vertex operation is performed
    virtual void createPath(const ClipperLib::Path &path, bool cyclic) = 0;

    /// \brief adds a path attribute, returns the new attribute index
    virtual int  addPathAttribute(const std::string &) = 0;

    /// \brief adds a per-vertex attribute, returns the new attribute index
    virtual int  addPerVertexAttribute(const std::string &) = 0;

    /// \brief returns the number of vertices in the path
    virtual int  getNumVertices() const = 0;

    /// \brief returns the position of the i-th vertex in integer grid space
    virtual ClipperLib::IntPoint getVertexPos(int) const = 0;

    /// \brief returns the path attributes' names
    virtual void getPathAttributes(std::vector<std::string> &) const = 0;

    /// \brief returns the index of the path attributes, -1 if unknown
    virtual int  getPathAttributeIndex(const std::string &) const = 0;

    /// \brief gets the value of the path attribute
    virtual void getPathAttributeValue(int, double &) const = 0;

    /// \brief sets the value of the path attribute
    virtual void setPathAttributeValue(int, double) = 0;

    /// \brief returns the per-vertex attributes names
    virtual void getPerVertexAttributes(std::vector<std::string> &) const = 0;

    /// \brief returns the index of the per-vertex attribute, -1 if unknown
    virtual int  getPerVertexAttributeIndex(const std::string &) const = 0;

    /// \brief returns the value of the per-vertex attribute for the i-th vertex
    virtual void getPerVertexAttributeValue(int attrib_idx, int vertex_idx, double &) const = 0;

    /// \brief sets the value of the per-vertex attribute for the i-th vertex
    virtual void setPerVertexAttributeValue(int attrib_idx, int vertex_idx, double) = 0;

  };

}
