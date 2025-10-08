#include <filesystem>
#include <iostream> // std::cout
#include <QApplication>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
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


SceneWidget::SceneWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
    , m_lastMousePos()
    , sceneWidgetVisualizerProxy{SceneWidgetVisualizerFactory::create(ModelType::Ball)}
    , settingParameter{std::make_unique<SettingParameter>()}
    , settingRenderParameter{std::make_unique<SettingRenderParameter>()}
    , currentModelType{sceneWidgetVisualizerProxy->getModelTypeValue()}
{
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


void SceneWidget::addVisualizer(const std::string &filename, int stepNumber)
{
    if (! std::filesystem::exists(filename))
    {
        throw std::invalid_argument("File '"s + filename + "' does not exist!");
    }

    setupSettingParameters(filename, stepNumber);

    setupVtkScene();

    renderVtkScene();
}

void SceneWidget::setupSettingParameters(const std::string & configFilename, int stepNumber)
{
    readSettingsFromConfigFile(configFilename);

    settingParameter->numberOfLines = 2 * (settingParameter->nNodeX * settingParameter->nNodeY);
    settingParameter->step = stepNumber;
    settingParameter->changed = false;
    settingParameter->firstTime = true;

    sceneWidgetVisualizerProxy->initMatrix(settingParameter->numberOfColumnX, settingParameter->numberOfRowsY);

    settingRenderParameter->m_renderer->SetBackground(settingRenderParameter->colors->GetColor3d("Silver").GetData());

    DEBUG << *settingParameter << endl;
}

void SceneWidget::readSettingsFromConfigFile(const std::string &filename)
{
    Config config(filename);

    {
        ConfigCategory* generalContext = config.getConfigCategory("GENERAL");
        const std::string outputFileNameFromCfg = generalContext->getConfigParameter("output_file_name")->getValue<std::string>();
        settingParameter->outputFileName = prepareOutputFileName(filename, outputFileNameFromCfg);
        settingParameter->numberOfColumnX = generalContext->getConfigParameter("number_of_columns")->getValue<int>();
        settingParameter->numberOfRowsY = generalContext->getConfigParameter("number_of_rows")->getValue<int>();
        settingParameter->nsteps = generalContext->getConfigParameter("number_steps")->getValue<int>();
        emit totalNumberOfStepsReadFromConfigFile(settingParameter->nsteps);
    }

    {
        ConfigCategory* execContext = config.getConfigCategory("DISTRIBUTED");
        settingParameter->nNodeX = execContext->getConfigParameter("number_node_x")->getValue<int>();
        settingParameter->nNodeY = execContext->getConfigParameter("number_node_y")->getValue<int>();
        /// Notice: there are much more params, which are not used: e.g. border_size_x, border_size_y
    }
}

void SceneWidget::setupVtkScene()
{
    sceneWidgetVisualizerProxy->prepareStage(settingParameter->nNodeX, settingParameter->nNodeY);

    renderWindow()->AddRenderer(settingRenderParameter->m_renderer);
    interactor()->SetRenderWindow(renderWindow());

    renderWindow()->SetSize(settingParameter->numberOfColumnX, settingParameter->numberOfRowsY + 10);

    /// An interactor with this style blocks rotation but not zoom.
    /// Use nullptr in SetInteractorStyle to block everything.
    vtkNew<vtkInteractorStyleImage> style;
    interactor()->SetInteractorStyle(style);

    renderWindow()->SetWindowName(QApplication::applicationName().toLocal8Bit().data());

    vtkNew<vtkCallbackCommand> keypressCallback;
    keypressCallback->SetCallback(SceneWidget::keypressCallbackFunction);
    keypressCallback->SetClientData(this);
    interactor()->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);
}

