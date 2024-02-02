#include "scenewidget.h"
#include "qapplication.h"


#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <regex>
#include <climits>

#include <vtkNamedColors.h>
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include<vtkInteractorStyleImage.h>
#include <vtkFileOutputWindow.h>
#include <vtkNew.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkActor2D.h>
#include <chrono>

#include <Visualizer.hpp>
using namespace std;
#include <sstream>


int pixelsQuadrato;
int *maxStepVisited;
unordered_map<int, long int> *hashMap;
Line *lines;

vtkNew<vtkNamedColors> colors;
vtkNew<vtkActor> gridActor;
vtkNew<vtkActor2D> actorBuildLine;
vtkSmartPointer<vtkActor2D> buildStepActor;
//Per la generazione delle linee del load balancing
vtkSmartPointer<vtkPoints> pts=vtkSmartPointer<vtkPoints>::New();
vtkSmartPointer<vtkCellArray> cellLines=vtkSmartPointer<vtkCellArray>::New();
vtkSmartPointer<vtkPolyData> grid=vtkSmartPointer<vtkPolyData>::New();
vtkSmartPointer<vtkTextMapper> singleLineTextStep=vtkSmartPointer<vtkTextMapper>::New();
vtkSmartPointer<vtkTextProperty> singleLineTextPropStep=vtkSmartPointer<vtkTextProperty>::New();


vtkSmartPointer<vtkRenderWindow>renderWindow_;
vtkSmartPointer<vtkRenderWindowInteractor> interactor_;


SceneWidget::SceneWidget(QWidget* parent, int argc, char *argv[])
    : QVTKOpenGLNativeWidget(parent)
{
    sceneWidgetVisualizerProxy=new SceneWidgetVisualizerProxy();
    settingParameter =new SettingParameter();
    settingParameter->sceneWidgetVisualizerProxy=sceneWidgetVisualizerProxy;
    settingRenderParameter = new SettingRenderParameter();

}


void SceneWidget::addVisualizer(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << " no pixel size " << endl;
        return;
    }
    char *filename = argv[1];
    pixelsQuadrato = atoi(argv[2]);
    int infoFromFile[8];
    settingParameter->outputFileName = new char[256];
    string tmpString = filename;
    std::size_t found = tmpString.find_last_of("/\\");
    tmpString = tmpString.substr(0, found);
    tmpString += "/Output/";
    char *firstS = new char[256];
    std::strcpy(firstS, tmpString.c_str());

    sceneWidgetVisualizerProxy->vis->readConfigurationFile(filename, infoFromFile, settingParameter->outputFileName);

    strcat(firstS, settingParameter->outputFileName);
    strcpy(settingParameter->outputFileName, firstS);

    settingParameter->dimX = infoFromFile[0]; //numero colonne
    settingParameter->dimY = infoFromFile[1]; //numero righe
    int borderSizeX = infoFromFile[2];
    int borderSizeY = infoFromFile[3];
    int numBorders = infoFromFile[4];
    settingParameter-> nNodeX = infoFromFile[5];
    settingParameter-> nNodeY = infoFromFile[6];
    settingParameter->nsteps = infoFromFile[7];

    cout << "dimX " <<settingParameter-> dimX << endl;
    cout << "dimY " << settingParameter-> dimY << endl;
    cout << "borderSizeX " << borderSizeX << endl;
    cout << "borderSizeY " << borderSizeY << endl;
    cout << "numBorders " << numBorders << endl;
    cout << "nNodeX " <<settingParameter-> nNodeX << endl;
    cout << "nNodeY " <<settingParameter-> nNodeY << endl;
    cout << "outputFileName " << settingParameter->outputFileName << endl;


    setupVtkScene();

    renderVtkScene();

    // Render
    renderWindow()->Render();
    interactor()->Initialize();
    interactor()->Start();
}

