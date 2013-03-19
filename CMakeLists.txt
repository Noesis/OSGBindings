PROJECT(osgNoesisGUI)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7)

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR};${CMAKE_MODULE_PATH}")
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
INCLUDE(CheckIncludeFiles)

IF(WIN32)
    IF(MSVC)
        ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
    ENDIF(MSVC)
ELSE(WIN32)
    SET(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused")
ENDIF(WIN32)

FIND_PACKAGE(OpenGL)

FIND_PATH(OPENSCENEGRAPH_INCLUDE_DIR osg/Referenced
    PATHS
    $ENV{OSG_ROOT}/include
    $ENV{OSG_DIR}/include
    /usr/include 
    /usr/local/include
)

FIND_PATH(OPENSCENEGRAPH_LIB_DIR libosg.so osg.lib
    PATHS
    $ENV{OSG_ROOT}/lib
    $ENV{OSG_DIR}/lib
    /usr/lib 
    /usr/local/lib
)

FIND_PATH(NOESISGUI_DIR Include/Noesis.h)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${OPENSCENEGRAPH_INCLUDE_DIR} ${NOESISGUI_DIR}/Include)
LINK_DIRECTORIES(${OPENSCENEGRAPH_LIB_DIR} ${NOESISGUI_DIR}/Lib)

MACRO(START_EXAMPLE EXE_NAME)
    INCLUDE_DIRECTORIES(${EXTERNAL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    ADD_EXECUTABLE(${EXE_NAME} ${SOURCE_FILES} ${HEADER_FILES})
    SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
    
    TARGET_LINK_LIBRARIES(${EXE_NAME}
        debug osg${CMAKE_DEBUG_POSTFIX}          optimized osg
        debug osgAnimation${CMAKE_DEBUG_POSTFIX} optimized osgAnimation
        debug osgParticle${CMAKE_DEBUG_POSTFIX}  optimized osgParticle
        debug osgShadow${CMAKE_DEBUG_POSTFIX}    optimized osgShadow
        debug osgDB${CMAKE_DEBUG_POSTFIX}        optimized osgDB
        debug osgGA${CMAKE_DEBUG_POSTFIX}        optimized osgGA
        debug osgText${CMAKE_DEBUG_POSTFIX}      optimized osgText
        debug osgUtil${CMAKE_DEBUG_POSTFIX}      optimized osgUtil
        debug osgViewer${CMAKE_DEBUG_POSTFIX}    optimized osgViewer
        debug OpenThreads${CMAKE_DEBUG_POSTFIX}  optimized OpenThreads
        Noesis.lib opengl32.lib ${EXTERNAL_LIBRARIES}
    )
    INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION ${NOESISGUI_DIR}/Bin)
ENDMACRO(START_EXAMPLE)

SET(HEADER_FILES Common/NoesisWrapper.h)
SET(SOURCE_FILES SimpleScene.cpp Common/NoesisWrapper.cpp)
START_EXAMPLE(SimpleScene)

SET(HEADER_FILES Common/NoesisWrapper.h)
SET(SOURCE_FILES InteractiveUI.cpp Common/NoesisWrapper.cpp)
START_EXAMPLE(InteractiveUI)
