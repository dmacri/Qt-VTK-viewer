#include "scenewidget.h"
#include "Config.h"
#include "qapplication.h"


#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>


#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <filesystem>

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

#include <Visualizer.hpp>


// int pixelsQuadrato = 10; // Not needed for VTK, was for old Allegro version
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
vtkSmartPointer<vtkRenderer> globalRenderer_;


SceneWidget::SceneWidget(QWidget* parent, int argc, char *argv[])
    : QVTKOpenGLNativeWidget(parent)
    , m_toolTipTimer(new QTimer(this))
    , m_lastMousePos()
{
    sceneWidgetVisualizerProxy = new SceneWidgetVisualizerProxy;
    settingParameter = new SettingParameter();
    settingParameter->sceneWidgetVisualizerProxy = sceneWidgetVisualizerProxy;
    settingRenderParameter = new SettingRenderParameter();
    
    // Set up tooltip timer
    m_toolTipTimer->setSingleShot(true);
    m_toolTipTimer->setInterval(1000); // 1 second delay before showing tooltip
    connect(m_toolTipTimer, &QTimer::timeout, this, &SceneWidget::showToolTip);
    
    // Enable mouse tracking
    setMouseTracking(true);
}


void SceneWidget::addVisualizer(int argc, char* argv[])
{
    if (argc == 1)
    {
        throw std::invalid_argument("Missing arguments");
    }

    char *filename = argv[1];
    if (! std::filesystem::exists(filename))
    {
        throw std::invalid_argument("File '"s + filename + "' does not exist!");
    }

    Config config(filename);
    config.readConfigFile();

    ConfigCategory* generalContext = config.getConfigCategory("GENERAL");
    ConfigCategory* execContext = config.getConfigCategory("DISTRIBUTED");
    int infoFromFile[8];

    constexpr size_t outputFileNameLength = 256;
    settingParameter->outputFileName = new char[outputFileNameLength]{};
    string tmpFileName = filename;
    std::size_t positionOfLastPathSeparatorIfFound = tmpFileName.find_last_of("/\\");
    tmpFileName = tmpFileName.substr(0, positionOfLastPathSeparatorIfFound) + "/Output/";

    sceneWidgetVisualizerProxy->vis.readConfigurationFile(filename, infoFromFile, settingParameter->outputFileName);
    tmpFileName += generalContext->getConfigParameter("output_file_name")->getValue<const char*>();
    tmpFileName.copy(settingParameter->outputFileName, outputFileNameLength);

    settingParameter->dimX = generalContext->getConfigParameter("number_of_columns")->getValue<int>();
    settingParameter->dimY = generalContext->getConfigParameter("number_of_rows")->getValue<int>();
    int borderSizeX = execContext->getConfigParameter("border_size_x")->getValue<int>();
    int borderSizeY = execContext->getConfigParameter("border_size_y")->getValue<int>();
    int numBorders = 1;
    settingParameter->nNodeX = execContext->getConfigParameter("number_node_x")->getValue<int>();
    settingParameter->nNodeY = execContext->getConfigParameter("number_node_y")->getValue<int>();
    settingParameter->nsteps = generalContext->getConfigParameter("number_steps")->getValue<int>();

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
    interactor()->Enable();
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
    
    // Initialize global renderer reference
    globalRenderer_ = settingRenderParameter->m_renderer;
    renderWindow()->SetSize(settingParameter->dimX , (settingParameter->dimY + 10) );
    // An interactor
    /* With this style you can block only rotation, but not blocking zoom.You can use NULL value in SetInteractorStyle if you want block evreth   */

    vtkNew<vtkInteractorStyleImage> style;
    interactor()->SetInteractorStyle(style);

    // Set the scene widget pointer in the settings
    settingParameter->sceneWidget = this;

    renderWindow()->SetWindowName("Visualizer");
}

void SceneWidget::renderVtkScene()
{
    cout << "DEBUG: Starting renderVtkScene" << endl;
    cout << "DEBUG: Loading hashmap from file..." << endl;
    sceneWidgetVisualizerProxy->vis.loadHashmapFromFile(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);
    cout << "DEBUG: Hashmap loaded successfully" << endl;

    vtkNew<vtkCallbackCommand> keypressCallback;
    keypressCallback->SetCallback(KeypressCallbackFunction);
    keypressCallback->SetClientData(settingParameter);
    interactor()->AddObserver(vtkCommand::KeyPressEvent,keypressCallback);

    std::vector<Line> lines;
    lines.resize(settingParameter->numberOfLines);
    cout << "DEBUG: Allocated lines array" << endl;

    cout << "DEBUG: Calling getElementMatrix..." << endl;
    sceneWidgetVisualizerProxy->vis.getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p,settingParameter-> dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, &lines[0]);
    cout << "DEBUG: getElementMatrix completed" << endl;

    cout << "DEBUG: Calling drawWithVTK..." << endl;
    sceneWidgetVisualizerProxy->vis.drawWithVTK(sceneWidgetVisualizerProxy->p,settingParameter-> dimY, settingParameter->dimX, settingParameter->step, &lines[0], settingParameter->numberOfLines, settingParameter->edittext,settingRenderParameter->m_renderer,gridActor);
    cout << "DEBUG: drawWithVTK completed" << endl;
    
    cout << "DEBUG: Calling buildLoadBalanceLine..." << endl;
    sceneWidgetVisualizerProxy->vis.buildLoadBalanceLine(&lines[0], settingParameter->numberOfLines,settingParameter->dimY+1,settingParameter->dimX+1,pts,cellLines,grid,colors,settingRenderParameter->m_renderer,actorBuildLine);
    cout << "DEBUG: buildLoadBalanceLine completed" << endl;

    buildStepActor= sceneWidgetVisualizerProxy->vis.buildStepText(settingParameter->step,settingParameter->font_size,colors,singleLineTextPropStep,singleLineTextStep,settingRenderParameter->m_renderer);

    renderWindow_=renderWindow();
    interactor_=interactor();
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
        settingParameter->firstTime = true;
    }

    renderVtkScene();
    SceneWidget::upgradeModelInCentralPanel();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* event)
{
    QVTKOpenGLNativeWidget::mouseMoveEvent(event);
    
    // Update the last mouse position
    m_lastMousePos = event->pos();
    
    // Restart the tooltip timer
    m_toolTipTimer->stop();
    m_toolTipTimer->start();
    // TODO: GB: Ask what should we display, what position?
    // Optionally, you can show the coordinates immediately in the status bar or elsewhere
    // statusBar()->showMessage(QString("X: %1, Y: %2").arg(event->x()).arg(event->y()));
}

