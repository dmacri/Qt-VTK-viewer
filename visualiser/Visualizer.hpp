#pragma once

#include <string>
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

#include "OOpenCAL/base/Element.h" // rgb
#include "types.h" // StepIndex

class Line;

class Visualizer
{
public:
    template<class Matrix>
    void drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, StepIndex step, Line *lines, vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor);
    template<class Matrix>
    void refreshWindowsVTK(/*const*/ Matrix& p, int nRows, int nCols, StepIndex step, Line *lines, int dimLines,  vtkSmartPointer<vtkActor> gridActor);
    void buildLoadBalanceLine(Line *lines, int dimLines, int nCols, int nRows, vtkSmartPointer<vtkPoints> pts, vtkSmartPointer<vtkCellArray> cellLines, vtkSmartPointer<vtkPolyData> grid, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor2D> actorBuildLine);
    void refreshBuildLoadBalanceLine(Line *lines, int dimLines, int nCols, int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors);
    vtkTextProperty* buildStepLine(StepIndex step, vtkSmartPointer<vtkTextMapper>, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkNamedColors> colors, std::string color);
    vtkNew<vtkActor2D> buildStepText(StepIndex step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer);

private:
    template<class Matrix>
    void buidColor(vtkLookupTable* lut, int nCols, int nRows, Matrix& p);
};

////////////////////////////////////////////////////////////////////

template <class Matrix>
void Visualizer::drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, StepIndex step, Line *lines, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor)
{
    const auto numberOfPoints = nRows * nCols;
    vtkNew<vtkDoubleArray> pointValues;
    pointValues->SetNumberOfTuples(numberOfPoints);
    for (size_t i = 0; i < numberOfPoints; ++i)
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
            points->InsertNextPoint(col, row, 1);
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

template <class Matrix>
void Visualizer::refreshWindowsVTK(/*const*/ Matrix& p, int nRows, int nCols, StepIndex step, Line *lines, int dimLines, vtkSmartPointer<vtkActor> gridActor)
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

template <class Matrix>
void Visualizer::buidColor(vtkLookupTable* lut, int nCols, int nRows, Matrix& p)
{
    for (int r = 0; r < nRows; ++r)
    {
        for (int c = 0; c < nCols; ++c)
        {
            rgb *color = p[r][c].outputValue();
            lut->SetTableValue((nRows-1-r)*nCols+c, (double)color->getRed(), (double)color->getGreen(), (double)color->getBlue());
        }
    }
}
