/** @file SceneWidget.cpp
 * @brief Implementation of the SceneWidget class for 3D visualization. */

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
#include "widgets/ColorSettings.h"


namespace
{
/** @brief Prepares the output file path for saving visualization data
 *  @param configFile Path to the configuration file
 *  @param outputFileNameFromCfg Output filename from configuration
 *  @return Full path to the output file */
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

vtkColor3d toVtkColor(QColor color)
{
    return vtkColor3d{
        color.redF(),
        color.greenF(),
        color.blueF()
    };
}
} // namespace


SceneWidget::SceneWidget(QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
    , m_lastMousePos()
    , sceneWidgetVisualizerProxy{SceneWidgetVisualizerFactory::create(ModelType::Ball)}
    , settingParameter{std::make_unique<SettingParameter>()}
    , currentModelType{sceneWidgetVisualizerProxy->getModelTypeValue()}
{
    enableToolTipWhenMouseAboveWidget();

    connect(&ColorSettings::instance(), &ColorSettings::colorsChanged, this, &SceneWidget::onColorsReloadRequested);
}

void SceneWidget::enableToolTipWhenMouseAboveWidget()
{
    setMouseTracking(/*enable=*/true);
    setAttribute(Qt::WA_AlwaysShowToolTips);
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

    sceneWidgetVisualizerProxy->initMatrix(settingParameter->numberOfColumnX, settingParameter->numberOfRowsY);

    refreshBackgroundColorFromSettings();
}

void SceneWidget::refreshGridColorFromSettings()
{
    const auto color = ColorSettings::instance().gridColor();

    actorBuildLine->GetProperty()->SetColor(toVtkColor(color).GetData());
    actorBuildLine->GetProperty()->Modified();

    renderer->Modified();
    renderWindow()->Render();
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

    renderWindow()->AddRenderer(renderer);
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

    if (sp->changed)
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

        sp->changed = false;
    }
}

void SceneWidget::renderVtkScene()
{
    sceneWidgetVisualizerProxy->readStepsOffsetsForAllNodesFromFiles(settingParameter->nNodeX, settingParameter->nNodeY, settingParameter->outputFileName);

    emit availableStepsReadFromConfigFile(sceneWidgetVisualizerProxy->availableSteps());

    lines.resize(settingParameter->numberOfLines);
    sceneWidgetVisualizerProxy->readStageStateFromFilesForStep(settingParameter.get(), &lines[0]);

    sceneWidgetVisualizerProxy->drawWithVTK(settingParameter->numberOfRowsY, settingParameter->numberOfColumnX, settingParameter->step, renderer, gridActor);

    sceneWidgetVisualizerProxy->getVisualizer().buildLoadBalanceLine(lines, settingParameter->numberOfColumnX+1, renderer, actorBuildLine);

    sceneWidgetVisualizerProxy->getVisualizer().buildStepText(settingParameter->step, settingParameter->font_size, singleLineTextStep, renderer);

    // Render
    renderWindow()->Render();
    interactor()->Initialize();
    interactor()->Enable();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* event)
{
    QVTKOpenGLNativeWidget::mouseMoveEvent(event);
    updateToolTip(event->pos());
}

void SceneWidget::leaveEvent(QEvent* event)
{
    QVTKOpenGLNativeWidget::leaveEvent(event);
    QToolTip::hideText();
}

std::array<double, 3> SceneWidget::screenToWorldCoordinates(const QPoint& pos) const
{
    std::array<double, 3> worldPos = {0.0, 0.0, 0.0};
    
    if (!renderer || ! renderWindow())
    {
        return worldPos;
    }
    
    // Convert screen coordinates to VTK display coordinates
    int* size = renderWindow()->GetSize();
    double displayPos[3] = {
        static_cast<double>(pos.x()),
        static_cast<double>(size[1] - pos.y()), // Flip Y coordinate
        0.0
    };
    
    // Convert display coordinates to world coordinates
    renderer->SetDisplayPoint(displayPos);
    renderer->DisplayToWorld();
    renderer->GetWorldPoint(worldPos.data());
    
    return worldPos;
}

