// SL 2020-09-07
#pragma once

#include <LibSL/LibSL.h>

/// \brief Interface to a path
class IPath
{
public:

  /// \brief creates an empty path
  IPath() = default;

  /// \brief destructor
  virtual ~IPath() = default;

  /// \brief returns true if the path is cyclic
  virtual bool isCyclic() const = 0;

  /// \brief ceates a path from vertices, where cyclic indicates if the path is open or close
  /// must be called before any per-vertex operation is performed
  virtual void createPath(const std::vector<v2f> &vertices, bool cyclic) = 0;

  /// \brief adds a path attribute, returns the new attribute index
  virtual int  addPathAttribute(const std::string &) = 0;

  /// \brief adds a per-vertex attribute, returns the new attribute index
  virtual int  addPerVertexAttribute(const std::string &) = 0;

  /// \brief returns the number of vertices in the path
  virtual int  getNumVertices() = 0;

  /// \brief returns the position of the i-th vertex in slice space
  virtual v2f  getVertexPos_mm(int) = 0;

  /// \brief returns the path attributes names
  virtual void getPathAttributes(std::vecotr<std::string> &) = 0;

  /// \brief returns the index of the path attributes, -1 if unknown
  virtual int  getPathAttributeIndex(const std::string &) = 0;

  /// \brief gets the value of the path attribute
  virtual void getPathAttributeValue(int, double &) = 0;

  /// \brief sets the value of the path attribute
  virtual void setPathAttributeValue(int, double) = 0;

  /// \brief returns the per-vertex attributes names
  virtual void getPerVertexAttributes(std::vecotr<std::string> &) = 0;

  /// \brief returns the index of the per-vertex attribute, -1 if unknown
  virtual int  getPerVertexAttributeIndex(const std::string &) = 0;

  /// \brief returns the value of the per-vertex attribute for the i-th vertex
  virtual void getPerVertexAttributeValue(int, double &) = 0;

  /// \brief sets the value of the per-vertex attribute for the i-th vertex
  virtual void setPerVertexAttributeValue(int, double) = 0;

};
