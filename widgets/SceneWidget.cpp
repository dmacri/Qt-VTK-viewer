#include <filesystem>
#include <iostream> // std::cout
#include <QApplication>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include "SceneWidget.h"
#include "config/Config.h"
#include "visualiser/Line.h"
#include "visualiser/Visualizer.hpp"
#include "visualiser/SettingParameter.h"
#include "visualiser/SettingRenderParameter.h"

constexpr bool DEBUG_LOGS_ENABLED = false;
#define DEBUG if constexpr (DEBUG_LOGS_ENABLED) std::cout


namespace
{
std::string prepareOutputFileName(const std::string& configFile, const std::string& outputFileNameFromCfg)
{
    namespace fs = std::filesystem;

    // Step 1: prepare output directory
    fs::path configPath(configFile);
    fs::path outputDir = configPath.parent_path() / "Output";
    fs::create_directories(outputDir);  // ensure that directory exists

    // Step 2: build full output file path
    return (outputDir / outputFileNameFromCfg).string();
}
} // namespace


SceneWidget::SceneWidget(QWidget* parent, int argc, char *argv[])
    : QVTKOpenGLNativeWidget(parent)
    , m_lastMousePos()
    , sceneWidgetVisualizerProxy{std::make_unique<SceneWidgetVisualizerProxy>()}
    , settingParameter{std::make_unique<SettingParameter>()}
    , settingRenderParameter{std::make_unique<SettingRenderParameter>()}
{
    settingParameter->sceneWidgetVisualizerProxy = sceneWidgetVisualizerProxy.get();

    enableToolTipWhenMouseAboveWidget();
}

void SceneWidget::enableToolTipWhenMouseAboveWidget()
{
    m_toolTipTimer.setSingleShot(true);

    constexpr int delayBeforeShowingToolTipInMs = 1'000;
    m_toolTipTimer.setInterval(delayBeforeShowingToolTipInMs);
    connect(&m_toolTipTimer, &QTimer::timeout, this, &SceneWidget::showToolTip);

    setMouseTracking(/*enable=*/true);
}

SceneWidget::~SceneWidget() = default;


void SceneWidget::addVisualizer(const std::string &filename)
{
    if (! std::filesystem::exists(filename))
    {
        throw std::invalid_argument("File '"s + filename + "' does not exist!");
    }

    setupSettingParameters(filename);

    setupVtkScene();

    renderVtkScene();
}

void SceneWidget::readSettingsFromConfigFile(const std::string &filename)
{
    Config config(filename);

    {
        ConfigCategory* generalContext = config.getConfigCategory("GENERAL");
        const std::string outputFileNameFromCfg = generalContext->getConfigParameter("output_file_name")->getValue<std::string>();
        settingParameter->outputFileName = prepareOutputFileName(filename, outputFileNameFromCfg);
        settingParameter->dimX = generalContext->getConfigParameter("number_of_columns")->getValue<int>();
        settingParameter->dimY = generalContext->getConfigParameter("number_of_rows")->getValue<int>();
        settingParameter->nsteps = generalContext->getConfigParameter("number_steps")->getValue<int>();
    }

    {
        ConfigCategory* execContext = config.getConfigCategory("DISTRIBUTED");
        // int borderSizeX = execContext->getConfigParameter("border_size_x")->getValue<int>(); // not used
        // int borderSizeY = execContext->getConfigParameter("border_size_y")->getValue<int>(); // not used
        // int numBorders = 1; // not used
        settingParameter->nNodeX = execContext->getConfigParameter("number_node_x")->getValue<int>();
        settingParameter->nNodeY = execContext->getConfigParameter("number_node_y")->getValue<int>();
    }
}

void SceneWidget::setupSettingParameters(const std::string & configFilename)
{
    readSettingsFromConfigFile(configFilename);

    settingParameter->numberOfLines = 2 * (settingParameter->nNodeX * settingParameter->nNodeY);
    settingParameter->step = 1;
    settingParameter->changed = false;
    settingParameter->firstTime = true;
    settingParameter->insertAction = false;

    sceneWidgetVisualizerProxy->initMatrix(settingParameter->dimX, settingParameter->dimY);
    settingParameter->sceneWidgetVisualizerProxy->p = sceneWidgetVisualizerProxy->p;

    settingRenderParameter->m_renderer->SetBackground(settingRenderParameter->colors->GetColor3d("Silver").GetData());

    // Set the scene widget pointer in the settings
    settingParameter->sceneWidget = this;

    cout << *settingParameter << endl;
}

