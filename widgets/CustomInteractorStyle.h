/** @file CustomInteractorStyle.h
 * @brief Custom VTK interactor style for cursor-based zoom.
 * 
 * This interactor style extends vtkInteractorStyleImage to provide
 * zoom functionality that keeps the point under the cursor stationary.
 */

#ifndef CUSTOM_INTERACTOR_STYLE_H
#define CUSTOM_INTERACTOR_STYLE_H

#include <vtkInteractorStyleImage.h>

/** @brief Custom interactor style for cursor-based zoom.
 * 
 * Extends vtkInteractorStyleImage to override mouse wheel events
 * and zoom towards the cursor position instead of the screen center.
 */
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

private:
    /** @brief Perform zoom towards cursor.
     * 
     * @param zoomFactor Factor to zoom by (< 1.0 for zoom in, > 1.0 for zoom out) */
    void ZoomTowardsCursor(double zoomFactor);
};

#endif // CUSTOM_INTERACTOR_STYLE_H
