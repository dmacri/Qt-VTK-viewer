/** @file SceneWidgetVisualizerProxy.h
 * @brief Template class for managing visualization of cell-based models.
 *
 * This file contains the SceneWidgetVisualizerTemplate class which provides
 * a bridge between the visualization system and cell-based models. It combines
 * a Visualizer instance with a ModelReader to handle the visualization of
 * grid-based cellular automata or similar models. */

#pragma once

#include "visualiser/Visualizer.hpp"
#include "ModelReader.hpp"

/** @class SceneWidgetVisualizerTemplate
 * @tparam Cell The cell type used in the model
 *
 * @brief Template class that manages visualization of a grid-based model.
 *
 * This class combines a Visualizer for rendering with a ModelReader to load
 * and manage cell data. It provides functionality to initialize and maintain
 * a 2D grid of cells that can be visualized.
 *
 * @note The template parameter Cell should be a type that represents a single
 *       cell in the model and should be compatible with the ModelReader. */
template<typename Cell>
struct SceneWidgetVisualizerTemplate
{
    Visualizer visualiser;                ///< The visualizer instance for rendering the model
    ModelReader<Cell> modelReader;        ///< The reader for loading and managing model data
    std::vector<std::vector<Cell>> p;     ///< 2D vector storing the cell data

    /** @brief Initializes the internal matrix with the specified dimensions.
     *
     * This method resizes the internal 2D vector to match the given dimensions,
     * creating a grid of default-constructed Cell objects.
     *
     * @param dimX The width of the grid (number of columns)
     * @param dimY The height of the grid (number of rows)
     *
     * @note The dimensions must be positive integers. The method will create a grid with dimY rows and dimX columns. */
    void initMatrix(int dimX, int dimY)
    {
        p.resize(dimY);
        for (int i = 0; i < dimY; i++)
        {
            p[i].resize(dimX);
        }
    }
};
