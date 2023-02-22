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
 //   vtkSmartPointer<vtkRenderer> renderer=  vtkSmartPointer<vtkRenderer>::New();
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

    void addVisualizer(int argc, char* argv[]);

    void upgradeModelInCentralPanel();

    void selectedStepParameter(string parameterInsertedInTextEdit);

public slots:
    void increaseCountUp();
    void decreaseCountDown();
};

#endif // SCENEWIDGET_H
