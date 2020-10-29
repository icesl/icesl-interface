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

#include <IPath.h>

#include <unordered_map>

namespace IceSLInterface
{

  /// \brief Implements a basic path. Used in returning geometry data back to IceSL
  class Path : public IPath
  {
  protected:

    /// \brief is the path cylic?
    bool             m_IsCyclic = false;
    /// \brief path type bitfield
    uint             m_TypeBits = 0;
    /// \brief extruder used in path
    int              m_Tool;
    /// \brief path outline
    ClipperLib::Path m_Path;
    /// \brief path attributes
    std::unordered_map<std::string, int>  m_Attribs;
    /// \brief path attributes values
    std::vector<double>                   m_AttribValues;
    /// \brief per-vertex attributes
    std::unordered_map<std::string, int > m_PerVertexAttribs;
    /// \brief per-vertex attributes values
    std::vector< std::vector<double> >    m_PerVertexAttribValues;

  public:

    Path() = default;

    /// \brief creats a path from the interface iPath
    Path(IPath& iPath);

    /// \brief returns true if the path is cyclic
    bool isCyclic() const override;

    /// \brief return the extruder used for the path
    int getTool() const override;

    /// \brief sets the extruder used for the path
    void setTool(int t) override;

    /// \brief returns the path type bitfield
    uint getPathType() const override;

    /// \brief sets the path type bitfield
    void setPathType(uint) override;

    /// \brief initialize a path, where parameter cyclic indicates if the path is open or closed
    /// must be called before any per-vertex operation is performed
    void createPath(const ClipperLib::Path &path, bool cyclic) override;

    /// \brief adds a path attribute, returns the new attribute index
    int  addPathAttribute(const std::string &) override;

    /// \brief adds a per-vertex attribute, returns the new attribute index
    int  addPerVertexAttribute(const std::string &) override;

    /// \brief returns the number of vertices in the path
    int  getNumVertices() const override;

    /// \brief returns the position of the i-th vertex in integer grid space
    ClipperLib::IntPoint getVertexPos(int) const override;

    /// \brief returns the path attributes' names
    void getPathAttributes(std::vector<std::string> &) const override;

    /// \brief returns the index of the path attribute, -1 if unknown
    int  getPathAttributeIndex(const std::string &) const override;

    /// \brief gets the value of the path attribute
    void getPathAttributeValue(int, double &) const override;

    /// \brief sets the value of the path attribute
    void setPathAttributeValue(int, double) override;

    /// \brief returns the per-vertex attributes names
    void getPerVertexAttributes(std::vector<std::string> &) const override;

    /// \brief returns the index of the per-vertex attribute, -1 if unknown
    int  getPerVertexAttributeIndex(const std::string &) const override;

    /// \brief returns the value of the per-vertex attribute for the i-th vertex
    void getPerVertexAttributeValue(int attrib_idx, int vertex_idx, double &) const override;

    /// \brief sets the value of the per-vertex attribute for the i-th vertex
    void setPerVertexAttributeValue(int attrib_idx, int vertex_idx, double) override;

  };

}
