/** @file Visualizer.hpp
 * @brief Declaration of the Visualizer class for VTK-based visualization. */

#pragma once

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include "utilities/types.h"    // StepIndex

class Line;

/** @class Visualizer
 * @brief Handles VTK-based visualization of simulation data.
 * 
 * This class provides methods to render and update 2D visualizations
 * of simulation data using the VTK library. It supports drawing grids,
 * color mapping, and text annotations. */
class Visualizer
{
public:
    template<class Matrix>
    void drawWithVTK(const Matrix& p, int nRows, int nCols, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor);
    template<class Matrix>
    void refreshWindowsVTK(const Matrix& p, int nRows, int nCols, vtkSmartPointer<vtkActor> gridActor);
    template<class Matrix>
    void drawWithVTK3DSubstate(const Matrix& p, int nRows, int nCols, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor, const std::string& substateFieldName, double minValue, double maxValue);
    template<class Matrix>
    void refreshWindowsVTK3DSubstate(const Matrix& p, int nRows, int nCols, vtkSmartPointer<vtkActor> gridActor, const std::string& substateFieldName, double minValue, double maxValue);
    void buildLoadBalanceLine(const std::vector<Line>& lines, int nRows, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor2D> actorBuildLine);
    void refreshBuildLoadBalanceLine(const std::vector<Line> &lines, int nRows, vtkActor2D* lineActor);
    vtkTextProperty* buildStepLine(StepIndex step, vtkSmartPointer<vtkTextMapper> singleLineTextB);
    vtkNew<vtkActor2D> buildStepText(StepIndex step, int font_size, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer);

private:
    template<class Matrix>
    void buidColor(vtkLookupTable* lut, int nCols, int nRows, const Matrix& p);

    /** @brief Build 3D points and values for 3D substate visualization.
     * 
     * This helper method extracts substate values from cells and builds points with
     * height-mapped Z coordinates. Used by both drawWithVTK3DSubstate and refreshWindowsVTK3DSubstate
     * to avoid code duplication.
     * 
     * @param p Matrix of cells containing substate data
     * @param nRows Number of grid rows
     * @param nCols Number of grid columns
     * @param substateFieldName Name of the substate field to extract
     * @param minValue Minimum value for normalization
     * @param maxValue Maximum value for normalization
     * @param heightScale Scale factor for Z height
     * @param points Output vtkPoints to populate
     * @param pointValues Output vtkDoubleArray with normalized values for color mapping */
    template<class Matrix>
    void build3DSubstatePoints(const Matrix& p, int nRows, int nCols, const std::string& substateFieldName,
                               double minValue, double maxValue, double heightScale,
                               vtkPoints* points, vtkDoubleArray* pointValues);

    /** @brief Creates a vtkPolyData representing a set of 2D lines.
      * @param lines Vector of Line objects (each defines a line segment)
      * @param nRows Number of grid rows (used to invert Y coordinates)
      * @return vtkSmartPointer<vtkPolyData> with points and lines set */
    vtkSmartPointer<vtkPolyData> createLinePolyData(const std::vector<Line>& lines, int nRows);
};

////////////////////////////////////////////////////////////////////

template <class Matrix>
void Visualizer::drawWithVTK(const Matrix &p, int nRows, int nCols, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor)
{
    const auto numberOfPoints = nRows * nCols;
    vtkNew<vtkDoubleArray> pointValues;
    pointValues->SetNumberOfTuples(numberOfPoints);
    for (int i = 0; i < numberOfPoints; ++i)
    {
        pointValues->SetValue(i, i);
    }

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(numberOfPoints);

    vtkNew<vtkPoints> points;
    for (int row = 0; row < nRows; row++)
    {
        for (int col = 0; col < nCols; col++)
        {
            points->InsertNextPoint(/*x=*/col, /*y=*/row, /*z=*/1); /// z is not used
        }
    }

    vtkNew<vtkStructuredGrid> structuredGrid;
    structuredGrid->SetDimensions(nCols, nRows, 1);
    structuredGrid->SetPoints(points);
    structuredGrid->GetPointData()->SetScalars(pointValues);

    buidColor(lut, nCols, nRows, p);

    vtkNew<vtkDataSetMapper> gridMapper;
    gridMapper->UpdateDataObject();
    gridMapper->SetInputData(structuredGrid);
    gridMapper->SetLookupTable(lut);
    gridMapper->SetScalarRange(0, numberOfPoints - 1);

    gridActor->SetMapper(gridMapper);
    renderer->AddActor(gridActor);
}

