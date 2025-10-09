cmake_minimum_required(VERSION 3.10)

project(QtVtkViewer)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ============================================
# Sources and resources
# ============================================
file(GLOB_RECURSE Headers *.h *.hpp)
file(GLOB Sources *.cpp)
list(APPEND Sources
    config/Config.cpp
    config/ConfigCategory.cpp
    visualiser/SettingParameter.cpp
    visualiser/VideoExporter.cpp
    visualiser/Visualiser.cpp
    visualiserProxy/SceneWidgetVisualizerFactory.cpp
    widgets/ClickableLabel.cpp
    widgets/ConfigDetailsDialog.cpp
    widgets/SceneWidget.cpp
)
file(GLOB Resources *.qrc)
file(GLOB Styles *.qss)
file(GLOB UIs *.ui)
file(GLOB_RECURSE Docs *.md)


# ============================================
# Qt setup
# ============================================
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")


# ============================================
# VTK dependencies
# ============================================
find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    FiltersSources
    InteractionStyle
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


# ============================================
# Executable target
# ============================================
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${Sources} ${Headers} ${Resources} ${UIs} ${Styles} ${Docs} LICENSE
)


# ============================================
# OOpenCAL include directory (optional)
# ============================================
set(OOPENCAL_DIR "/home/dmacri80/Progetto-Visualizer/" CACHE PATH "Path to the base directory of OOpenCAL sources")

if(OOPENCAL_DIR)
    message(STATUS "Using OOpenCAL directory: ${OOPENCAL_DIR}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${OOPENCAL_DIR})
else()
    message(WARNING "OOPENCAL_DIR not set! Includes like <OOpenCAL/...> may not work.")
endif()


# ============================================
# inih library (manual integration)
# ============================================
# NOTE:
# The inih library does not provide a CMake build system.
# It only contains sources and a Meson build definition.
# Therefore we must manually add the relevant .c/.cpp files
# and include directories here.
#
# Sources:
#   - ini.c / ini.h   (C parser)
#   - cpp/INIReader.* (C++ wrapper)
#
# We fetch the repository using FetchContent and then
# register its sources into our target manually.
include(FetchContent)

FetchContent_Declare(
    inih
    GIT_REPOSITORY https://github.com/benhoyt/inih.git
    GIT_TAG        master
)

FetchContent_GetProperties(inih)
if(NOT inih_POPULATED)
    FetchContent_Populate(inih)

    target_sources(${PROJECT_NAME} PRIVATE
        ${inih_SOURCE_DIR}/ini.c
        ${inih_SOURCE_DIR}/cpp/INIReader.cpp
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${inih_SOURCE_DIR}
        ${inih_SOURCE_DIR}/cpp
    )
endif()


# ============================================
# Linking
# ============================================
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::OpenGL VTK::GUISupportQt VTK::IOLegacy)

vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES VTK::GUISupportQt VTK::IOLegacy
)
