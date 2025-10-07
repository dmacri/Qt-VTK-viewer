#include <stdexcept> // std::invalid_argument
#include "ISceneWidgetVisualizer.h"
#include "SceneWidgetVisualizerFactory.h"
#include "SceneWidgetVisualizerAdapter.h"
#include <OOpenCAL/models/Ball/BallCell.h>
#include <OOpenCAL/models/SciddicaT/SciddicaTCell.h>


std::unique_ptr<ISceneWidgetVisualizer> SceneWidgetVisualizerFactory::create(ModelType type)
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

std::unique_ptr<ISceneWidgetVisualizer> SceneWidgetVisualizerFactory::createFromName(const string &modelName)
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

std::vector<string> SceneWidgetVisualizerFactory::getAvailableModels()
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