QString SceneWidget::getNodeAtWorldPosition(const std::array<double, 3>& worldPos) const
{
    if (!settingParameter || !sceneWidgetVisualizerProxy || !renderer)
    {
        return {};
    }

    // Get the bounds of the entire scene
    double* bounds = renderer->ComputeVisiblePropBounds();
    if (!bounds)
    {
        return {};
    }

    // Check if the position is within the scene bounds
    if (worldPos[0] < bounds[0] || worldPos[0] > bounds[1] ||
        worldPos[1] < bounds[2] || worldPos[1] > bounds[3])
    {
        return {}; // Outside scene bounds
    }

    // Calculate the width and height of each node's area in world coordinates
    const double sceneWidth = bounds[1] - bounds[0];
    const double sceneHeight = bounds[3] - bounds[2];
    
    const double nodeWidth = sceneWidth / settingParameter->nNodeX;
    const double nodeHeight = sceneHeight / settingParameter->nNodeY;

    // Calculate which node the position is in (0-based indices)
    const int nodeX = (worldPos[0] - bounds[0]) / nodeWidth;
    const int nodeY = (worldPos[1] - bounds[2]) / nodeHeight;

    // Check if the calculated node is within bounds
    if (nodeX >= 0 && nodeX < settingParameter->nNodeX &&
        nodeY >= 0 && nodeY < settingParameter->nNodeY)
    {
        return QString("Node [%1, %2]").arg(nodeX).arg(nodeY);
    }

    return {}; // Outside node grid
}

const Line* SceneWidget::findNearestLine(const std::array<double, 3>& worldPos, size_t& lineIndex, double& distanceSquared) const
{
    if (!settingParameter || !sceneWidgetVisualizerProxy)
    {
        return nullptr;
    }

    // const auto& lines = sceneWidgetVisualizerProxy->getVisualizer().getLines();
    if (lines.empty())
    {
        return nullptr;
    }

    constexpr double threshold = 2; // Threshold for line selection (in world coordinates)
    constexpr double thresholdSq = threshold * threshold;
    
    const Line* nearestLine = nullptr;
    double minDistanceSq = std::numeric_limits<double>::max();
    size_t foundIndex = 0;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const auto& line = lines[i];
        
        // Calculate squared distance from point to line segment
        const double lineLengthSq = (line.x2 - line.x1) * (line.x2 - line.x1) + 
                                   (line.y2 - line.y1) * (line.y2 - line.y1);
        
        if (lineLengthSq < 1e-10) // Skip zero-length lines
            continue;
            
        const double t = std::max(0.0, std::min(1.0, 
            ((worldPos[0] - line.x1) * (line.x2 - line.x1) + 
             (worldPos[1] - line.y1) * (line.y2 - line.y1)) / lineLengthSq));
             
        const double projX = line.x1 + t * (line.x2 - line.x1);
        const double projY = line.y1 + t * (line.y2 - line.y1);
        
        const double dx = worldPos[0] - projX;
        const double dy = worldPos[1] - projY;
        const double distSq = dx * dx + dy * dy;
        
        if (distSq < minDistanceSq && distSq <= thresholdSq)
        {
            minDistanceSq = distSq;
            nearestLine = &line;
            foundIndex = i;
        }
    }

    if (nearestLine)
    {
        lineIndex = foundIndex;
        distanceSquared = minDistanceSq;
    }
    
    return nearestLine;
}

