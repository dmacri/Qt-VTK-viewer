cmake_minimum_required(VERSION 3.16)
project(CustomModelPlugin VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Configuration - Adjust these paths for your system
# ============================================================================

# Path to the Qt-VTK-viewer source directory
set(QTVTKVIEWER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "Path to Qt-VTK-viewer source")

# Path to OOpenCAL (if using models from OOpenCAL)
set(OOPENCAL_DIR "${QTVTKVIEWER_DIR}/.." CACHE PATH "Path to OOpenCAL directory")

# ============================================================================
# Find required packages
# ============================================================================

# Only find packages if not already found by parent project
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

# Qt is optional for plugins (only needed if plugin uses Qt types in headers)
# If building as subdirectory, Qt should already be found by main project
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
    # Do NOT compile Factory - use symbols from main app!
    README.md
)

# Link with VTK (minimal linking - main app has most dependencies)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${VTK_LIBRARIES}
)

# Ensure symbols are exported
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET default
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PREFIX "lib"  # Ensure it's named libCustomModelPlugin.so
)

# ============================================================================
# Installation (optional)
# ============================================================================

# Install to plugins directory relative to build directory
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
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt-VTK-viewer source: ${QTVTKVIEWER_DIR}")
message(STATUS "OOpenCAL directory:   ${OOPENCAL_DIR}")
message(STATUS "VTK version:          ${VTK_VERSION}")

# Check if building as subdirectory or standalone
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
