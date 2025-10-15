/** @file SceneWidget.h
 * @brief Declaration of the SceneWidget class for 3D visualization.
 * The class wraps QVTKOpenGLNativeWidget which is embedded VTK widget in Qt application. */

#pragma once

#include <QVTKOpenGLNativeWidget.h>
#include <QTimer>
#include <QMouseEvent>
#include <QToolTip>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkTextMapper.h>
#include "visualiserProxy/ISceneWidgetVisualizer.h"
#include "visualiserProxy/SceneWidgetVisualizerFactory.h"
#include "types.h"

class SettingParameter;

/** @class SceneWidget
 * @brief A widget for 3D visualization using VTK in Qt app.
 * 
 * This widget provides a 3D visualization environment with support for multiple model types and interactive features. */
class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT

public:
    /// @brief Constructs a SceneWidget with parent (which will takes care about deallecation of the widget, Qt is using that pattern)
    explicit SceneWidget(QWidget* parent);
    
    /// @brief Destroys the SceneWidget. It is on purpose defined in cpp file to clean up properly incomplete types.
    ~SceneWidget();

    /** @brief Adds a visualizer for the specified config file. It moves position to provided step number.
     *  @param filename The config file to visualize
     *  @param stepNumber The simulation step to display **/
    void addVisualizer(const std::string &filename, int stepNumber);

    /** @brief Updates the visualization widget to show the specified step.
     *  @param stepNumber The step number to display */
    void selectedStepParameter(StepIndex stepNumber);

    /** @brief Switch to a different model type.
     * 
     * This method allows changing the visualization model at runtime.
     * @note This does NOT reload data files. Use reloadData() after switching.
     * 
     * @param modelType The new model type to use */
    void switchModel(ModelType modelType);

    /** @brief Reload data files for the current model.
     * 
     * This method reads selected config file from disk and refreshes the visualization.
     * Call this after switching models or when data files have changed.
     * @note reloading data with not compatible model can crash the application */
    void reloadData();

    /** @brief Clear the entire scene (remove all VTK actors and data).
     * 
     * This method clears the renderer and resets the visualizer state.
     * Call this before loading a new configuration file. */
    void clearScene();

    /** @brief Load a new configuration file and reinitialize the scene.
     * 
     * @param configFileName Path to the new configuration file
     * @param stepNumber Initial step number to display */
    void loadNewConfiguration(const std::string& configFileName, int stepNumber = 0);

    /** @brief Get the current model name.
     * 
     * @return std::string The name of the currently active model */
    auto getCurrentModelName() const
    {
        return sceneWidgetVisualizerProxy->getModelName();
    }

    /** @brief Get the current setting parameter
     *  @return Pointer to the current SettingParameter object */
    const SettingParameter* getSettingParameter() const
    {
        return settingParameter.get();
    }

    /** @brief Callback function for VTK keypress events.
     *  It handles arrow_up and arrow_down keys pressed and changes view of the widget.
     *  Provided argument types are compatible with vtkCallbackCommand::SetCallback
     * 
     * @param caller The VTK object that triggered the event
     * @param eventId The ID of the event
     * @param clientData User data passed to the callback
     * @param callData Event-specific data */
    static void keypressCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

signals:
    /** @brief Signal emitted when step number is changed using keyboard keys (sent from method keypressCallbackFunction)
     *  @param stepNumber The new step number */
    void changedStepNumberWithKeyboardKeys(StepIndex stepNumber);

    /** @brief Signal emitted when total number of steps is read from config file.
     *  @param totalSteps Total number of steps available */
    void totalNumberOfStepsReadFromConfigFile(StepIndex totalSteps);

    /** @brief Signal emitted when available steps are read from config file.
     *  @param availableSteps Vector of available step numbers */
    void availableStepsReadFromConfigFile(std::vector<StepIndex> availableSteps);

public slots:
    /** @brief Slot called when color settings need to be reloaded (at least one of them was changed)
     *
     * This method is typically connected to a signal indicating that color settings
     * have changed. It updates all visual elements in the widget to reflect the
     * current color settings from the ColorSettings singleton. */
    void onColorsReloadRequested();

private slots:
    /// @brief Shows a tooltip at the current mouse position
    void showToolTip();

