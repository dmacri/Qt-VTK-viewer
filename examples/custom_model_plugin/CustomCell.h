#pragma once

#include <algorithm> // std::max, std::min
#include <cstdlib>

#include "OOpenCAL/base/Cell.h"

/** CustomCell - Example custom model for Qt-VTK-viewer plugin system
 * 
 * This is a simple example showing how to create a custom cell model
 * that can be loaded as a plugin without recompiling the main application.
 * 
 * This model stores a single integer value and displays it with a color
 * gradient from blue (low values) to red (high values). */
class CustomCell : public Cell
{
    int value;

public:
    OPENCALF CustomCell()
        : value(0)
    {
    }

    OPENCALF CustomCell(int val)
        : value(val)
    {
    }

    OPENCALF void setValue(int v)
    {
        value = v;
    }

    OPENCALF int getValue() const
    {
        return value;
    }

    /// Parse a string representation and set the cell value
    void composeElement(char* str) override
    {
        this->value = atoi(str);
    }

    /// Convert cell state to string representation
    std::string stringEncoding(const char*) const override
    {
        return std::to_string(value);
    }

    /** Determine the output color based on the cell value
     * Blue (0) -> Cyan -> Green -> Yellow -> Red (255) */
    Color outputValue(const char* str) const override
    {
        Color outputColor{128, 128, 128};

        // Normalize value to 0-1 range (assuming 0-255 input)
        double normalized = value / 255.0;
        normalized = std::max(0.0, std::min(1.0, normalized));

        if (normalized < 0.25)
        {
            // Blue -> Cyan
            double t = normalized * 4.0;
            outputColor = Color(0, static_cast<int>(255 * t), 255);
        }
        else if (normalized < 0.5)
        {
            // Cyan -> Green
            double t = (normalized - 0.25) * 4.0;
            outputColor = Color(0, 255, static_cast<int>(255 * (1 - t)));
        }
        else if (normalized < 0.75)
        {
            // Green -> Yellow
            double t = (normalized - 0.5) * 4.0;
            outputColor = Color(static_cast<int>(255 * t), 255, 0);
        }
        else
        {
            // Yellow -> Red
            double t = (normalized - 0.75) * 4.0;
            outputColor = Color(255, static_cast<int>(255 * (1 - t)), 0);
        }

        return outputColor;
    }

    /// Called at the start of each simulation step
    void startStep(int /*step*/) override
    {
        // Custom initialization for each step if needed
    }
};
