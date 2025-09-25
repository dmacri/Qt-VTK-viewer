cmake_minimum_required(VERSION 3.10)

project(QtVtkViewer)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)



# List source files & resources
file(GLOB Sources *.cpp *.hpp)
file(GLOB_RECURSE Headers *.h)
file(GLOB Resources *.qrc)
file(GLOB Styles *.qss)
file(GLOB UIs *.ui)
file(GLOB_RECURSE Docs *.md)


# Use Qt
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

# Find package VTK
find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    FiltersSources
    InteractionStyle
    RenderingContextOpenGL2
    RenderingCore
    GUISupportQt # instead of deprecated vtkGUISupportQt
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    IOXML
    REQUIRED
        GUISupportQt
        IOLegacy
)

# Build executable
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${Sources} ${Headers} ${Resources} ${UIs} ${Styles} ${Docs} LICENSE)


# ============================================
# Option to change OOpenCAL directory
# ============================================
set(OOPENCAL_DIR "/home/dmacri80/Progetto-Visualizer/" CACHE PATH "Path to the base directory of OOpenCAL sources")

if(OOPENCAL_DIR)
    message(STATUS "Using OOpenCAL directory: ${OOPENCAL_DIR}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${OOPENCAL_DIR})
else()
    message(WARNING "OOPENCAL_DIR not set! Includes like <OOpenCAL/...> may not work.")
endif()


# Linking
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::OpenGL VTK::GUISupportQt VTK::IOLegacy)
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES VTK::GUISupportQt VTK::IOLegacy
)
