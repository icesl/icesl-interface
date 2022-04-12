CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(NOT ICESL_PROJECT)
  ADD_SUBDIRECTORY(imgui)
  ADD_SUBDIRECTORY(clipper)
  ADD_LIBRARY(imgui
    imgui/imgui.h
    imgui/imgui.cpp
  )
ENDIF()