void SceneWidget::updateToolTip(const QPoint& pos)
{
    if (!renderer || !renderWindow())
    {
        return;
    }
    
    m_lastMousePos = pos;
    m_lastWorldPos = screenToWorldCoordinates(pos);
    
    // Check if we're over a line
    size_t lineIndex = 0;
    double distanceSq = 0.0;
    const Line* nearestLine = findNearestLine(m_lastWorldPos, lineIndex, distanceSq);
    
    // Prepare tooltip text with VTK coordinates
    QString tooltipText = QString("World Position: (x: %1, y: %2, z: %3)")
        .arg(m_lastWorldPos[0], 0, 'f', 2)
        .arg(m_lastWorldPos[1], 0, 'f', 2)
        .arg(m_lastWorldPos[2], 0, 'f', 2);
    
    if (nearestLine)
    {
        // Show line information
        tooltipText += QString("\n\nLine %1:").arg(lineIndex);
        tooltipText += QString("\n  From: (%1, %2)")
            .arg(nearestLine->x1, 0, 'f', 2)
            .arg(nearestLine->y1, 0, 'f', 2);
        tooltipText += QString("\n  To:   (%1, %2)")
            .arg(nearestLine->x2, 0, 'f', 2)
            .arg(nearestLine->y2, 0, 'f', 2);
    }
    else
    {
        // Show node information if not over a line
        QString nodeInfo = getNodeAtWorldPosition(m_lastWorldPos);
        if (!nodeInfo.isEmpty())
        {
            tooltipText += QString("\n%1").arg(nodeInfo);
        }
    }
    
    // Show tooltip at the current mouse position
    QPoint globalPos = mapToGlobal(pos);
    QToolTip::showText(globalPos, 
                      tooltipText,
                      this,
                      QRect(pos, QSize(1, 1)),
                      0); // Show until mouse moves out
}

void SceneWidget::showToolTip()
{
    updateToolTip(m_lastMousePos);
}

void SceneWidget::selectedStepParameter(StepIndex stepNumber)
{
    settingParameter->step = stepNumber;
    settingParameter->changed = true;
    upgradeModelInCentralPanel();
}


void SceneWidget::updateVisualization()
{
    lines.resize(settingParameter->numberOfLines);

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

    if (settingParameter->numberOfLines > 0)
    {
        sceneWidgetVisualizerProxy->getVisualizer().refreshBuildLoadBalanceLine(
            &lines[0], 
            settingParameter->numberOfLines, 
            settingParameter->numberOfRowsY + 1,
            settingParameter->numberOfColumnX + 1,
            actorBuildLine
        );
    }

    sceneWidgetVisualizerProxy->getVisualizer().buildStepLine(settingParameter->step, singleLineTextStep);
}

void SceneWidget::upgradeModelInCentralPanel()
{
    if (!settingParameter->changed)
        return;

    try
    {
        updateVisualization();
        
        // Force renderer update
        renderer->Modified();
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
}

void SceneWidget::reloadData()
{
    try
    {
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
        settingParameter->changed = true;
        upgradeModelInCentralPanel();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to reload data: " << e.what() << std::endl;
        throw;
    }
}

void SceneWidget::clearScene()
{    
    // Clear the renderer
    renderer->RemoveAllViewProps();
    
    // Clear stage data
    sceneWidgetVisualizerProxy->clearStage();
    
    // Reset VTK actors
    gridActor.Reset();
    actorBuildLine.Reset();
}

void SceneWidget::loadNewConfiguration(const std::string& configFileName, int stepNumber)
{    
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
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load configuration: " << e.what() << std::endl;
        throw;
    }
}

void SceneWidget::onColorsReloadRequested()
{
    refreshBackgroundColorFromSettings();
    refreshStepNumberTextColorFromSettings();
    refreshGridColorFromSettings();
}
void SceneWidget::refreshBackgroundColorFromSettings()
{
    const auto color = ColorSettings::instance().backgroundColor();
    renderer->SetBackground(toVtkColor(color).GetData());
    renderer->Modified();
    renderWindow()->Render();
}

void SceneWidget::refreshStepNumberTextColorFromSettings()
{
    const auto color = ColorSettings::instance().textColor();

    auto realTextProp = singleLineTextStep->GetTextProperty();
    realTextProp->SetColor(toVtkColor(color).GetData());
    realTextProp->Modified();

    renderer->Modified();
    renderWindow()->Render();
}
