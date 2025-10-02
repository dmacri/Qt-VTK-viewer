#include "VideoExporter.h"

#include <vtkOggTheoraWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>
#include <vtkNew.h>

#include <stdexcept>

VideoExporter::VideoExporter(QObject* parent)
    : QObject(parent)
{
}

VideoExporter::~VideoExporter() = default;

void VideoExporter::exportVideo(
    vtkRenderWindow* renderWindow,
    const QString& outputFilePath,
    int fps,
    int totalSteps,
    std::function<void(int)> updateStepCallback,
    std::function<void(int, int)> progressCallback,
    std::function<bool()> cancelledCallback)
{
    if (!renderWindow)
    {
        throw std::runtime_error("Render window is null");
    }

    if (totalSteps <= 0)
    {
        throw std::runtime_error("Total steps must be greater than 0");
    }

    // Setup VTK video writer
    vtkNew<vtkOggTheoraWriter> writer;
    writer->SetFileName(outputFilePath.toStdString().c_str());
    writer->SetRate(fps);
    writer->SetQuality(2); // Quality 0-2, where 2 is highest

    // Setup window to image filter
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetScale(1); // Image quality scale
    windowToImageFilter->SetInputBufferTypeToRGB();
    windowToImageFilter->ReadFrontBufferOff(); // Read from back buffer

    // Connect filter to writer
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());

    // Start writing
    writer->Start();

    try
    {
        // Iterate through all steps and capture frames
        for (int step = 1; step <= totalSteps; ++step)
        {
            // Check if user cancelled
            if (cancelledCallback && cancelledCallback())
            {
                writer->End();
                throw std::runtime_error("Video export cancelled by user");
            }

            // Report progress
            if (progressCallback)
            {
                progressCallback(step, totalSteps);
            }
            emit progressChanged(step, totalSteps);

            // Update visualization for this step
            if (updateStepCallback)
            {
                updateStepCallback(step);
            }

            // Force render
            renderWindow->Render();

            // Capture frame
            windowToImageFilter->Modified();
            writer->Write();
        }

        // Finalize video
        writer->End();
        emit exportCompleted();
    }
    catch (const std::exception& e)
    {
        writer->End();
        emit exportFailed(QString::fromStdString(e.what()));
        throw;
    }
}