void SceneWidget::setupVtkScene()
{
    sceneWidgetVisualizerProxy->vis.hashMap.resize(settingParameter->nNodeX * settingParameter->nNodeY);

    maxStepVisited.resize(settingParameter->nNodeX * settingParameter->nNodeY);

    renderWindow()->AddRenderer(settingRenderParameter->m_renderer);
    interactor()->SetRenderWindow(renderWindow());

    renderWindow()->SetSize(settingParameter->dimX, settingParameter->dimY + 10);

    /// An interactor with this style blocks rotation but not zoom.
    /// Use nullptr in SetInteractorStyle to block everything.
    vtkNew<vtkInteractorStyleImage> style;
    interactor()->SetInteractorStyle(style);

    renderWindow()->SetWindowName("Visualizer");
}

void SceneWidget::renderVtkScene()
{
    DEBUG << "DEBUG: Starting " << __FUNCTION__ << endl;
    DEBUG << "DEBUG: Loading hashmap from file..." << endl;
    sceneWidgetVisualizerProxy->vis.loadHashmapFromFile(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);
    DEBUG << "DEBUG: Hashmap loaded successfully" << endl;

    vtkNew<vtkCallbackCommand> keypressCallback;
    keypressCallback->SetCallback(SceneWidget::keypressCallbackFunction);
    keypressCallback->SetClientData(this);
    interactor()->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

    std::vector<Line> lines;
    lines.resize(settingParameter->numberOfLines);
    DEBUG << "DEBUG: Allocated lines array" << endl;

    DEBUG << "DEBUG: Calling getElementMatrix..." << endl;
    sceneWidgetVisualizerProxy->vis.getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p, settingParameter->dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, &lines[0]);
    DEBUG << "DEBUG: getElementMatrix completed" << endl;

    DEBUG << "DEBUG: Calling drawWithVTK..." << endl;
    sceneWidgetVisualizerProxy->vis.drawWithVTK(sceneWidgetVisualizerProxy->p, settingParameter->dimY, settingParameter->dimX, settingParameter->step, &lines[0], settingParameter->numberOfLines, settingParameter->edittext, settingRenderParameter->m_renderer, gridActor);
    DEBUG << "DEBUG: drawWithVTK completed" << endl;
    
    DEBUG << "DEBUG: Calling buildLoadBalanceLine..." << endl;
    vtkNew<vtkCellArray> cellLines;
    vtkNew<vtkPoints> pts;
    vtkNew<vtkPolyData> grid;
    sceneWidgetVisualizerProxy->vis.buildLoadBalanceLine(&lines[0], settingParameter->numberOfLines, settingParameter->dimY+1, settingParameter->dimX+1, pts, cellLines, grid,settingRenderParameter->colors,settingRenderParameter->m_renderer,actorBuildLine);
    DEBUG << "DEBUG: buildLoadBalanceLine completed" << endl;

    sceneWidgetVisualizerProxy->vis.buildStepText(settingParameter->step, settingParameter->font_size, settingRenderParameter->colors, singleLineTextPropStep, singleLineTextStep, settingRenderParameter->m_renderer);

    // Render
    renderWindow()->Render();
    interactor()->Initialize();
    interactor()->Enable();
}

