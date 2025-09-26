#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QVTKOpenGLNativeWidget.h>
#include <QTimer>
#include <QMouseEvent>
#include <QToolTip>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include "visualiserProxy/SceneWidgetVisualizerProxyDefault.h"


class SceneWidget;
class SettingParameter;


class  SettingRenderParameter : public QVTKOpenGLNativeWidget{
public:
    vtkNew<vtkNamedColors> colors;
    vtkSmartPointer<vtkRenderer>  m_renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>renderWindow;
};

class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit SceneWidget(QWidget* parent = nullptr, int argc=0, char* argv[]=nullptr);

    SettingParameter* settingParameter;

    SettingRenderParameter* settingRenderParameter;

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

private:
    SceneWidgetVisualizerProxy* sceneWidgetVisualizerProxy;
    QTimer* m_toolTipTimer;
    QPoint m_lastMousePos;
};

#endif // SCENEWIDGET_H
