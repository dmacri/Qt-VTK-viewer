cmake_minimum_required(VERSION 3.10)

project(QtVtkViewer)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ============================================
# Sources and resources
# ============================================
file(GLOB_RECURSE Headers
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/*.hpp"
)
file(GLOB Sources
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
)
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
    widgets/ColorSettingsDialog.cpp
    widgets/ColorSettings.cpp
    utilities/PluginLoader.cpp
    utilities/ModelReader.cpp
    utilities/CommandLineParser.cpp
)
file(GLOB Resources
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.qrc"
)
file(GLOB Styles
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.qss"
)
file(GLOB UIs
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.ui"
)
file(GLOB_RECURSE Docs
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.md"
)
set(OTHER_FILES LICENSE .github/workflows/release--deb-package--ubuntu24.04.yml .githooks/pre-commit .clang-format)


# ============================================
# Qt setup
# ============================================
set(CMAKE_AUTORCC ON)
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

# ============================================
# Executable target
# ============================================
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${Sources} ${Headers} ${Resources} ${UIs} ${Styles} ${Docs} ${OTHER_FILES}
)


# ============================================
# OOpenCAL include directory (optional)
# ============================================
set(OOPENCAL_DIR "/home/dmacri80/Progetto-Visualizer/" CACHE PATH "Path to the base directory of OOpenCAL sources")

if(OOPENCAL_DIR)
    message(STATUS "Using OOpenCAL directory: ${OOPENCAL_DIR}")
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${OOPENCAL_DIR}
        ${OOPENCAL_DIR}/OOpenCAL
    )
else()
    message(WARNING "OOPENCAL_DIR not set! Includes like <OOpenCAL/...> may not work.")
endif()


# ============================================
# FetchContent setup (offline-friendly)
# ============================================
# The dependencies are fetched once and then reused without attempting updates
# (useful when working without network access).
include(FetchContent)

set(SKIP_UPDATES ON CACHE BOOL "Skip updating FetchContent dependencies after initial download")

if(SKIP_UPDATES)
    set(_UPDATE_ARGS UPDATE_DISCONNECTED ON)
else()
    set(_UPDATE_ARGS)
endif()

# ============================================
# inih library (FetchContent)
# ============================================
FetchContent_Declare(
    inih
    GIT_REPOSITORY https://github.com/benhoyt/inih.git
    GIT_TAG        master
    ${_UPDATE_ARGS}
)

FetchContent_MakeAvailable(inih)

FetchContent_GetProperties(inih)
if(inih_SOURCE_DIR)
    target_sources(${PROJECT_NAME} PRIVATE
        ${inih_SOURCE_DIR}/ini.c
        ${inih_SOURCE_DIR}/cpp/INIReader.cpp
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${inih_SOURCE_DIR}
        ${inih_SOURCE_DIR}/cpp
    )
else()
    message(FATAL_ERROR "inih source directory not found. Ensure initial FetchContent download succeeded.")
endif()

# ============================================
# argparse library (FetchContent)
# ============================================
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        v3.0
    ${_UPDATE_ARGS}
)

FetchContent_MakeAvailable(argparse)

FetchContent_GetProperties(argparse)
if(argparse_SOURCE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${argparse_SOURCE_DIR}/include
    )
else()
    message(FATAL_ERROR "argparse source directory not found. Ensure initial FetchContent download succeeded.")
endif()


# ============================================
# Linking
# ============================================
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::OpenGL VTK::GUISupportQt VTK::IOLegacy)

# Export symbols for plugins to use
# This allows plugins to call functions from the main executable
target_link_options(${PROJECT_NAME} PRIVATE -rdynamic)

vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES VTK::GUISupportQt VTK::IOLegacy
)

# ============================================
# Doxygen Documentation (optional)
# ============================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    find_program(DOT_PROGRAM dot)           # graphviz (dot) - optional

    set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")
    set(DOXYGEN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/docs")
    set(PROJECT_BRIEF "${PROJECT_NAME} documentation generated by Doxygen")

    set(DOXYFILE_DIR "${CMAKE_SOURCE_DIR}/doc")
    set(DOXYFILE_IN "${DOXYFILE_DIR}/Doxyfile.in")
    set(DOXYFILE_OUT "${CMAKE_BINARY_DIR}/Doxyfile")

if(EXISTS "${DOXYFILE_IN}")
        if(DOT_PROGRAM)
            set(HAVE_DOT "YES")
        else()
            set(HAVE_DOT "NO")
        endif()

        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating Doxygen documentation (HTML only)..."
            VERBATIM
        )

        message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
        message(STATUS "Docs will be generated at: ${DOXYGEN_OUTPUT_DIR} (HTML)")
        if(DOT_PROGRAM)
            message(STATUS "Graphviz (dot) found: ${DOT_PROGRAM} (class diagrams enabled)")
        else()
            message(STATUS "Graphviz (dot) not found: class diagrams disabled")
        endif()
    else()
        message(WARNING "Doxyfile.in not found in ${DOXYFILE_DIR}. Skipping docs target.")
    endif()
else()
    message(STATUS "Doxygen not found. Skipping docs target.")
endif()

# ============================================
# Development tools: Sanitizers, IWYU, compile_commands
# ============================================

# Export compile_commands.json for tooling (clang-tidy, cppcheck, IWYU, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable sanitizers on non-Windows with GCC/Clang
if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    include(CheckLibraryExists)

    check_library_exists(asan __asan_init "" HAVE_LIBASAN)
    check_library_exists(ubsan __ubsan_handle_add_overflow "" HAVE_LIBUBSAN)

    if (HAVE_LIBASAN AND HAVE_LIBUBSAN)
        message(STATUS "Sanitizers available: enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
    else()
        message(WARNING "Sanitizers requested but libasan/libubsan not available on this system")
    endif()
endif()

# include-what-you-use (IWYU)
find_program(IWYU_PATH NAMES include-what-you-use iwyu)

if (IWYU_PATH)
    message(STATUS "Found include-what-you-use: ${IWYU_PATH}")
    set(IWYU_MAPPING_FILE "${CMAKE_SOURCE_DIR}/config/iwyu.imp")
    if (EXISTS "${IWYU_MAPPING_FILE}")
        list(APPEND CMAKE_CXX_INCLUDE_WHAT_YOU_USE
            ${IWYU_PATH}
            -Xiwyu
            --mapping_file=${IWYU_MAPPING_FILE}
        )
        message(STATUS "IWYU mapping file: ${IWYU_MAPPING_FILE}")
    else()
        message(WARNING "IWYU mapping file not found at ${IWYU_MAPPING_FILE}; running without mapping")
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
    endif()
else()
    message(WARNING "include-what-you-use not found. Consider installing it for header optimization.")
endif()

# ============================================
# Formatting source code: clang-format
# ============================================

# Collect all relevant source and header files into a single list.
# This makes it easy to maintain and avoids hardcoded directory paths.
set(FORMAT_SOURCES
    ${Sources}
    ${Headers}
)

# Find clang-format executable
find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-14 clang-format-13 clang-format-12)

if(CLANG_FORMAT_EXE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}, adding format targets")

    # Create a command-safe list by quoting paths (handles spaces in filenames).
    foreach(file ${FORMAT_SOURCES})
        if(EXISTS "${file}")
            list(APPEND FORMAT_SOURCES_SAFE "${file}")
        endif()
    endforeach()

    # Only add the target if there are files to format
    if(FORMAT_SOURCES_SAFE)
        add_custom_target(clang-format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${FORMAT_SOURCES_SAFE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-format on project sources"
        )

        add_custom_target(clang-format-check
            COMMAND ${CLANG_FORMAT_EXE} -style=file -output-replacements-xml ${FORMAT_SOURCES_SAFE}
                | grep -q "replacement offset"
                && (echo \"Files need reformatting\" && exit 1) || echo \"Everything is properly formatted\"
            COMMENT "Checking formatting with clang-format (no changes applied)"
        )
    else()
        message(WARNING "No source files found to format.")
    endif()
else()
    message(STATUS "clang-format not found, skipping format targets")
endif()

# ============================================
# Development tools: Static analysis & linting
# ============================================

# --------------------------------------------
# make clang_tidy
# Runs clang-tidy for static code analysis
# --------------------------------------------
find_program(CLANG_TIDY NAMES clang-tidy)
if (CLANG_TIDY)
    message(STATUS "Adding command: clang-tidy (static analysis)")

    add_custom_target(clang_tidy
        DEPENDS ${PROJECT_NAME}
        COMMAND ${CLANG_TIDY}
            -p="${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
            ${Sources} ${Headers}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy to perform static analysis on source files"
        USES_TERMINAL
    )
else()
    message(STATUS "clang-tidy not found: skipping clang_tidy target")
endif()


# --------------------------------------------
# make flawfinder
# Security vulnerability scanner for C/C++ code
# --------------------------------------------
find_program(FLAWFINDER_TOOL NAMES flawfinder)
if (FLAWFINDER_TOOL)
    message(STATUS "Adding command: flawfinder (security analysis)")

    add_custom_target(flawfinder
        COMMAND ${FLAWFINDER_TOOL}
            --quiet --dataonly --nolink --columns --context --singleline --minlevel=0
            ${HEADERS} ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running flawfinder to perform security analysis"
        USES_TERMINAL
    )
else()
    message(STATUS "flawfinder not found: skipping flawfinder target")
endif()


# --------------------------------------------
# make cppcheck_<project>
# Runs cppcheck using compile_commands.json
# --------------------------------------------
find_program(CPPCHECK NAMES cppcheck)
if (CPPCHECK)
    message(STATUS "Adding command: cppcheck")

    # Cppcheck expects include/exclude directories, not lists of files.
    # Exclude build directory and optionally the main source dir.
    set(CPPCHECK_EXCLUDES
        "-i${CMAKE_BINARY_DIR}"
        "-i${CMAKE_SOURCE_DIR}/external"
    )

    add_custom_target(cppcheck
        COMMAND ${CPPCHECK}
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --force
            --enable=warning,performance,unusedFunction,style,information
            --template=gcc
            --inline-suppr
            --library=gnu.cfg --library=std.cfg --library=cppcheck-lib.cfg
            --library=motif.cfg --library=qt.cfg --library=windows.cfg
            --library=posix.cfg --library=avr.cfg
            --suppress=missingIncludeSystem
            ${CPPCHECK_EXCLUDES}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running cppcheck static analysis"
        USES_TERMINAL
    )
else()
    message(STATUS "cppcheck not found: skipping cppcheck target")
endif()

# ============================================
# Subproject:
# ============================================
add_subdirectory(examples/custom_model_plugin)
