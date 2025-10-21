/** CustomModelPlugin_Full.cpp
 * 
 * FULL WORKING VERSION of the plugin with actual includes
 * 
 * This version includes the real headers from Qt-VTK-viewer and will
 * compile into a working plugin that can be loaded at runtime.
 * 
 * Build instructions are in CMakeLists.txt */

#include <iostream>                                        // for basic_ostream
#include <memory>                                          // for __unique_p...
#include <string>                                          // for basic_string
#include "CustomCell.h"                                    // for CustomCell
#include "visualiserProxy/SceneWidgetVisualizerAdapter.h"  // for SceneWidge...
#include "visualiserProxy/SceneWidgetVisualizerFactory.h"  // for SceneWidge...

#define MODEL_NAME "CustomModel"

extern "C"
{
/// Plugin entry point - this gets called when the plugin is loaded
__attribute__((visibility("default")))
void registerPlugin()
{
    std::cout << "Registering " MODEL_NAME " plugin..." << std::endl;
    
    // Register the custom model with the factory
    bool success = SceneWidgetVisualizerFactory::registerModel(MODEL_NAME, []() {
        return std::make_unique<SceneWidgetVisualizerAdapter<CustomCell>>(
            MODEL_NAME  // - this will appear in the menu
        );
    });
    
    if (success)
    {
        std::cout << "✓ " MODEL_NAME " plugin registered successfully!" << std::endl;
        std::cout << "  The model is now available in Model menu" << std::endl;
    }
    else
    {
        std::cerr << "✗ Failed to register " MODEL_NAME " - name may already exist" << std::endl;
    }
}

/// Optional: Get plugin metadata
__attribute__((visibility("default")))
const char* getPluginInfo()
{
    return MODEL_NAME " Plugin v1.0\n"
           "Author: Developers of Qt-VTK-viewer\n"
           "Description: Example custom cell model with color gradient visualization\n"
           "Compatible with: Qt-VTK-viewer 2.x";
}

/// Optional: Get plugin version
__attribute__((visibility("default")))
int getPluginVersion()
{
    return 100; // Version 1.00
}

/// Optional: Get model name provided by this plugin
__attribute__((visibility("default")))
const char* getModelName()
{
    return MODEL_NAME;
}
} // extern "C"
