#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include "SceneWidgetVisualizerProxy.h"
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkActor2D.h>

using namespace std;

struct  SettingParameter {
    int step;
    int nsteps;
    bool changed;
    bool firstTime;
    bool insertAction;
    int dimX;
    int dimY;
    int nNodeX;
    int nNodeY;
    char *outputFileName;
    int numberOfLines ;
    int font_size=18;
    string edittext;// an empty string for editting
    SceneWidgetVisualizerProxy* sceneWidgetVisualizerProxy;
    vtkNew<vtkActor> gridActor;
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkActor2D> actorBuildLine;
    vtkSmartPointer<vtkActor2D> buildStepActor;
    vtkSmartPointer<vtkTextMapper> singleLineTextStep=vtkSmartPointer<vtkTextMapper>::New();
    vtkSmartPointer<vtkTextProperty> singleLineTextPropStep=vtkSmartPointer<vtkTextProperty>::New();

    vtkSmartPointer<vtkRenderWindow>renderWindow_;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor_;
    unordered_map<int, long int> *hashMap;
    int *maxStepVisited;
    Line *lines;

};

class  SettingRenderParameter : public QVTKOpenGLNativeWidget{
public:
    vtkNew<vtkNamedColors> colors;
    vtkSmartPointer<vtkRenderer>  m_renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>renderWindow;
};

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget* parent = nullptr,int argc=0, char* argv[]=nullptr);

    SettingParameter* settingParameter;

    SettingRenderParameter* settingRenderParameter;

    void addVisualizer(int argc, char* argv[]);

    void upgradeModelInCentralPanel();

    void selectedStepParameter(string parameterInsertedInTextEdit);

    void setupVtkScene();

    void renderVtkScene();
    int pixelsQuadrato;

    //Per la generazione delle linee del load balancing
    vtkSmartPointer<vtkPoints> pts=vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> cellLines=vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPolyData> grid=vtkSmartPointer<vtkPolyData>::New();



public slots:
    void increaseCountUp();
    void decreaseCountDown();

private:
    SceneWidgetVisualizerProxy* sceneWidgetVisualizerProxy;


};

#endif // SCENEWIDGET_H
