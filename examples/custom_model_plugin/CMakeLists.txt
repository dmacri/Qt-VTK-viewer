cmake_minimum_required(VERSION 3.16)
project(CustomModelPlugin VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Plugin configuration options
# ============================================================================

# These options allow you to build different plugin variants from the same source.
# You can override them in CMake GUI, on the command line, or in presets:
#   cmake -DPLUGIN_MODEL_NAME="\"FireModel\"" -DPLUGIN_CELL_CLASS=FireCell ..

set(PLUGIN_MODEL_NAME "\"Custom Model\"" CACHE STRING "Model name visible in the viewer (string literal)")
set(PLUGIN_CELL_CLASS CustomCell CACHE STRING "C++ class implementing the cell model")

# ============================================================================
# Configuration - Adjust these paths for your system
# ============================================================================

set(QTVTKVIEWER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "Path to Qt-VTK-viewer source")
set(OOPENCAL_DIR "${QTVTKVIEWER_DIR}/.." CACHE PATH "Path to OOpenCAL directory")

# ============================================================================
# Find required packages
# ============================================================================

if(NOT VTK_FOUND)
    find_package(VTK COMPONENTS
        CommonColor
        CommonCore
        CommonDataModel
        FiltersSources
        InteractionStyle
        InteractionWidgets
        RenderingAnnotation
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        IOXML
        IOOggTheora
        REQUIRED
            GUISupportQt
            IOLegacy
    )
endif()

if(NOT Qt6_FOUND AND NOT Qt5_FOUND)
    find_package(Qt6 COMPONENTS Core Widgets QUIET)
    if(NOT Qt6_FOUND)
        find_package(Qt5 5.15 COMPONENTS Core Widgets QUIET)
    endif()
endif()

# ============================================================================
# Include directories
# ============================================================================

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QTVTKVIEWER_DIR}
    ${QTVTKVIEWER_DIR}/visualiserProxy
    ${QTVTKVIEWER_DIR}/visualiser
    ${QTVTKVIEWER_DIR}/widgets
    ${OOPENCAL_DIR}
    ${VTK_INCLUDE_DIRS}
)

# ============================================================================
# Build the plugin as a shared library
# ============================================================================

add_library(${PROJECT_NAME} SHARED
    CustomModelPlugin_Full.cpp
    README.md
)

# Pass macro definitions to the compiler
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        MODEL_NAME=${PLUGIN_MODEL_NAME}
        CLASS_NAME=${PLUGIN_CELL_CLASS}
)

# Link with VTK
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${VTK_LIBRARIES}
)

# Ensure symbols are exported
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET default
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PREFIX "lib"
)

# ============================================================================
# Installation (optional)
# ============================================================================

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
)

# ============================================================================
# Build instructions message
# ============================================================================

message(STATUS "========================================")
message(STATUS "${PROJECT_NAME} Configuration")
message(STATUS "========================================")
message(STATUS "Build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt-VTK-viewer dir:   ${QTVTKVIEWER_DIR}")
message(STATUS "OOpenCAL directory:  ${OOPENCAL_DIR}")
message(STATUS "VTK version:         ${VTK_VERSION}")
message(STATUS "----------------------------------------")
message(STATUS "Plugin model name:   ${PLUGIN_MODEL_NAME}")
message(STATUS "Plugin cell class:   ${PLUGIN_CELL_CLASS}")
message(STATUS "----------------------------------------")

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
    message(STATUS "Building as subdirectory of main project")
else()
    message(STATUS "Building standalone")
    message(STATUS "")
    message(STATUS "To build:")
    message(STATUS "  mkdir build && cd build")
    message(STATUS "  cmake ..")
    message(STATUS "  make")
endif()

message(STATUS "")
message(STATUS "Output: libCustomModelPlugin.so")
message(STATUS "========================================")
