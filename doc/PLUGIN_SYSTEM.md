# Plugin System for Models

## Introduction

Starting from the refactoring version, the QtVtkViewer application supports **dynamic model loading** without recompiling the whole application. The system is based on:

* **String-based model registry** instead of a fixed enum
* **Dynamic libraries (.so)** used as plugins
* **Registration functions** for automatic model injection

## Architecture

### Model Factory

`SceneWidgetVisualizerFactory` uses a model registry:

```cpp
static bool registerModel(const std::string& modelName, ModelCreator creator);
static std::unique_ptr<ISceneWidgetVisualizer> create(const std::string& modelName);
static std::vector<std::string> getAvailableModels();
static bool isModelRegistered(const std::string& modelName);
```

Built-in models (`Ball`, `SciddicaT`) are registered automatically at application startup.

### How Registration Works

1. Plugin exports the `registerPlugin()` function
2. The application loads the dynamic library (`dlopen`)
3. It calls `registerPlugin()`, which registers the model inside the factory
4. The model becomes available in the menu and can be used immediately

---

## Creating Your Own Plugin

### Step 1: Create a Model Derived from `Element`

Your model must inherit from the `Element` class:

```cpp
// MyCustomCell.h
#ifndef MYCUSTOMCELL_H
#define MYCUSTOMCELL_H

#include <cstdio>
#include <cstdlib>
#include "Element.h"

class MyCustomCell : public Element
{
private:
    int value;
    rgb outputColor{128, 128, 128};

public:
    MyCustomCell() : value(0) {}
    
    MyCustomCell(int val) : value(val) {}

    void setValue(int v) { value = v; }
    int getValue() const { return value; }

    void composeElement(char* str) override
    {
        this->value = atoi(str);
    }

    char* stringEncoding() override
    {
        char* zstr = new char[16];
        sprintf(zstr, "%d", value);
        return zstr;
    }

    rgb* outputValue(char* str) override
    {
        // Example color mapping based on value
        if (value < 50)
            outputColor = rgb(0, 255, 0);      // Green
        else if (value < 100)
            outputColor = rgb(255, 255, 0);    // Yellow
        else
            outputColor = rgb(255, 0, 0);      // Red

        return &outputColor;
    }

    void startStep(int /*step*/) override {}
};

#endif
```

### Step 2: Create the Plugin File with the Registration Function

```cpp
// MyCustomModelPlugin.cpp
#include "MyCustomCell.h"
#include "visualiserProxy/SceneWidgetVisualizerFactory.h"
#include "visualiserProxy/SceneWidgetVisualizerAdapter.h"

extern "C" {

// Function exported by the plugin - must be extern "C"
__attribute__((visibility("default")))
void registerPlugin()
{
    SceneWidgetVisualizerFactory::registerModel("MyCustom", []() {
        return std::make_unique<SceneWidgetVisualizerAdapter<MyCustomCell>>(
            2, // ID (deprecated but required by constructor)
            "MyCustom" // Model name
        );
    });
}

} // extern "C"
```

### Step 3: Compile the Plugin as a Shared Library

Create `CMakeLists.txt` for the plugin:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyCustomModelPlugin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to QtVtkViewer (adjust to your system)
set(QTVTKVIEWER_DIR "/home/agh/Desktop/ItaliaSoftware/Qt-VTK-viewer2")

# Find VTK
find_package(VTK REQUIRED)

# Include directories
include_directories(
    ${QTVTKVIEWER_DIR}
    ${QTVTKVIEWER_DIR}/visualiserProxy
    ${VTK_INCLUDE_DIRS}
    /home/agh/Desktop/ItaliaSoftware  # For OOpenCAL
)

# Add shared library
add_library(MyCustomModelPlugin SHARED
    MyCustomModelPlugin.cpp
)

# Link (only link with VTK, not the main app)
target_link_libraries(MyCustomModelPlugin
    ${VTK_LIBRARIES}
)

# Set symbol visibility
set_target_properties(MyCustomModelPlugin PROPERTIES
    CXX_VISIBILITY_PRESET default
)
```

Compile:

```bash
mkdir build
cd build
cmake ..
make
```

You will get `libMyCustomModelPlugin.so`.

---

## Loading the Plugin in the Application

### Option 1: Automatic Load on Startup

Add plugin loading code in `main.cpp`:

```cpp
#include <dlfcn.h>
#include <iostream>
#include <filesystem>

void loadPlugins(const std::string& pluginDir)
{
    namespace fs = std::filesystem;
    
    if (!fs::exists(pluginDir))
    {
        std::cerr << "Plugin directory does not exist: " << pluginDir << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(pluginDir))
    {
        if (entry.path().extension() == ".so")
        {
            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (!handle)
            {
                std::cerr << "Failed to load plugin: " << entry.path() 
                          << "\nError: " << dlerror() << std::endl;
                continue;
            }

            // Find registerPlugin function
            typedef void (*RegisterFunc)();
            RegisterFunc registerPlugin = (RegisterFunc)dlsym(handle, "registerPlugin");
            
            if (!registerPlugin)
            {
                std::cerr << "Plugin " << entry.path() 
                          << " does not export registerPlugin()" << std::endl;
                dlclose(handle);
                continue;
            }

            // Register plugin
            registerPlugin();
            std::cout << "Loaded plugin: " << entry.path() << std::endl;
            
            // Note: do not dlclose(handle), plugin must stay loaded
        }
    }
}
```
