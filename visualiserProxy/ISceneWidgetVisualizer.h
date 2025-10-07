#pragma once

#include <vtkRenderer.h>

class SettingParameter;
class Line;
class Visualizer;


/** @brief Abstract interface for scene widget visualizers.
 * 
 * This interface allows runtime polymorphism for different cell models
 * (e.g., BallCell, SciddicaTCell) without compile-time template binding. */
class ISceneWidgetVisualizer
{
public:
    virtual ~ISceneWidgetVisualizer() = default;

    /// @brief Initialize the matrix with given dimensions.
    virtual void initMatrix(int dimX, int dimY) = 0;

    /// @brief Prepare the stage for reading data.
    virtual void prepareStage(int nNodeX, int nNodeY) = 0;

    /** @brief Clear the stage (remove all loaded step data).
     * 
     * This should be called before reloading data to avoid duplicate entries. */
    virtual void clearStage() = 0;

    /// @brief Read steps offsets for all nodes from files.
    virtual void readStepsOffsetsForAllNodesFromFiles(int nNodeX, int nNodeY, const std::string& filename) = 0;

    /// @brief Read stage state from files for a specific step.
    virtual void readStageStateFromFilesForStep(SettingParameter* sp, Line* lines) = 0;

    /// @brief Draw the visualization using VTK.
    virtual void drawWithVTK(int nRows, int nCols, int step, Line* lines, 
                            vtkSmartPointer<vtkRenderer> renderer, 
                            vtkSmartPointer<vtkActor> gridActor) = 0;

    /// @brief Refresh the VTK windows.
    virtual void refreshWindowsVTK(int nRows, int nCols, int step, Line* lines, 
                                   int dimLines, vtkSmartPointer<vtkActor> gridActor) = 0;

    /// @brief Get the visualizer instance.
    virtual Visualizer& getVisualizer() = 0;

    /** @brief Get the model name (e.g., "Ball", "SciddicaT").
     * 
     * This should return the name from the Cell type's static name() method. */
    virtual std::string getModelName() const = 0;

    /// @brief Get the ModelType enum value for this visualizer.
    virtual int getModelTypeValue() const = 0;
};
