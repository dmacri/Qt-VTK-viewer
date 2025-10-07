#pragma once

#include <memory> // std::make_unique()
#include <string>
#include <stdexcept> // std::invalid_argument
#include "ISceneWidgetVisualizer.h"
#include "SceneWidgetVisualizerAdapter.h"
#include "SceneWidgetVisualizerProxyBall.h"
#include "SceneWidgetVisualizerProxySciddicaT.h"

/// @brief Supported model types for visualization.
enum class ModelType
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
    static std::unique_ptr<ISceneWidgetVisualizer> create(ModelType type)
    {
        switch (type)
        {
            case ModelType::Ball:
                return std::make_unique<SceneWidgetVisualizerAdapter<BallCell>>(
                    static_cast<int>(ModelType::Ball), "Ball");
            
            case ModelType::SciddicaT:
                return std::make_unique<SceneWidgetVisualizerAdapter<SciddicaTCell>>(
                    static_cast<int>(ModelType::SciddicaT), "SciddicaT");
            
            default:
                throw std::invalid_argument("Unsupported model type");
        }
    }

    /** @brief Create a visualizer from a string name.
     * 
     * @param modelName The name of the model (e.g., from Cell::name())
     * @return std::unique_ptr<ISceneWidgetVisualizer> Pointer to the created visualizer
     * @throws std::invalid_argument if the model name is not recognized */
    static std::unique_ptr<ISceneWidgetVisualizer> createFromName(const std::string& modelName)
    {
        // Try each model type and check if name matches
        for (int i = 0; i < static_cast<int>(ModelType::SciddicaT) + 1; ++i)
        {
            auto visualizer = create(static_cast<ModelType>(i));
            if (visualizer->getModelName() == modelName)
            {
                return visualizer;
            }
        }
        throw std::invalid_argument("Unknown model name: " + modelName);
    }

    /** @brief Get all available model names.
     * 
     * This method creates temporary instances of each model to get their names
     * from the Cell::name() method.
     * 
     * @return std::vector<std::string> List of available model names */
    static std::vector<std::string> getAvailableModels()
    {
        std::vector<std::string> models;
        
        // Iterate through all model types
        for (int i = 0; i <= static_cast<int>(ModelType::SciddicaT); ++i)
        {
            try
            {
                auto visualizer = create(static_cast<ModelType>(i));
                models.push_back(visualizer->getModelName());
            }
            catch (...)
            {
                // Skip models that fail to create
            }
        }
        
        return models;
    }
};