void SceneWidget::keypressCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);

    const string keyPressed = interactor->GetKeySym();
    SceneWidget* sw = static_cast<SceneWidget*>(clientData);
    SettingParameter* sp = sw->settingParameter.get();

    if (keyPressed == "Up")
    {
        if (sp->step < sp->nsteps * 2)
            sp->step += 1;
        sp->changed = true;

        if (sw)
            sw->changedStepNumberWithKeyboardKeys(sp->step);
    }
    else if (keyPressed == "Down")
    {
        if (sp->step  > 1)
            sp->step  -= 1;
        sp->changed = true;

        if (sw)
            sw->changedStepNumberWithKeyboardKeys(sp->step);
    }

    if (sp->changed || sp->firstTime)
    {
        try
        {
            sw->updateVisualization();

            // Force renderer update for keyboard callback too
            sw->renderWindow()->Modified();
            sw->renderWindow()->Render();
        }
        catch(const std::runtime_error& re)
        {
            std::cerr << "Runtime error di getElementMatrix: " << re.what() << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cerr << "Error occurred: " << ex.what() << std::endl;
        }

        sp->firstTime = false;
        sp->changed = false;
    }
}

void SceneWidget::renderVtkScene()
{
    DEBUG << "DEBUG: Starting " << __FUNCTION__ << endl;
    sceneWidgetVisualizerProxy->readStepsOffsetsForAllNodesFromFiles(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);
    DEBUG << "DEBUG: Hashmap loaded successfully" << endl;
    emit availableStepsReadFromConfigFile(sceneWidgetVisualizerProxy->availableSteps());

    std::vector<Line> lines(settingParameter->numberOfLines);

    sceneWidgetVisualizerProxy->readStageStateFromFilesForStep(settingParameter.get(), &lines[0]);
    DEBUG << "DEBUG: readStageStateFromFilesForStep completed" << endl;

    sceneWidgetVisualizerProxy->drawWithVTK(settingParameter->numberOfRowsY, settingParameter->numberOfColumnX, settingParameter->step, &lines[0], settingRenderParameter->m_renderer, gridActor);
    DEBUG << "DEBUG: drawWithVTK completed" << endl;

    vtkNew<vtkCellArray> cellLines;
    vtkNew<vtkPoints> pts;
    vtkNew<vtkPolyData> grid;
    sceneWidgetVisualizerProxy->getVisualizer().buildLoadBalanceLine(&lines[0], settingParameter->numberOfLines, settingParameter->numberOfRowsY+1, settingParameter->numberOfColumnX+1, pts, cellLines, grid,settingRenderParameter->colors,settingRenderParameter->m_renderer,actorBuildLine);
    DEBUG << "DEBUG: buildLoadBalanceLine completed" << endl;

    sceneWidgetVisualizerProxy->getVisualizer().buildStepText(settingParameter->step, settingParameter->font_size, settingRenderParameter->colors, singleLineTextPropStep, singleLineTextStep, settingRenderParameter->m_renderer);

    // Render
    renderWindow()->Render();
    interactor()->Initialize();
    interactor()->Enable();
}


void SceneWidget::increaseCountUp()
{
    if (settingParameter->step < settingParameter->nsteps * 2)
    {
        settingParameter->step += 1;
        settingParameter->changed = true;
    }
    upgradeModelInCentralPanel();
}

