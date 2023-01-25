#include "scenewidget.h"

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

using namespace std;


#include <sstream>      // std::wostringstream

#include "Parameter.h"
#include "Visualizer.cpp"



int pixelsQuadrato;
int *maxStepVisited;
unordered_map<int, long int> *hashMap;
Line *lines;
Parameter **p;
SettingParameter* settingParameter =new SettingParameter();
Visualizer<Parameter> *vis= new Visualizer<Parameter>();
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
SettingRenderParameter* settingRenderParameter;

vtkSmartPointer<vtkRenderWindow>renderWindow_;
vtkSmartPointer<vtkRenderWindowInteractor> interactor_;


SceneWidget::SceneWidget(QWidget* parent, int argc, char *argv[])
    : QVTKOpenGLNativeWidget(parent)
{

}


void SceneWidget::addDataSet(vtkSmartPointer<vtkDataSet> dataSet)
{


}
void SceneWidget::addVisualizer(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << " no pixel size " << endl;
        return;
    }
    // std::string str = "C:\Progetti-VTK\QtVktVisualizer\Configuration.txt";
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

    vis->readConfigurationFile(filename, infoFromFile, settingParameter->outputFileName);

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

    vtkNew<vtkFileOutputWindow> fileOutputWindow;
    fileOutputWindow->SetFileName("output.txt");
    // Note that the SetInstance function is a static member of vtkOutputWindow.
    vtkOutputWindow::SetInstance(fileOutputWindow);
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

    p = new Parameter *[settingParameter->dimY];
    for (int i = 0; i < settingParameter->dimY; i++)
    {
        p[i] = new Parameter[settingParameter->dimX];
    }
    cout << settingParameter->dimX << " " <<settingParameter-> dimY << endl;

    //settingRenderParameter= vis->initVTKDimension(settingParameter->dimX * pixelsQuadrato, (settingParameter->dimY + 10) * pixelsQuadrato);


    SettingRenderParameter* settingRenderParameter = new SettingRenderParameter();
    renderWindow()->AddRenderer(settingRenderParameter->m_renderer);

    interactor()->SetRenderWindow(renderWindow());
    settingRenderParameter->m_renderer->SetBackground( settingRenderParameter->colors->GetColor3d("Silver").GetData());
    renderWindow()->SetSize(settingParameter->dimX * pixelsQuadrato, (settingParameter->dimY + 10) * pixelsQuadrato);
    // An interactor
    /* With this style you can block only rotation, but not blocking zoom.You can use NULL value in SetInteractorStyle if you want block evreth   */
    vtkNew<vtkInteractorStyleImage> style;
    interactor()->SetInteractorStyle(style);
    renderWindow()->SetWindowName("QtVtkVisualizer");



    vis->loadHashmapFromFile(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);

    vtkNew<vtkCallbackCommand> keypressCallback;
    keypressCallback->SetCallback(KeypressCallbackFunction);
    keypressCallback->SetClientData(settingParameter);
    interactor()->AddObserver(vtkCommand::KeyPressEvent,keypressCallback);

    lines = new Line[settingParameter->numberOfLines];

    vis->getElementMatrix(settingParameter->step, p,settingParameter-> dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, lines);

    vis->drawWithVTK(p,settingParameter-> dimY+1, settingParameter->dimX+1, settingParameter->step, lines, settingParameter->numberOfLines, settingParameter->edittext,settingRenderParameter->m_renderer,gridActor);

    vis->buildLoadBalanceLine(lines,settingParameter->numberOfLines,settingParameter->dimY,settingParameter->dimX,pts,cellLines,grid,colors,settingRenderParameter->m_renderer,actorBuildLine);

    buildStepActor= vis->buildStepText(settingParameter->step,settingParameter->font_size,colors,singleLineTextPropStep,singleLineTextStep,settingRenderParameter->m_renderer);

    renderWindow_=renderWindow();
    interactor_=interactor();
    //  settingParameter->step++;
    delete[] lines;

    // Render
    // settingRenderParameter->renderWindow= RenderWindow();
    renderWindow()->Render();
    interactor()->Start();



}

void SceneWidget::removeDataSet()
{
    vtkActor* actor = settingRenderParameter->m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
        settingRenderParameter->m_renderer->RemoveActor(actor);
    }

    renderWindow()->Render();
}