protected:
    /** @brief Handles mouse move events. The method is overrided for toolTip with position.
     *  @param event The mouse event */
    void mouseMoveEvent(QMouseEvent* event) override;
    
    /** @brief Handles the leave event (when mouse leaves the widget).
     *  The method is overrided to hide toolTip when mouse moved.
     *  @param event The leave event */
    void leaveEvent(QEvent* event) override;

    /// @brief Renders the VTK scene. It needs to be called when reading from config file
    void renderVtkScene();

    /// @brief Upgrades the model in the central panel
    void upgradeModelInCentralPanel();
    
    /// @brief Updates the visualization with current settings
    void updateVisualization();

    /// @brief Enables tooltip display when mouse is above the widget
    void enableToolTipWhenMouseAboveWidget();
    
    /** @brief Updates the tooltip with current mouse position
     *  @param pos Current mouse position in widget coordinates */
    void updateToolTip(const QPoint& pos);
    
    /** @brief Converts screen coordinates to VTK world coordinates
     *  @param pos The screen position in widget coordinates
     *  @return The corresponding world coordinates in VTK space */
    std::array<double, 3> screenToWorldCoordinates(const QPoint& pos) const;
    
    /** @brief Determines which node the mouse is currently over using VTK coordinates
     *  @param worldPos The current position in VTK world coordinates
     *  @return A string indicating which node the mouse is over, or empty string if not over any node */
    QString getNodeAtWorldPosition(const std::array<double, 3>& worldPos) const;

    /// @brief Reads settings from a configuration file
    /// @param filename Path to the configuration file
    void readSettingsFromConfigFile(const std::string &filename);

    /// @brief Sets up the VTK scene, it is called when reading config file
    void setupVtkScene();
    
    /** @param configFilename Path to the configuration file and move view to provided step
     *  @param stepNumber Initial step number to display */
    void setupSettingParameters(const std::string & configFilename, int stepNumber);

    /** @brief Updates the background color from application settings.
     *
     * This method retrieves the current background color from the ColorSettings
     * singleton and applies it to the scene's renderer. It should be called
     * whenever the background color setting changes. */
    void refreshBackgroundColorFromSettings();

    /** @brief Updates the step number text color from application settings.
     *
     * This method retrieves the current text color from the ColorSettings
     * singleton and applies it to any step number text elements in the scene. */
    void refreshStepNumberTextColorFromSettings();

    /** @brief Updates the grid color from application settings.
     *
     * This method retrieves the current grid color from the ColorSettings
     * singleton and applies it to the scene's grid. It should be called
     * whenever the grid color setting changes. */
    void refreshGridColorFromSettings();

private:
    /** @brief Proxy for the scene widget visualizer
     *  This proxy provides access to the visualizer implementation
     *  and is responsible for updating the visualization when settings change. */
    std::unique_ptr<ISceneWidgetVisualizer> sceneWidgetVisualizerProxy;
    
    /// @brief Current setting parameter for the visualization.
    std::unique_ptr<SettingParameter> settingParameter;
    
    /// @brief Currently active model type
    ModelType currentModelType;

    /** @brief Last recorded mouse position in screen coordinates. */
    QPoint m_lastMousePos;
    
    /** @brief Last recorded position in VTK world coordinates. */
    std::array<double, 3> m_lastWorldPos;
    
    /// @brief VTK renderer for the scene: This renderer is responsible for rendering the 3D scene.
    vtkNew<vtkRenderer> renderer;
    
    /** @brief Actor for the grid in the scene: This actor is responsible for rendering the grid in the scene.
     *  The grid provides information about what part was calculated by which node */
    vtkNew<vtkActor> gridActor;

    /// @brief Actor for load balancing lines: This actor is responsible for rendering the load balancing lines.
    vtkNew<vtkActor2D> actorBuildLine;
    
    /// @brief Text mapper for step display: This text mapper is responsible for rendering the step number in the scene.
    vtkNew<vtkTextMapper> singleLineTextStep;

    /** @brief Collection of line segments used for visualization.
     *
     * This vector stores all the line segments that are currently being rendered
     * in the scene. Each segment is from different node. */
    std::vector<Line> lines;
};
