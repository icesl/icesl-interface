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

#include <Path.h>

using namespace IceSLInterface;

// --------------------------------------------------------

Path::Path(IPath& iPath)
{
  m_IsCyclic = iPath.isCyclic();
  m_TypeBits = iPath.getPathType();
  m_Path.resize(iPath.getNumVertices());
  ForIndex(i, iPath.getNumVertices()) {
    m_Path[i] = iPath.getVertexPos(i);
  }
  // Path attributes
  std::vector<std::string> attribs;
  iPath.getPathAttributes(attribs);
  for (const std::string& attrib : attribs) {
    m_Attribs[attrib] = static_cast<int>(m_AttribValues.size());
    m_AttribValues.push_back(0);
    iPath.getPathAttributeValue(iPath.getPerVertexAttributeIndex(attrib), m_AttribValues.back());
  }
  // PerVertex attributes
  std::vector<std::string> pervertexAttribs;
  iPath.getPerVertexAttributes(pervertexAttribs);
  for (const std::string& attrib : pervertexAttribs) {
    m_PerVertexAttribs[attrib] = static_cast<int>(m_PerVertexAttribValues.size());
    m_PerVertexAttribValues.push_back(std::vector<double>());
    int index = iPath.getPerVertexAttributeIndex(attrib);
    ForIndex(i, iPath.getNumVertices()) {
      m_PerVertexAttribValues.back().push_back(0);
      iPath.getPerVertexAttributeValue(index, i, m_PerVertexAttribValues.back().back());
    }
  }
}

// --------------------------------------------------------

bool Path::isCyclic() const
{
  return m_IsCyclic;
}

// --------------------------------------------------------

int Path::getTool() const
{
  return -1;
}

// --------------------------------------------------------

uint Path::getPathType() const
{
  return (uint)m_TypeBits;
}

// --------------------------------------------------------

void Path::setPathType(uint bits)
{
  m_TypeBits = bits;
}

// --------------------------------------------------------

void Path::createPath(const ClipperLib::Path& path, bool cyclic)
{
  sl_assert(path.size() > 1);
  m_IsCyclic = cyclic;
  m_Path     = path;
  if (cyclic) {
    m_Path.push_back(m_Path.front());
  }
}

// --------------------------------------------------------

int  Path::addPathAttribute(const std::string& name)
{
  int idx = (int)m_AttribValues.size();
  m_AttribValues.push_back(0.0);
  m_Attribs.insert(std::make_pair(name, idx));
  return idx;
}

// --------------------------------------------------------

int  Path::addPerVertexAttribute(const std::string& name)
{
  sl_assert(!m_Path.empty());
  int idx = (int)m_PerVertexAttribValues.size();
  m_PerVertexAttribValues.push_back(std::vector<double>());
  m_PerVertexAttribValues.back().resize(m_Path.size(),0.0);
  m_PerVertexAttribs.insert(std::make_pair(name, idx));
  return idx;
}

// --------------------------------------------------------

int  Path::getNumVertices() const
{
  return (int)m_Path.size();
}

// --------------------------------------------------------

ClipperLib::IntPoint Path::getVertexPos(int idx) const
{
  sl_assert(idx >= 0 && idx < (int)m_Path.size());
  return m_Path[idx];
}

// --------------------------------------------------------

void Path::getPathAttributes(std::vector<std::string> &_attribs) const
{
  _attribs.clear();
  for (auto a : m_Attribs) {
    _attribs.push_back(a.first);
  }
}

// --------------------------------------------------------

int  Path::getPathAttributeIndex(const std::string &name) const
{
  auto A = m_Attribs.find(name);
  if (A == m_Attribs.end()) {
    return -1;
  } else {
    return A->second;
  }
}

// --------------------------------------------------------

void Path::getPathAttributeValue(int idx, double &_v) const
{
  sl_assert(idx >= 0 && idx < (int)m_AttribValues.size());
  _v = m_AttribValues[idx];
}

// --------------------------------------------------------

void Path::setPathAttributeValue(int idx, double v)
{
  sl_assert(idx >= 0 && idx < (int)m_AttribValues.size());
  m_AttribValues[idx] = v;
}

// --------------------------------------------------------

void Path::getPerVertexAttributes(std::vector<std::string> &_attribs) const
{
  _attribs.clear();
  for (auto a : m_PerVertexAttribs) {
    _attribs.push_back(a.first);
  }
}

// --------------------------------------------------------

int  Path::getPerVertexAttributeIndex(const std::string &name) const
{
  auto A = m_PerVertexAttribs.find(name);
  if (A == m_PerVertexAttribs.end()) {
    return -1;
  } else {
    return A->second;
  }
}

// --------------------------------------------------------

void Path::getPerVertexAttributeValue(int attrib_idx, int vertex_idx, double &_v) const
{
  sl_assert(attrib_idx >= 0 && attrib_idx < (int)m_PerVertexAttribValues.size());
  sl_assert(vertex_idx >= 0 && vertex_idx < (int)m_Path.size());
  _v = m_PerVertexAttribValues[attrib_idx][vertex_idx];
}

// --------------------------------------------------------

void Path::setPerVertexAttributeValue(int attrib_idx, int vertex_idx, double v)
{
  sl_assert(attrib_idx >= 0 && attrib_idx < (int)m_PerVertexAttribValues.size());
  sl_assert(vertex_idx >= 0 && vertex_idx < (int)m_Path.size());
  m_PerVertexAttribValues[attrib_idx][vertex_idx] = v;
}

// --------------------------------------------------------