void SceneWidget::zoomToExtent()
{
    // Zoom to extent of last added actor
    vtkSmartPointer<vtkActor> actor = settingRenderParameter->m_renderer->GetActors()->GetLastActor();
    if (actor != nullptr) {
        settingRenderParameter->m_renderer->ResetCamera(actor->GetBounds());
    }

    renderWindow()->Render();
}

void SceneWidget::increaseCountUp()
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
    if (parameterInsertedInTextEdit.compare("s")==0)
    {
        settingParameter->step = 98;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("d")==0)
    {
        settingParameter->step = 99;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("f")==0)
    {
        settingParameter->step  = 198;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("g")==0)
    {
        settingParameter->step  = 199;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("h")==0)
    {
        settingParameter->step = 298;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("j")==0)
    {
        settingParameter->step = 299;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("k")==0)
    {
        settingParameter->step  = 398;
        settingParameter->changed = true;
    }
    if (parameterInsertedInTextEdit.compare("l")==0)
    {
        settingParameter->step  = 400;
        settingParameter->changed = true;
    }

     SceneWidget::upgradeModelInCentralPanel();
}




void SceneWidget::upgradeModelInCentralPanel(){
    if (settingParameter->changed==true || settingParameter->firstTime==true )
    {
        lines = new Line[settingParameter->numberOfLines];
        //  std::cout << "Sono al passo prima getElementMatrix: " << cam->step << std::endl;
        try
        {
            vis->getElementMatrix(settingParameter->step, p, settingParameter->dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, lines);
            // std::cout << "Sono al passo dopo getElementMatrix: " << cam->step << std::endl;
            vis->refreshWindowsVTK(p, settingParameter->dimY+1, settingParameter->dimX+1, settingParameter->step, lines, settingParameter->numberOfLines,gridActor);
            vis->refreshBuildLoadBalanceLine(lines,settingParameter->numberOfLines,settingParameter->dimY+1,settingParameter->dimX+1,actorBuildLine,colors);
            // vis->refreshBuildStepText(settingParameter->step,buildStepActor);
            vis->buildStepLine(settingParameter->step,singleLineTextStep,singleLineTextPropStep,colors,"Red");

        }catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
        }
        catch(const std::exception& ex)
        {
            // speciffic handling for all exceptions extending std::exception, except
            // std::runtime_error which is handled explicitly
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }
        renderWindow_->Render();
        settingParameter->firstTime = false;
        settingParameter->changed = false;
        //  cout << cam-> step << endl;
        delete[] lines;

    }

}

    namespace {

    void KeypressCallbackFunction(vtkObject* caller,
                                  long unsigned int vtkNotUsed(eventId),
                                  void* clientData,
                                  void* callData)
    {
        //std::cout << "Keypress callback" << std::endl;
        vtkRenderWindowInteractor* key =
                static_cast<vtkRenderWindowInteractor*>(caller);

        //  std::cout << "Pressed: " << key->GetKeySym() << std::endl;
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
            for (int i = 0; i < settingParameter->nNodeY; i++)
            {
                delete p[i];
            }
            delete[] p;
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
                vis->getElementMatrix(cam->step, p, cam->dimX, cam->dimY, cam->nNodeX, cam->nNodeY, cam->outputFileName, lines);
                // std::cout << "Sono al passo dopo getElementMatrix: " << cam->step << std::endl;
                vis->refreshWindowsVTK(p, cam->dimY+1, cam->dimX+1, cam->step, lines, cam->numberOfLines,gridActor);
                vis->refreshBuildLoadBalanceLine(lines,cam->numberOfLines,cam->dimY+1,cam->dimX+1,actorBuildLine,colors);
                // vis->refreshBuildStepText(settingParameter->step,buildStepActor);
                vis->buildStepLine(settingParameter->step,singleLineTextStep,singleLineTextPropStep,colors,"Red");

            }catch(const std::runtime_error& re)
            {
                std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
            }
            catch(const std::exception& ex)
            {
                // speciffic handling for all exceptions extending std::exception, except
                // std::runtime_error which is handled explicitly
                std::cerr << "Error occurred: " << ex.what() << std::endl;
            }
            renderWindow_->Render();
            cam->firstTime = false;
            cam->changed = false;
            //  cout << cam-> step << endl;
            delete[] lines;
        }
    }
    }


