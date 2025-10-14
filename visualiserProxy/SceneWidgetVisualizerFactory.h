/** @file SceneWidgetVisualizerFactory.h
 * @brief Factory for creating scene widget visualizers.
 * 
 * This file contains the SceneWidgetVisualizerFactory class, which is responsible for
 * creating appropriate visualizer instances based on the model type. The factory
 * enables dynamic model selection at runtime and provides a unified way to create
 * visualizers without exposing the underlying implementation details.
 * 
 * The factory supports all model types defined in the ModelType enum and provides
 * methods to create visualizers either by type or by name. It serves as the main
 * entry point for the visualization system to obtain visualizer instances.
 * 
 * @see ISceneWidgetVisualizer
 * @see SceneWidgetVisualizerAdapter
 * @see doc/CHANGELOG_RUNTIME_MODELS.md for architectural details */

#pragma once

#include <string>
#include <memory> // std::unique_ptr<>
#include <vector>

// Forward declaration
class ISceneWidgetVisualizer;

/** @enum ModelType
 * @brief Enumerates the supported model types for visualization.
 * 
 * This enum defines all model types that can be visualized by the system.
 * Each value corresponds to a specific model implementation with its own
 * visualization requirements and behaviors. */
enum class ModelType: int
{
    Ball,       ///< Ball model for granular material simulation
    SciddicaT   ///< SCIDDICA-T model for debris flows simulation
};

/** @class SceneWidgetVisualizerFactory
 * @brief Factory class for creating visualizer instances based on model type.
 * 
 * This factory is responsible for creating the appropriate visualizer instances
 * for different model types. It provides a simple interface for creating visualizers
 * without requiring knowledge of the specific implementation details of each model.
 * 
 * The factory uses the Adapter pattern internally to wrap template-based visualizers
 * in a polymorphic interface, allowing for runtime model selection and visualization.
 * 
 * @note This class follows the Singleton pattern, though it's not enforced, to ensure
 *       a single point of control for visualizer creation. */
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
