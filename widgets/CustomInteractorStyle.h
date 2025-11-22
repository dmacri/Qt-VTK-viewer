/** @file CustomInteractorStyle.h
 * @brief Custom VTK interactor style for cursor-based zoom.
 * 
 * This interactor style extends vtkInteractorStyleImage to provide
 * zoom functionality that keeps the point under the cursor stationary. */

#pragma once

#include <vtkInteractorStyleImage.h>
#include <functional>

/** @brief Custom interactor style for cursor-based zoom.
 * 
 * Extends vtkInteractorStyleImage to override mouse wheel events
 * and zoom towards the cursor position instead of the screen center. */
class CustomInteractorStyle : public vtkInteractorStyleImage
{
public:
    static CustomInteractorStyle* New();
    vtkTypeMacro(CustomInteractorStyle, vtkInteractorStyleImage);

    /** @brief Handle mouse wheel forward event (zoom in).
     * 
     * Zooms in towards the cursor position. */
    void OnMouseWheelForward() override;

    /** @brief Handle mouse wheel backward event (zoom out).
     * 
     * Zooms out away from the cursor position. */
    void OnMouseWheelBackward() override;

    /** @brief Handle left mouse button press event.
     * 
     * Starts panning when left button is pressed. */
    void OnLeftButtonDown() override;

    /** @brief Handle left mouse button release event.
     * 
     * Stops panning when left button is released. */
    void OnLeftButtonUp() override;

    /** @brief Handle mouse move event during panning.
     * 
     * Pans the view when left button is held down. */
    void OnMouseMove() override;

    /** @brief Set callback for operation start (zoom/pan).
     * 
     * Called when a long-running operation (zoom or pan) starts.
     * Useful for showing wait cursor.
     * 
     * @param callback Function to call when operation starts */
    void SetOperationStartCallback(std::function<void()> callback)
    {
        m_operationStartCallback = callback;
    }

    /** @brief Set callback for operation end (zoom/pan).
     * 
     * Called when a long-running operation (zoom or pan) ends.
     * Useful for restoring normal cursor.
     * 
     * @param callback Function to call when operation ends */
    void SetOperationEndCallback(std::function<void()> callback)
    {
        m_operationEndCallback = callback;
    }

private:
    /** @brief Perform zoom towards cursor.
     * 
     * @param zoomFactor Factor to zoom by (< 1.0 for zoom in, > 1.0 for zoom out) */
    void ZoomTowardsCursor(double zoomFactor);

    /** @brief Perform panning based on mouse movement.
     * 
     * Moves the focal point based on the delta between last and current mouse position. */
    void PanCamera();

    /// @brief Last mouse position for drag calculation
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;

    /// @brief Flag indicating if panning is active
    bool m_isPanning = false;

    /// @brief Callback for operation start (zoom/pan)
    std::function<void()> m_operationStartCallback;

    /// @brief Callback for operation end (zoom/pan)
    std::function<void()> m_operationEndCallback;
};