void SceneWidget::leaveEvent(QEvent* event)
{
    QVTKOpenGLNativeWidget::leaveEvent(event);
    m_toolTipTimer->stop();
    QToolTip::hideText();
}

void SceneWidget::showToolTip()
{
    // Show tooltip at the current mouse position
    QPoint globalPos = mapToGlobal(m_lastMousePos);
    QToolTip::showText(globalPos, 
                      QString("X: %1, Y: %2").arg(m_lastMousePos.x()).arg(m_lastMousePos.y()),
                      this, 
                      QRect(m_lastMousePos, QSize(1, 1)), 
                      2000); // Show for 2 seconds
}

void SceneWidget::selectedStepParameter(int stepNumber)
{
    settingParameter->step = stepNumber;
    settingParameter->changed = true;
    SceneWidget::upgradeModelInCentralPanel();
}


void SceneWidget::upgradeModelInCentralPanel(){
    if (settingParameter->changed==true || settingParameter->firstTime==true )
    {
        lines = new Line[settingParameter->numberOfLines];
        try
        {   cout <<"CurrentStep--->>" <<settingParameter->step << endl;
            sceneWidgetVisualizerProxy->vis.getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p, settingParameter->dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, lines);

            sceneWidgetVisualizerProxy->vis.refreshWindowsVTK(sceneWidgetVisualizerProxy->p, settingParameter->dimY, settingParameter->dimX, settingParameter->step, lines, settingParameter->numberOfLines,gridActor);
                  cout <<"Number of lines--->>" << settingParameter->numberOfLines << endl;
            if (settingParameter->numberOfLines > 0) {
                sceneWidgetVisualizerProxy->vis.refreshBuildLoadBalanceLine(lines,settingParameter->numberOfLines,settingParameter->dimY+1,settingParameter->dimX+1,actorBuildLine,colors);
            }
            // Force renderer update
            settingRenderParameter->m_renderer->Modified();

            sceneWidgetVisualizerProxy->vis.buildStepLine(settingParameter->step,singleLineTextStep,singleLineTextPropStep,colors,"White");

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
    SettingParameter* cam = static_cast<SettingParameter*>(clientData);
    
    if (keyPressed == "Up")
    {
        if (cam->step < cam->nsteps * 2)
            cam->step += 1;
        cam->changed = true;

        if (cam->sceneWidget)
            cam->sceneWidget->changedStepNumberWithKeyboardKeys(cam->step);
    }

    if (keyPressed == "Down")
    {
        if (cam->step  > 1)
            cam->step  -= 1;
        cam->changed = true;

        if (cam->sceneWidget)
            cam->sceneWidget->changedStepNumberWithKeyboardKeys(cam->step);
    }
    if (keyPressed.compare("i")==0)
    {
        cam->insertAction = true;
    }
    if (keyPressed.compare("s")==0) // TODO: GB: Why that magic numbers?
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
            cam->sceneWidgetVisualizerProxy->vis.getElementMatrix(cam->step, cam->sceneWidgetVisualizerProxy->p, cam->dimX, cam->dimY, cam->nNodeX, cam->nNodeY, cam->outputFileName, lines);

            cam->sceneWidgetVisualizerProxy->vis.refreshWindowsVTK(cam->sceneWidgetVisualizerProxy->p, cam->dimY, cam->dimX, cam->step, lines, cam->numberOfLines,gridActor);

            //cam->sceneWidgetVisualizerProxy->vis->refreshBuildLoadBalanceLine(lines,cam->numberOfLines,cam->dimY+1,cam->dimX+1,actorBuildLine,colors,pts,cellLines,grid);
            
            cam->sceneWidgetVisualizerProxy->vis.refreshBuildLoadBalanceLine(lines,cam->numberOfLines,cam->dimY+1,cam->dimX+1,actorBuildLine,colors);
            // Force renderer update for keyboard callback too
            renderWindow_->Modified();

            cam->sceneWidgetVisualizerProxy->vis.buildStepLine(cam->step,singleLineTextStep,singleLineTextPropStep,colors,"Red");

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


