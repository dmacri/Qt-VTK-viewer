#pragma once

#include <QObject>
#include <QString>
#include <functional>

class vtkRenderWindow;

/**
 * @brief Class responsible for exporting VTK render window content to OGG video format
 * 
 * This class encapsulates all VTK-specific video recording functionality,
 * keeping the main window code clean from VTK dependencies.
 */
class VideoExporter : public QObject
{
    Q_OBJECT

public:
    explicit VideoExporter(QObject* parent = nullptr);
    ~VideoExporter();

    /**
     * @brief Export video from VTK render window
     * 
     * @param renderWindow VTK render window to capture frames from
     * @param outputFilePath Path where the video file will be saved
     * @param fps Frames per second for the output video
     * @param totalSteps Total number of steps/frames to capture
     * @param updateStepCallback Callback function to update visualization for each step
     * @param progressCallback Callback function to report progress (current step, total steps)
     * @param cancelledCallback Callback function to check if export was cancelled
     * @throws std::runtime_error if export fails or is cancelled
     */
    void exportVideo(
        vtkRenderWindow* renderWindow,
        const QString& outputFilePath,
        int fps,
        int totalSteps,
        std::function<void(int)> updateStepCallback,
        std::function<void(int, int)> progressCallback,
        std::function<bool()> cancelledCallback
    );

signals:
    /**
     * @brief Emitted when export progress changes
     * @param currentStep Current step being processed
     * @param totalSteps Total number of steps
     */
    void progressChanged(int currentStep, int totalSteps);

    /**
     * @brief Emitted when export is complete
     */
    void exportCompleted();

    /**
     * @brief Emitted when export fails
     * @param errorMessage Description of the error
     */
    void exportFailed(const QString& errorMessage);
};
