#pragma once

#include "visualiser/Visualizer.hpp"

template<typename Cell>
struct SceneWidgetVisualizerTemplate
{
    Visualizer<Cell> vis;

    std::vector<std::vector<Cell>> p;

    void initMatrix(int dimX, int dimY)
    {
        p.resize(dimY);
        for (int i = 0; i < dimY; i++)
        {
            p[i].resize(dimX);
        }
    }
};
