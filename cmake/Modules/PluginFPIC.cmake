################################################################################
CMake_Minimum_Required(VERSION 2.8.0)
################################################################################

IF(NOT EMSCRIPTEN)
  IF(NOT WIN32)
      ## Linux/GCC: add -fPIC so that LibSL can be linked against shared libraries
      IF(NOT WIN32)
      IF(NOT ANDROID)
      IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
      SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fPIC")
      SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")
      SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
      SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fPIC")
      SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC")
      SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
      ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
      ENDIF(NOT ANDROID)
      ENDIF(NOT WIN32)
  ENDIF()
ENDIF()
