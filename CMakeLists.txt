cmake_minimum_required(VERSION 3.8)

project(QtVtkViewer)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)



# List source files & resources
file (GLOB Sources *.cpp)
file (GLOB Headers *.h)
file (GLOB Resources *.qrc)
file (GLOB UIs *.ui)

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
        vtkGUISupportQt
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        IOXML
        REQUIRED
            GUISupportQt
            IOLegacy
    )


# Build executable
add_executable(QtVtkViewer MACOSX_BUNDLE ${Sources} ${Headers} ${Resources} ${UIs})

#target_link_libraries(QtVtkViewer PRIVATE Qt5::Widgets Qt5::OpenGL VTK::GUISupportQt VTK::IOLegacy)

target_link_libraries(QtVtkViewer PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
    TARGETS QtVtkViewer
    MODULES VTK::GUISupportQt VTK::IOLegacy)