void SceneWidget::keypressCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);

    const string keyPressed = interactor->GetKeySym();
    SceneWidget* sw = static_cast<SceneWidget*>(clientData);
    SettingParameter* sp = sw->settingParameter.get();

    SceneWidgetVisualizerProxy* visualiserProxy = sp->sceneWidgetVisualizerProxy;

    if (keyPressed == "Up")
    {
        if (sp->step < sp->nsteps * 2)
            sp->step += 1;
        sp->changed = true;

        if (sp->sceneWidget)
            sp->sceneWidget->changedStepNumberWithKeyboardKeys(sp->step);
    }
    else if (keyPressed == "Down")
    {
        if (sp->step  > 1)
            sp->step  -= 1;
        sp->changed = true;

        if (sp->sceneWidget)
            sp->sceneWidget->changedStepNumberWithKeyboardKeys(sp->step);
    }
    else if (keyPressed == "i")
    {
        sp->insertAction = true;
    }

    if (sp->changed || sp->firstTime)
    {
        std::vector<Line> lines;
        lines.resize(sp->numberOfLines);
        try
        {
            visualiserProxy->vis.getElementMatrix(sp->step, visualiserProxy->p, sp->dimX, sp->dimY, sp->nNodeX, sp->nNodeY, sp->outputFileName, &lines[0]);

            visualiserProxy->vis.refreshWindowsVTK(visualiserProxy->p, sp->dimY, sp->dimX, sp->step, &lines[0], sp->numberOfLines, sw->gridActor);

            //visualiserProxy->vis->refreshBuildLoadBalanceLine(lines, cam->numberOfLines, cam->dimY+1, cam->dimX+1, actorBuildLine, colors, pts, cellLines, grid);

            visualiserProxy->vis.refreshBuildLoadBalanceLine(&lines[0], sp->numberOfLines, sp->dimY+1, sp->dimX+1, sw->actorBuildLine, sw->colors);
            // Force renderer update for keyboard callback too
            sp->sceneWidget->renderWindow()->Modified();

            visualiserProxy->vis.buildStepLine(sp->step, sw->singleLineTextStep, sw->singleLineTextPropStep, sw->colors, "Red");
        }
        catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }
        sp->sceneWidget->renderWindow()->Render();
        sp->firstTime = false;
        sp->changed = false;
    }
}


void SceneWidget::increaseCountUp()
{
    if (settingParameter->step < settingParameter->nsteps * 2)
    {
        settingParameter->step += 1;
        settingParameter->changed = true;
    }
    SceneWidget::upgradeModelInCentralPanel();
}

void SceneWidget::decreaseCountDown()
{
    if (settingParameter->step  > 1)
    {
        settingParameter->step  -= 1;
        settingParameter->changed = true;
    }

    SceneWidget::upgradeModelInCentralPanel();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* event)
{
    QVTKOpenGLNativeWidget::mouseMoveEvent(event);
    
    // Update the last mouse position
    m_lastMousePos = event->pos();
    
    // Restart the tooltip timer
    m_toolTipTimer.stop();
    m_toolTipTimer.start();
}

void SceneWidget::leaveEvent(QEvent* event)
{
    QVTKOpenGLNativeWidget::leaveEvent(event);
    m_toolTipTimer.stop();
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
    upgradeModelInCentralPanel();
}


void SceneWidget::upgradeModelInCentralPanel()
{
    if (settingParameter->changed || settingParameter->firstTime )
    {
        std::vector<Line> lines;
        lines.resize(settingParameter->numberOfLines);

        try
        {
            sceneWidgetVisualizerProxy->vis.getElementMatrix(settingParameter->step, sceneWidgetVisualizerProxy->p, settingParameter->dimX, settingParameter->dimY, settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName, &lines[0]);
            sceneWidgetVisualizerProxy->vis.refreshWindowsVTK(sceneWidgetVisualizerProxy->p, settingParameter->dimY, settingParameter->dimX, settingParameter->step, &lines[0], settingParameter->numberOfLines, gridActor);
            DEBUG <<"--->> CurrentStep:" <<settingParameter->step << ". Number of lines:" << settingParameter->numberOfLines << endl;
            if (settingParameter->numberOfLines > 0)
            {
                sceneWidgetVisualizerProxy->vis.refreshBuildLoadBalanceLine(&lines[0], settingParameter->numberOfLines, settingParameter->dimY+1, settingParameter->dimX+1, actorBuildLine, settingRenderParameter->colors);
            }
            // Force renderer update
            settingRenderParameter->m_renderer->Modified();

            sceneWidgetVisualizerProxy->vis.buildStepLine(settingParameter->step, singleLineTextStep, singleLineTextPropStep, settingRenderParameter->colors, "White");
        }
        catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error getElementMatrix: " << re.what() << std::endl;
            throw;
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
            throw;
        }

        settingParameter->firstTime = false;
        settingParameter->changed = false;
        renderWindow()->Render();
        QApplication::processEvents();
    }
}
