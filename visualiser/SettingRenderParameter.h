/** @file SettingRenderParameter.h
 * @brief Declaration of the SettingRenderParameter class for managing VTK rendering settings. */

#pragma once

#include <vtkNew.h>
#include <vtkRenderer.h>

/** @class SettingRenderParameter
 * @brief Manages VTK rendering parameters and resources.
 * 
 * This class provides a simple wrapper around VTK's renderer component.
 * It's designed to centralize renderer management in the application.
 * 
 * @note The necessity of this class should be reviewed as it currently
 * provides minimal functionality beyond direct VTK renderer access. */
class SettingRenderParameter
{
public:
    /// @brief VTK renderer instance for managing 2D and 3D scene rendering
    vtkNew<vtkRenderer> m_renderer;
};
