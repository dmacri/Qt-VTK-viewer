/** @file SettingParameter.h
 * @brief Declaration of the SettingParameter structure for visualization settings. */

#pragma once

#include <iosfwd>
#include <string>
#include "types.h"

/** @struct SettingParameter
 * @brief Contains parameters for visualization settings and simulation configuration.
 * 
 * This structure holds various parameters that control the visualization and
 * behavior of the simulation. */
struct SettingParameter
{
    StepIndex step;         ///< Current simulation step
    StepIndex nsteps;       ///< Total number of simulation steps
    int numberOfColumnX;    ///< Number of columns in the simulation grid
    int numberOfRowsY;      ///< Number of rows in the simulation grid
    int nNodeX;            ///< Number of nodes in X direction
    int nNodeY;            ///< Number of nodes in Y direction
    int numberOfLines;      ///< Total number of lines in the visualization
    std::string outputFileName; ///< Name of the output file
    
    static constexpr int font_size = 18; ///< Font size for text rendering
    
    bool changed;   ///< Flag indicating if settings have been modified and currently visible state should be redrown

    /// @brief Printing SettingParameter to output stream
    friend std::ostream& operator<<(std::ostream& os, const SettingParameter& sp);
};