template<class Matrix>
void Visualizer::refreshWindowsVTK(const Matrix &p, int nRows, int nCols, vtkSmartPointer<vtkActor> gridActor)
{
    if (vtkLookupTable* lut = dynamic_cast<vtkLookupTable*>(gridActor->GetMapper()->GetLookupTable()))
    {
        buidColor(lut, nCols, nRows, p);
        gridActor->GetMapper()->SetLookupTable(lut);
        gridActor->GetMapper()->Update();
    }
    else
        throw std::runtime_error("Invalid dynamic cast!");
}

/** @brief Converts a color channel value to a normalized range [0, 1].
 *
 * This function is designed to be forward-compatible with upcoming changes in OOpenCal.
 * Currently, OOpenCal provides color values in the 0–255 integer range. VTK, however,
 * expects normalized double precision values in the 0–1 range.
 *
 * If the input value is greater than 1, it is assumed to be in the 0–255 range
 * and will be scaled down to [0, 1]. If the value is already in the [0, 1] range
 * (future OOpenCal format), it will be returned unchanged.
 *
 * @param channel The input color component (either in 0–255 or already in 0–1).
 * @return Normalized color value in the range [0, 1]. */
inline double toUnitColor(double channel)
{
    // Backward compatibility: if value is > 1, assume 0–255 format and scale.
    if (channel > 1.0)
        return channel / 255.0;

    // Already normalized (future format) — return as-is.
    return channel;
}

template<class Matrix>
void Visualizer::buidColor(vtkLookupTable* lut, int nCols, int nRows, const Matrix &p)
{
    for (int r = 0; r < nRows; ++r)
    {
        for (int c = 0; c < nCols; ++c)
        {
            const auto color = p[r][c].outputValue(nullptr);
            lut->SetTableValue(
                (nRows - 1 - r) * nCols + c,
                toUnitColor(color.getRed()),
                toUnitColor(color.getGreen()),
                toUnitColor(color.getBlue()),
                1.0 // alpha channel – keep as 1.0 for full opacity (optional scaling)
            );
        }
    }
}

////////////////////////////////////////////////////////////////////
// 3D Substate Visualization Methods

template <class Matrix>
void Visualizer::drawWithVTK3DSubstate(const Matrix &p, int nRows, int nCols, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor, const std::string& substateFieldName, double minValue, double maxValue)
{
    const auto numberOfPoints = nRows * nCols;
    vtkNew<vtkDoubleArray> pointValues;
    pointValues->SetNumberOfTuples(numberOfPoints);
    
    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(numberOfPoints);

    vtkNew<vtkPoints> points;
    
    // Validate min/max values
    if (std::isnan(minValue) || std::isnan(maxValue) || minValue >= maxValue)
    {
        // Fallback to regular 2D visualization if invalid values
        drawWithVTK(p, nRows, nCols, renderer, gridActor);
        return;
    }

    // Calculate height scale factor based on grid size
    // Scale to approximately 1/3 of the grid size for good visibility
    double heightScale = std::max(nRows, nCols) / 3.0;

    // Build 3D points with height based on substate value using helper method
    build3DSubstatePoints(p, nRows, nCols, substateFieldName, minValue, maxValue, heightScale, points, pointValues);

    vtkNew<vtkStructuredGrid> structuredGrid;
    structuredGrid->SetDimensions(nCols, nRows, 1);
    structuredGrid->SetPoints(points);
    structuredGrid->GetPointData()->SetScalars(pointValues);

    // Build color lookup table based on normalized values
    // Use a gradient from yellow (low values) to white (high values) for better visibility
    lut->SetRange(0.0, 1.0);
    for (int i = 0; i < numberOfPoints; ++i)
    {
        double value = pointValues->GetValue(i);
        // Yellow to white gradient: low values = yellow, high values = white
        double red = 1.0;                    // Always full red
        double green = 1.0;                  // Full green (yellow at low values)
        double blue = value;                 // Blue increases from 0 to 1 (white at high values)
        lut->SetTableValue(i, red, green, blue, 1.0);
    }

    vtkNew<vtkDataSetMapper> gridMapper;
    gridMapper->UpdateDataObject();
    gridMapper->SetInputData(structuredGrid);
    gridMapper->SetLookupTable(lut);
    gridMapper->SetScalarRange(0.0, 1.0);

    gridActor->SetMapper(gridMapper);
    renderer->AddActor(gridActor);
}