void SceneWidget::decreaseCountDown()
{
    if (settingParameter->step > 1)
    {
        settingParameter->step -= 1;
        settingParameter->changed = true;
    }

    upgradeModelInCentralPanel();
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

void SceneWidget::selectedStepParameter(StepIndex stepNumber)
{
    settingParameter->step = stepNumber;
    settingParameter->changed = true;
    upgradeModelInCentralPanel();
}


void SceneWidget::updateVisualization()
{
    std::vector<Line> lines(settingParameter->numberOfLines);

    sceneWidgetVisualizerProxy->readStageStateFromFilesForStep(
        settingParameter.get(),
        &lines[0]
    );

    sceneWidgetVisualizerProxy->refreshWindowsVTK(
        settingParameter->numberOfRowsY,
        settingParameter->numberOfColumnX,
        settingParameter->step,
        &lines[0],
        settingParameter->numberOfLines,
        gridActor
    );

    //visualiserProxy->vis->refreshBuildLoadBalanceLine(lines, cam->numberOfLines, cam->dimY+1, cam->dimX+1, actorBuildLine, colors, pts, cellLines, grid);

    DEBUG << "--->> CurrentStep:" << settingParameter->step 
          << ". Number of lines:" << settingParameter->numberOfLines << endl;

    if (settingParameter->numberOfLines > 0)
    {
        sceneWidgetVisualizerProxy->getVisualizer().refreshBuildLoadBalanceLine(
            &lines[0], 
            settingParameter->numberOfLines, 
            settingParameter->numberOfRowsY + 1,
            settingParameter->numberOfColumnX + 1,
            actorBuildLine, 
            settingRenderParameter->colors
        );
    }

    const std::string stepLineColor{"red"};
    sceneWidgetVisualizerProxy->getVisualizer().buildStepLine(
        settingParameter->step, 
        singleLineTextStep, 
        singleLineTextPropStep, 
        settingRenderParameter->colors, 
        stepLineColor
    );
}

void SceneWidget::upgradeModelInCentralPanel()
{
    if (!settingParameter->changed && !settingParameter->firstTime)
        return;

    try
    {
        updateVisualization();
        
        // Force renderer update
        settingRenderParameter->m_renderer->Modified();
        renderWindow()->Render();
        QApplication::processEvents();
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
}

void SceneWidget::switchModel(ModelType modelType)
{
    if (modelType == currentModelType)
    {
        return; // Already using this model
    }

    // Create new visualizer with the selected model
    sceneWidgetVisualizerProxy = SceneWidgetVisualizerFactory::create(modelType);
    currentModelType = modelType;

    // Reinitialize the matrix with current dimensions
    sceneWidgetVisualizerProxy->initMatrix(settingParameter->numberOfColumnX, settingParameter->numberOfRowsY);

    std::cout << "Switched to model: " << sceneWidgetVisualizerProxy->getModelName() << std::endl;
    std::cout << "Note: Use 'Reload Data' to load data files for the new model." << std::endl;
}

void SceneWidget::reloadData()
{
    try
    {
        std::cout << "Reloading data for model: " << sceneWidgetVisualizerProxy->getModelName() << std::endl;

        // Clear existing stage data to avoid duplicates
        sceneWidgetVisualizerProxy->clearStage();
        
        // Reinitialize and load data
        sceneWidgetVisualizerProxy->prepareStage(settingParameter->nNodeX, settingParameter->nNodeY);
        sceneWidgetVisualizerProxy->readStepsOffsetsForAllNodesFromFiles(
            settingParameter->nNodeX, 
            settingParameter->nNodeY, 
            settingParameter->outputFileName
        );

        // Force a full refresh
        settingParameter->firstTime = true;
        settingParameter->changed = true;
        upgradeModelInCentralPanel();

        std::cout << "Data reloaded successfully." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to reload data: " << e.what() << std::endl;
        throw;
    }
}

void SceneWidget::clearScene()
{
    std::cout << "Clearing scene..." << std::endl;
    
    // Clear the renderer
    settingRenderParameter->m_renderer->RemoveAllViewProps();
    
    // Clear stage data
    sceneWidgetVisualizerProxy->clearStage();
    
    // Reset VTK actors
    gridActor = vtkNew<vtkActor>();
    actorBuildLine = vtkNew<vtkActor2D>();
    
    std::cout << "Scene cleared." << std::endl;
}

void SceneWidget::loadNewConfiguration(const std::string& configFileName, int stepNumber)
{
    std::cout << "Loading new configuration: " << configFileName << std::endl;
    
    try
    {
        // Clear existing scene
        clearScene();
        
        // Setup new parameters from config file
        setupSettingParameters(configFileName, stepNumber);
        
        // Setup VTK scene (without adding renderer again - it's already added)
        sceneWidgetVisualizerProxy->prepareStage(settingParameter->nNodeX, settingParameter->nNodeY);
        
        // Render the scene with new data
        renderVtkScene();
        
        std::cout << "Configuration loaded successfully: " << configFileName << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load configuration: " << e.what() << std::endl;
        throw;
    }
}
