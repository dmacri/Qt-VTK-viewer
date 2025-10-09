#pragma once

#include "ISceneWidgetVisualizer.h"
#include "SceneWidgetVisualizerProxy.h"

class Line;
class SettingParameter;


/** @brief Adapter that wraps SceneWidgetVisualizerTemplate to implement ISceneWidgetVisualizer.
 * 
 * This adapter allows template-based visualizers to be used polymorphically.
 * 
 * @tparam Cell The cell type (e.g., BallCell, SciddicaTCell) */
template<typename Cell>
class SceneWidgetVisualizerAdapter : public ISceneWidgetVisualizer
{
public:
    SceneWidgetVisualizerAdapter(int modelTypeValue, const std::string& modelName)
        : m_modelTypeValue(modelTypeValue)
        , m_modelName(modelName)
    {}

    void initMatrix(int dimX, int dimY) override
    {
        m_impl.initMatrix(dimX, dimY);
    }

    void prepareStage(int nNodeX, int nNodeY) override
    {
        m_impl.modelReader.prepareStage(nNodeX, nNodeY);
    }

    void clearStage() override
    {
        m_impl.modelReader.clearStage();
    }

    void readStepsOffsetsForAllNodesFromFiles(int nNodeX, int nNodeY, const std::string& filename) override
    {
        m_impl.modelReader.readStepsOffsetsForAllNodesFromFiles(nNodeX, nNodeY, filename);
    }

    void readStageStateFromFilesForStep(SettingParameter* sp, Line* lines) override
    {
        m_impl.modelReader.readStageStateFromFilesForStep(m_impl.p, sp, lines);
    }

    void drawWithVTK(int nRows, int nCols, int step, Line* lines,
                     vtkSmartPointer<vtkRenderer> renderer,
                     vtkSmartPointer<vtkActor> gridActor) override
    {
        m_impl.visualiser.drawWithVTK(m_impl.p, nRows, nCols, step, lines, renderer, gridActor);
    }

    void refreshWindowsVTK(int nRows, int nCols, int step, Line* lines,
                          int dimLines, vtkSmartPointer<vtkActor> gridActor) override
    {
        m_impl.visualiser.refreshWindowsVTK(m_impl.p, nRows, nCols, step, lines, dimLines, gridActor);
    }

    Visualizer& getVisualizer() override
    {
        return m_impl.visualiser;
    }

    std::string getModelName() const override
    {
        return m_modelName;
    }

    int getModelTypeValue() const override
    {
        return m_modelTypeValue;
    }

    std::vector<StepIndex> availableSteps() const
    {
        return m_impl.modelReader.availableSteps();
    }

private:
    SceneWidgetVisualizerTemplate<Cell> m_impl;
    const std::string m_modelName;
    const int m_modelTypeValue;
};