template<class Matrix>
void Visualizer::refreshWindowsVTK3DSubstate(const Matrix &p, int nRows, int nCols, vtkSmartPointer<vtkActor> gridActor, const std::string& substateFieldName, double minValue, double maxValue)
{
    if (vtkLookupTable* lut = dynamic_cast<vtkLookupTable*>(gridActor->GetMapper()->GetLookupTable()))
    {
        // Validate min/max values
        if (std::isnan(minValue) || std::isnan(maxValue) || minValue >= maxValue)
            return;

        double valueRange = maxValue - minValue;
        if (valueRange < 1e-10)
            valueRange = 1.0;

        // Calculate height scale factor based on grid size
        // Scale to approximately 1/3 of the grid size for good visibility
        double heightScale = std::max(nRows, nCols) / 3.0;

        const auto numberOfPoints = nRows * nCols;
        vtkNew<vtkDoubleArray> pointValues;
        pointValues->SetNumberOfTuples(numberOfPoints);

        vtkNew<vtkPoints> points;

        // Rebuild points with new substate values using helper method
        build3DSubstatePoints(p, nRows, nCols, substateFieldName, minValue, maxValue, heightScale, points, pointValues);

        // Update the structured grid
        vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(gridActor->GetMapper()->GetInput());
        if (grid)
        {
            grid->SetPoints(points);
            grid->GetPointData()->SetScalars(pointValues);
        }

        // Update lookup table colors
        // Use a gradient from yellow (low values) to white (high values) for better visibility
        lut->SetRange(0.0, 1.0);
        for (int i = 0; i < numberOfPoints; ++i)
        {
            double value = pointValues->GetValue(i);
            // Yellow to white gradient: low values = yellow, high values = white
            double red = 1.0;                    // Always full red
            double green = 1.0;                  // Full green (yellow at low values)
            double blue = value;                 // Blue increases from 0 to 1 (white at high values)
            lut->SetTableValue(i, red, green, blue, 1.0);
        }

        gridActor->GetMapper()->SetLookupTable(lut);
        gridActor->GetMapper()->Update();
    }
}

template<class Matrix>
void Visualizer::build3DSubstatePoints(const Matrix& p, int nRows, int nCols, const std::string& substateFieldName,
                                       double minValue, double maxValue, double heightScale,
                                       vtkPoints* points, vtkDoubleArray* pointValues)
{
    double valueRange = maxValue - minValue;
    if (valueRange < 1e-10)
        valueRange = 1.0;

    // Build 3D points with height based on substate value
    for (int row = 0; row < nRows; row++)
    {
        for (int col = 0; col < nCols; col++)
        {
            // Get the substate value for this cell
            std::string cellValueStr = p[row][col].stringEncoding(substateFieldName.c_str());
            double cellValue = 0.0;
            
            try
            {
                cellValue = std::stod(cellValueStr);
            }
            catch (const std::exception& e)
            {
                cellValue = minValue; // Default to min if parsing fails
                cout << "\t! Conversion error:" << cellValue << ", " << e.what() << '\n';
            }

            cellValue = std::clamp(cellValue, minValue, maxValue);

            // Calculate normalized height (0.0 to 1.0) then scale to grid size
            double normalizedHeight = (cellValue - minValue) / valueRange;
            double scaledHeight = normalizedHeight * heightScale;

            // Insert point with Z coordinate as height
            points->InsertNextPoint(col, row, scaledHeight);

            // Store the normalized value for color mapping
            pointValues->SetValue((nRows - 1 - row) * nCols + col, normalizedHeight);
        }
    }
}
