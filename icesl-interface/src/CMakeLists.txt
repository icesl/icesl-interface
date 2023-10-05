CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(IceSL-Interface)

INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../include
)

ADD_DEFINITIONS(-DASIO_STANDALONE )
ADD_DEFINITIONS(-DUSE_GLUX)
ADD_DEFINITIONS(-DLIBARCHIVE_STATIC)

#LINK_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/lib )

INCLUDE(PluginFPIC)

SET(ICESL_SOURCE
  UniqueID.h
  UniqueID.cpp
  IPlugin.h
  IPlugin.cpp
  IPluginEnvironment.h
  EnumerableSettingsInterface.h
  IPath.h
  PathTypeBitField.h
  Path.h
  Path.cpp
  ILayer.h
  ILayerSet.h
  # Plugins
  IInfillerPlugin.h
  IMachineCodePlugin.h
  IPostProcessingPlugin.h
  )

ADD_LIBRARY(icesl_interface ${ICESL_SOURCE})
SET_PROPERTY(TARGET icesl_interface APPEND PROPERTY
   INTERFACE_INCLUDE_DIRECTORIES
 			${CMAKE_CURRENT_SOURCE_DIR}
 			${CMAKE_CURRENT_SOURCE_DIR}/../include
)

IF(EMSCRIPTEN)
  TARGET_LINK_LIBRARIES(icesl_interface LibSL LibSL_gl)
ELSE(EMSCRIPTEN)
  TARGET_LINK_LIBRARIES(icesl_interface LibSL LibSL_gl4core)
ENDIF()

TARGET_LINK_LIBRARIES(icesl_interface
  clipper
)

IF(NOT EMSCRIPTEN)
  IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    TARGET_LINK_LIBRARIES( icesl_interface
      stdc++fs
    )
  ENDIF()
ENDIF()

SET_TARGET_PROPERTIES(icesl_interface PROPERTIES DEBUG_POSTFIX "-d")