void SceneWidget::setupVtkScene()
{
    //vtkNew<vtkFileOutputWindow> fileOutputWindow;
    //fileOutputWindow->SetFileName("output.txt");
    // Note that the SetInstance function is a static member of vtkOutputWindow.
    //vtkOutputWindow::SetInstance(fileOutputWindow);
    // This causes an error intentionally (file name not specified) - this error
    // will be written to the file output.txt
    //    vtkNew<vtkXMLPolyDataReader> reader;
    //    reader->Update();

    hashMap = new unordered_map<int, long int>[settingParameter->nNodeX * settingParameter->nNodeY];
    maxStepVisited = new int[settingParameter->nNodeX * settingParameter->nNodeY];
    settingParameter->numberOfLines = 2 * (settingParameter->nNodeX * settingParameter->nNodeY);
    settingParameter->step = 1;
    settingParameter->changed = false;
    settingParameter->firstTime = true;
    settingParameter->insertAction = false;

    sceneWidgetVisualizerProxy->p = sceneWidgetVisualizerProxy->getAllocatedParametersMatrix(settingParameter->dimX,settingParameter->dimY);
    settingParameter->sceneWidgetVisualizerProxy->p=sceneWidgetVisualizerProxy->p;
    cout << settingParameter->dimX << " " <<settingParameter-> dimY << endl;


    renderWindow()->AddRenderer(settingRenderParameter->m_renderer);
    interactor()->SetRenderWindow(renderWindow());
    settingRenderParameter->m_renderer->SetBackground( settingRenderParameter->colors->GetColor3d("Silver").GetData());
    renderWindow()->SetSize(settingParameter->dimX * pixelsQuadrato, (settingParameter->dimY + 10) * pixelsQuadrato);
    // An interactor
    /* With this style you can block only rotation, but not blocking zoom.You can use NULL value in SetInteractorStyle if you want block evreth   */
#ifdef ENABLE_FEATURE
    vtkNew<vtkInteractorStyleImage> style;
    interactor()->SetInteractorStyle(style);
#endif
    renderWindow()->SetWindowName("Visualizer");
}

void SceneWidget::renderVtkScene()
{
    sceneWidgetVisualizerProxy->vis->loadHashmapFromFile(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);

    vtkNew<vtkCallbackCommand> keypressCallback;
    keypressCallback->SetCallback(KeypressCallbackFunction);
    keypressCallback->SetClientData(settingParameter);
    interactor()->AddObserver(vtkCommand::KeyPressEvent,keypressCallback);

    lines = new Line[settingParameter->numberOfLines];

    sceneWidgetVisualizerProxy->vis->getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p,settingParameter-> dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, lines);

    sceneWidgetVisualizerProxy->vis->drawWithVTK(sceneWidgetVisualizerProxy->p,settingParameter-> dimY+1, settingParameter->dimX+1, settingParameter->step, lines, settingParameter->numberOfLines, settingParameter->edittext,settingRenderParameter->m_renderer,gridActor);

    sceneWidgetVisualizerProxy->vis->buildLoadBalanceLine(lines,settingParameter->numberOfLines,settingParameter->dimY,settingParameter->dimX,pts,cellLines,grid,colors,settingRenderParameter->m_renderer,actorBuildLine);

    buildStepActor= sceneWidgetVisualizerProxy->vis->buildStepText(settingParameter->step,settingParameter->font_size,colors,singleLineTextPropStep,singleLineTextStep,settingRenderParameter->m_renderer);

    renderWindow_=renderWindow();
    interactor_=interactor();
    delete[] lines;
}


void SceneWidget:: increaseCountUp()
{
    if (settingParameter->step < settingParameter->nsteps * 2){
        settingParameter->step += 1;
        settingParameter->changed = true;
    }
    SceneWidget::upgradeModelInCentralPanel();

}


void SceneWidget::decreaseCountDown()
{
    if (settingParameter->step  > 1){
        settingParameter->step  -= 1;
        settingParameter->changed = true;
    }

    SceneWidget::upgradeModelInCentralPanel();

}

void SceneWidget::selectedStepParameter(string parameterInsertedInTextEdit)
{
    int step = stoi(parameterInsertedInTextEdit);
    settingParameter->step = step;
    settingParameter->changed = true;
    SceneWidget::upgradeModelInCentralPanel();

}




