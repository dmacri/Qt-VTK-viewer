#pragma once

#include <QVTKOpenGLNativeWidget.h>
#include <QTimer>
#include <QMouseEvent>
#include <QToolTip>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include "visualiserProxy/SceneWidgetVisualizerProxyDefault.h"


class SettingParameter;
class SettingRenderParameter;


class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit SceneWidget(QWidget* parent);
    ~SceneWidget();

    void addVisualizer(const string &filename, int stepNumber);

    void selectedStepParameter(StepIndex stepNumber);

    const SettingParameter* getSettingParameter() const
    {
        return settingParameter.get();
    }

    static void keypressCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

signals:
    void changedStepNumberWithKeyboardKeys(StepIndex stepNumber);

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
    std::unique_ptr<SceneWidgetVisualizerProxy> sceneWidgetVisualizerProxy;
    std::unique_ptr<SettingParameter> settingParameter;
    std::unique_ptr<SettingRenderParameter> settingRenderParameter;

    QTimer m_toolTipTimer;
    QPoint m_lastMousePos;

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkActor> gridActor;
    vtkNew<vtkActor2D> actorBuildLine;
    /// For the generation of load balancing lines
    vtkNew<vtkTextMapper> singleLineTextStep;
    vtkNew<vtkTextProperty> singleLineTextPropStep;
};
