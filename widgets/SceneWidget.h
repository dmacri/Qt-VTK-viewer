#pragma once

#include <QVTKOpenGLNativeWidget.h>
#include <QTimer>
#include <QMouseEvent>
#include <QToolTip>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkTextMapper.h>
#include "visualiserProxy/ISceneWidgetVisualizer.h"
#include "visualiserProxy/SceneWidgetVisualizerFactory.h"
#include "types.h"

class SettingParameter;
class SettingRenderParameter;


class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT

public:
    explicit SceneWidget(QWidget* parent);
    ~SceneWidget();

    void addVisualizer(const std::string &filename, int stepNumber);

    void selectedStepParameter(StepIndex stepNumber);

    /** @brief Switch to a different model type.
     * 
     * This method allows changing the visualization model at runtime.
     * Note: This does NOT reload data files. Use reloadData() after switching.
     * 
     * @param modelType The new model type to use */
    void switchModel(ModelType modelType);

    /** @brief Reload data files for the current model.
     * 
     * This method reads data files from disk and refreshes the visualization.
     * Call this after switching models or when data files have changed.
     * Note: reloading data with not compatible model can crash the application */
    void reloadData();

    /** @brief Clear the entire scene (remove all VTK actors and data).
     * 
     * This method clears the renderer and resets the visualizer state.
     * Call this before loading a new configuration file. */
    void clearScene();

    /** @brief Load a new configuration file and reinitialize the scene.
     * 
     * @param configFileName Path to the new configuration file
     * @param stepNumber Initial step number to display */
    void loadNewConfiguration(const std::string& configFileName, int stepNumber = 0);

    /** @brief Get the current model name.
     * 
     * @return std::string The name of the currently active model */
    auto getCurrentModelName() const
    {
        return sceneWidgetVisualizerProxy->getModelName();
    }

    const SettingParameter* getSettingParameter() const
    {
        return settingParameter.get();
    }

    static void keypressCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

signals:
    void changedStepNumberWithKeyboardKeys(StepIndex stepNumber);

    void totalNumberOfStepsReadFromConfigFile(StepIndex totalSteps);

    void availableStepsReadFromConfigFile(std::vector<StepIndex> availableSteps);

public slots:
    void increaseCountUp();
    void decreaseCountDown();

private slots:
    void showToolTip();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

    void renderVtkScene();

    void upgradeModelInCentralPanel();
    void updateVisualization();

    void enableToolTipWhenMouseAboveWidget();

    void readSettingsFromConfigFile(const std::string &filename);

    void setupVtkScene();
    void setupSettingParameters(const std::string & configFilename, int stepNumber);

private:
    std::unique_ptr<ISceneWidgetVisualizer> sceneWidgetVisualizerProxy;
    std::unique_ptr<SettingParameter> settingParameter;
    std::unique_ptr<SettingRenderParameter> settingRenderParameter;
    
    ModelType currentModelType;

    QTimer m_toolTipTimer;
    QPoint m_lastMousePos;

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkActor> gridActor;
    vtkNew<vtkActor2D> actorBuildLine;
    /// For the generation of load balancing lines
    vtkNew<vtkTextMapper> singleLineTextStep;
    vtkNew<vtkTextProperty> singleLineTextPropStep;
};