void SceneWidget::upgradeModelInCentralPanel(){
    if (settingParameter->changed==true || settingParameter->firstTime==true )
    {
        lines = new Line[settingParameter->numberOfLines];
        try
        {
            sceneWidgetVisualizerProxy->vis->getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p, settingParameter->dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, lines);

            sceneWidgetVisualizerProxy->vis->refreshWindowsVTK(sceneWidgetVisualizerProxy->p, settingParameter->dimY+1, settingParameter->dimX+1, settingParameter->step, lines, settingParameter->numberOfLines,gridActor);

            sceneWidgetVisualizerProxy->vis->refreshBuildLoadBalanceLine(lines,settingParameter->numberOfLines,settingParameter->dimY+1,settingParameter->dimX+1,actorBuildLine,colors);

            sceneWidgetVisualizerProxy->vis->buildStepLine(settingParameter->step,singleLineTextStep,singleLineTextPropStep,colors,"Red");

        }catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }

        settingParameter->firstTime = false;
        settingParameter->changed = false;
        delete[] lines;
        renderWindow_->Render();
        QApplication::processEvents();
    }

}

namespace {

void KeypressCallbackFunction(vtkObject* caller,
                              long unsigned int vtkNotUsed(eventId),
                              void* clientData,
                              void* callData)
{
    vtkRenderWindowInteractor* key = static_cast<vtkRenderWindowInteractor*>(caller);

    string keyPressed=key->GetKeySym();
    SettingParameter* cam = (SettingParameter*) clientData;
    if (keyPressed.compare("Up")==0)
    {
        if (cam->step < cam->nsteps * 2)
            cam->step += 1;
        cam->changed = true;

    }

    if (keyPressed.compare("Down")==0)
    {
        if (cam->step  > 1)
            cam->step  -= 1;
        cam->changed = true;
    }
    if (keyPressed.compare("i")==0)
    {
        cam->insertAction = true;
    }
    if (keyPressed.compare("s")==0)
    {
        cam->step = 98;
        cam->changed = true;
    }
    if (keyPressed.compare("d")==0)
    {
        cam->step = 99;
        cam->changed = true;
    }
    if (keyPressed.compare("f")==0)
    {
        cam->step  = 198;
        cam->changed = true;
    }
    if (keyPressed.compare("g")==0)
    {
        cam->step  = 199;
        cam->changed = true;
    }
    if (keyPressed.compare("h")==0)
    {
        cam->step = 298;
        cam->changed = true;
    }
    if (keyPressed.compare("j")==0)
    {
        cam->step = 299;
        cam->changed = true;
    }
    if (keyPressed.compare("k")==0)
    {
        cam->step  = 398;
        cam->changed = true;
    }
    if (keyPressed.compare("l")==0)
    {
        cam->step  = 400;
        cam->changed = true;
    }
    if (keyPressed.compare("Escape")==0)
    {
        for (int i = 0; i < cam->nNodeY; i++)
        {
            delete cam->sceneWidgetVisualizerProxy->p[i];
        }
        delete[] cam->sceneWidgetVisualizerProxy->p;
        delete[] hashMap;
        delete[] maxStepVisited;

        auto iren = static_cast<vtkRenderWindowInteractor*>(caller);
        // Close the window
        iren->GetRenderWindow()->Finalize();

        // Stop the interactor
        iren->TerminateApp();
        std::cout << "Closing window..." << std::endl;

    }

    if (cam->changed==true || cam->firstTime==true )
    {
        lines = new Line[cam->numberOfLines];
        //  std::cout << "Sono al passo prima getElementMatrix: " << cam->step << std::endl;
        try
        {
            cam->sceneWidgetVisualizerProxy->vis->getElementMatrix(cam->step, cam->sceneWidgetVisualizerProxy->p, cam->dimX, cam->dimY, cam->nNodeX, cam->nNodeY, cam->outputFileName, lines);

            cam->sceneWidgetVisualizerProxy->vis->refreshWindowsVTK(cam->sceneWidgetVisualizerProxy->p, cam->dimY+1, cam->dimX+1, cam->step, lines, cam->numberOfLines,gridActor);

            cam->sceneWidgetVisualizerProxy->vis->refreshBuildLoadBalanceLine(lines,cam->numberOfLines,cam->dimY+1,cam->dimX+1,actorBuildLine,colors);

            cam->sceneWidgetVisualizerProxy-> vis->buildStepLine(cam->step,singleLineTextStep,singleLineTextPropStep,colors,"Red");

        }catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }
        renderWindow_->Render();
        cam->firstTime = false;
        cam->changed = false;
        delete[] lines;
    }
}
}


