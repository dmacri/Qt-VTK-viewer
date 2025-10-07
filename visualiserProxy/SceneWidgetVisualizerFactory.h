#pragma once

#include <string>
#include <memory> // std::unique_ptr<>
#include <vector>

class ISceneWidgetVisualizer;

/// @brief Supported model types for visualization.
enum class ModelType: int
{
    Ball,
    SciddicaT
};


/** @brief Factory for creating scene widget visualizers.
 * 
 * This factory creates appropriate visualizer instances based on the model type,
 * enabling runtime model selection. */
class SceneWidgetVisualizerFactory
{
public:
    /** @brief Create a visualizer for the specified model type.
     * 
     * @param type The model type to create
     * @return std::unique_ptr<ISceneWidgetVisualizer> Pointer to the created visualizer
     * @throws std::invalid_argument if the model type is not supported */
    static std::unique_ptr<ISceneWidgetVisualizer> create(ModelType type);

    /** @brief Create a visualizer from a string name.
     * 
     * @param modelName The name of the model (e.g., from Cell::name())
     * @return std::unique_ptr<ISceneWidgetVisualizer> Pointer to the created visualizer
     * @throws std::invalid_argument if the model name is not recognized */
    static std::unique_ptr<ISceneWidgetVisualizer> createFromName(const std::string& modelName);

    /** @brief Get all available model names.
     * 
     * This method creates temporary instances of each model to get their names
     * from the overrides from ISceneWidgetVisualizer::getModelName() method.
     * 
     * @return std::vector<std::string> List of available model names */
    static std::vector<std::string> getAvailableModels();
};
