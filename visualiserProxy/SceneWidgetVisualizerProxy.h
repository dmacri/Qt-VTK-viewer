#pragma once

#include "visualiser/Visualizer.hpp"

template<typename Cell>
struct SceneWidgetVisualizerTemplate
{
    Visualizer<Cell> vis;

    Cell** p;

    static Cell** getAllocatedParametersMatrix(int dimX, int dimY)
    {
        Cell** p = new Cell*[dimY];
        for (int i = 0; i < dimY; i++) {
            p[i] = new Cell[dimX];
        }
        return p;
    }
};
