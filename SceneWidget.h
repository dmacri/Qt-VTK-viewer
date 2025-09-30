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
    explicit SceneWidget(QWidget* parent = nullptr, int argc=0, char* argv[]=nullptr);
    ~SceneWidget();

    void addVisualizer(int argc, char* argv[]);

    void upgradeModelInCentralPanel();

    void selectedStepParameter(int stepNumber);

    void setupVtkScene();

    void renderVtkScene();

signals:
    void changedStepNumberWithKeyboardKeys(int stepNumber);

public slots:
    void increaseCountUp();
    void decreaseCountDown();

private slots:
    void showToolTip();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

    void enableToolTipWhenMouseAboveWidget();

private:
    std::unique_ptr<SceneWidgetVisualizerProxy> sceneWidgetVisualizerProxy;
    std::unique_ptr<SettingParameter> settingParameter;
    std::unique_ptr<SettingRenderParameter> settingRenderParameter;

    QTimer m_toolTipTimer;
    QPoint m_lastMousePos;

    std::vector<int> maxStepVisited;
};
