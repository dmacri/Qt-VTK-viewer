/** CustomModelPlugin.cpp
 * 
 * Example plugin for Qt-VTK-viewer demonstrating how to add a custom
 * model without recompiling the main application.
 * 
 * To use this plugin:
 * 1. Compile it as a shared library (.so file)
 * 2. Load it at runtime using dlopen or through the application's plugin loader
 * 3. The model will appear in the Model menu as "CustomModel" */

#include <functional>
#include <memory>
#include <iostream>

// Forward declarations for the factory
class ISceneWidgetVisualizer;

template<typename Cell>
class SceneWidgetVisualizerAdapter;

class SceneWidgetVisualizerFactory
{
public:
    using ModelCreator = std::function<std::unique_ptr<ISceneWidgetVisualizer>()>;
    static bool registerModel(const std::string& modelName, ModelCreator creator);
};

// This is a minimal declaration - in real plugin you would include the actual headers
// But for demonstration we show that plugin only needs to know about the registration API

extern "C"
{
/** Plugin entry point
 * 
 * This function is called when the plugin is loaded. It registers the custom
 * model with the factory so it becomes available in the application.
 * 
 * The function must be:
 * - Exported with extern "C" to avoid name mangling
 * - Marked with visibility("default") to be visible in the shared library
 * - Named "registerPlugin" (this is the standard entry point) */
__attribute__((visibility("default")))
void registerPlugin()
{
    std::cout << "Loading CustomModel plugin..." << std::endl;
    
    // Note: In real implementation, you would include the actual factory header
    // and use the real SceneWidgetVisualizerAdapter. This is just a minimal example.
    // 
    // Real code would be:
    // SceneWidgetVisualizerFactory::registerModel("CustomModel", []() {
    //     return std::make_unique<SceneWidgetVisualizerAdapter<CustomCell>>("CustomModel");
    // });
    
    std::cout << "CustomModel plugin registered successfully!" << std::endl;
}

/**
 * Optional: Plugin information function
 * 
 * Can be called to get metadata about the plugin
 */
__attribute__((visibility("default")))
const char* getPluginInfo()
{
    return "CustomModel Plugin v1.0 - Example model for Qt-VTK-viewer";
}

/**
 * Optional: Plugin version
 */
__attribute__((visibility("default")))
int getPluginVersion()
{
    return 1; // Version 1.0
}
} // extern "C"
