CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Enable local module files
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

IF(NOT IGNORE_INTERFACE_CLIPPER)
  OPTION(USE_LOCAL_CLIPPER "Use clipper library provided in the repository" ON)
ENDIF()

ADD_SUBDIRECTORY(icesl-interface)
ADD_SUBDIRECTORY(lib)

IF(ICESL_USE_PLUGINS)
  ADD_SUBDIRECTORY(sample-plugins)
ENDIF()