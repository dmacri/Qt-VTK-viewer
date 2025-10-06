#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <format>
#include <string_view>
#include <vector>
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

#include "Line.h"
#include "OOpenCAL/base/Element.h" // rgb

#ifndef __ssize_t_defined
using ssize_t = intptr_t;
#endif


template <class T>
class Visualizer
{
    std::vector<std::unordered_map<int, long int>> hashMap;

public:
    void prepareHashMap(int nNodeX, int nNodeY)
    {
        hashMap.resize(nNodeX * nNodeY);
    }

    long int getStepStartingPositionInFile(int step, int node)
    {
        return hashMap.at(node).at(step);
    }

    [[nodiscard]] std::ifstream giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node, int &nLocalCols, int &nLocalRows);
    [[nodiscard]] std::pair<int,int> giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node);
    template<class Matrix>
    void readStageStateFromFilesForStep(int step, Matrix& m, int nNodeX, int nNodeY, const std::string& fileName, Line *lines);
    template<class Matrix>
    void drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor);
    template<class Matrix>
    void refreshWindowsVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, int dimLines,  vtkSmartPointer<vtkActor> gridActor);

    /** @brief Loads data into hashMap from text files.
     *
     * Expected file format (each line):
     *     <stepNumber:int> <positionInFile:long>
     *
     * Example:
     *     0 37
     *     1 32504
     *     2 64971
     *
     * Each line must contain exactly two numbers separated by whitespace.
     *
     * @param nNodeX number of nodes along the X axis
     * @param nNodeY number of nodes along the Y axis
     * @param filename base filename (e.g., "ball"), for which nodes files are being read */
    void loadStepOffsetsPerNode(int nNodeX, int nNodeY, const std::string &filename);
    void buildLoadBalanceLine(Line *lines, int dimLines, int nCols, int nRows, vtkSmartPointer<vtkPoints> pts, vtkSmartPointer<vtkCellArray> cellLines, vtkSmartPointer<vtkPolyData> grid, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor2D> actorBuildLine);
    void refreshBuildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors);
    vtkTextProperty* buildStepLine(int step, vtkSmartPointer<vtkTextMapper>, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkNamedColors> colors, std::string color);
    vtkNew<vtkActor2D> buildStepText(int step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer);

private:
    std::pair<std::vector<int>, std::vector<int>> giveMeLocalColsAndRowsForAllSteps(int step, int nNodeX, int nNodeY, const std::string& fileName);
};

namespace VisualiserHelpers /// functions which are not templates
{
[[nodiscard]] inline std::string giveMeFileName(const std::string& fileName, int node)
{
    return std::format("{}{}.txt", fileName, node);
}
[[nodiscard]] inline std::string giveMeFileNameIndex(const std::string &fileName, int node)
{
    return std::format("{}{}_index.txt", fileName, node);
}

std::pair<int,int> getColumnAndRowFromLine(const std::string& line);

bool allNodesHaveEmptyData(const std::vector<int>& AlllocalCols, const std::vector<int>& AlllocalRows, int nodesCount);

std::pair<int,int> calculateXYOffset(int node, int nNodeX, int nNodeY, const std::vector<int>& allLocalCols, const std::vector<int>& allLocalRows);
}
////////////////////////////////////////////////////////////////////

template <class T>
std::pair<int,int> Visualizer<T>::giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node)
{
    int nLocalCols, nLocalRows;
    std::ifstream file = giveMeLocalColAndRowFromStep(step, fileName, node, nLocalCols, nLocalRows);
    return {nLocalCols, nLocalRows};
}
template <class T>
std::ifstream Visualizer<T>::giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node, int &nLocalCols, int &nLocalRows)
{
    const auto fileNameTmp = VisualiserHelpers::giveMeFileName(fileName, node);

    std::ifstream file(fileNameTmp);
    if (! file.is_open())
    {
        throw std::runtime_error(std::format("Can't read '{}' in {} function", fileNameTmp, __func__));
    }

    const auto fPos = getStepStartingPositionInFile(step, node);
    file.seekg(fPos);
    if (! file)
    {
        throw std::runtime_error(std::format("Seek failed in '{}' at position {}", fileNameTmp, fPos));
    }

    std::string line;
    if (! std::getline(file, line))
    {
        throw std::runtime_error(std::format("Failed to read line from '{}' at position {}", fileNameTmp, fPos));
    }

    std::tie(nLocalCols, nLocalRows) = VisualiserHelpers::getColumnAndRowFromLine(line);
    return file;
}

template<class T>
template<class Matrix>
void Visualizer<T>::readStageStateFromFilesForStep(int step, Matrix& m, int nNodeX, int nNodeY, const std::string& fileName, Line *lines)
{
    // Check if we need to use fallback step
    int actualStep = step;

    auto [allLocalCols, allLocalRows] = giveMeLocalColsAndRowsForAllSteps(step, nNodeX, nNodeY, fileName);

    // If all nodes are empty and this is step 4000, use previous step
    if (step == 4000 &&  // TODO: GB: What is 4000? Is 4000 the last step?
        VisualiserHelpers::allNodesHaveEmptyData(allLocalCols, allLocalRows, nNodeX * nNodeY))
    {
        actualStep = 3999;
        // Reload data with fallback step
        std::tie(allLocalCols, allLocalRows) = giveMeLocalColsAndRowsForAllSteps(actualStep, nNodeX, nNodeY, fileName);
    }

    bool startStepDone = false;

    constexpr size_t numbersPerLine = 10'000;
    const size_t lineBufferSize = (log10(UINT_MAX) + 2) * numbersPerLine;
    std::string line;
    line.reserve(lineBufferSize); /// for speedup to avoid realocations

    for (int node = 0; node < nNodeX * nNodeY; ++node)
    {
        const auto [offsetX, offsetY] = VisualiserHelpers::calculateXYOffset(node, nNodeX, nNodeY, allLocalCols, allLocalRows);

        int nLocalCols, nLocalRows;
        std::ifstream fp = giveMeLocalColAndRowFromStep(actualStep, fileName, node, nLocalCols, nLocalRows);
        if (! fp)
            throw std::runtime_error("Cannot open file for node " + std::to_string(node));

        /// introducing big buffer for reading (for speed up)
        static thread_local char fileBuffer[1 << 16];
        fp.rdbuf()->pubsetbuf(fileBuffer, sizeof(fileBuffer));

        lines[node * 2]     = Line(offsetX, offsetY, offsetX + nLocalCols, offsetY);
        lines[node * 2 + 1] = Line(offsetX, offsetY, offsetX, offsetY + nLocalRows);

        for (int row = 0; row < nLocalRows; ++row)
        {
            if (! std::getline(fp, line))
            {
                const auto fileNameTmp = VisualiserHelpers::giveMeFileName(fileName, node);
                throw std::runtime_error("Error when reading entire line of file " + fileNameTmp);
            }

            std::replace(begin(line), end(line), ' ', '\0'); /// this is faster than using std::ranges::replace

            /// Moving through tokens (token are characters ended with '0')
            char* currentTokenPtr = line.data();
            for (int col = 0; col < nLocalCols && *currentTokenPtr; ++col)
            {
                if (! startStepDone) [[unlikely]]
                {
                    m[row + offsetY][col + offsetX].T::startStep(step);
                    startStepDone = true;
                }

                m[row + offsetY][col + offsetX].T::composeElement(currentTokenPtr);

                /// go to another token
                while (*currentTokenPtr)
                    ++currentTokenPtr;
                ++currentTokenPtr; /// skip '\0'
            }
        }
    }
}
template<class T>
std::pair<std::vector<int>, std::vector<int>> Visualizer<T>::giveMeLocalColsAndRowsForAllSteps(int step, int nNodeX, int nNodeY, const std::string& fileName)
{
    const auto nodesCount = nNodeX * nNodeY;
    std::vector<int> allLocalCols{nodesCount}, allLocalRows{nodesCount};
    allLocalCols.resize(nodesCount);
    allLocalRows.resize(nodesCount);

    for (int node = 0; node < nodesCount; node++)
    {
        auto [nLocalCols, nLocalRows] = giveMeLocalColAndRowFromStep(step, fileName, node);

        allLocalCols[node] = nLocalCols;
        allLocalRows[node] = nLocalRows;
    }
    return {allLocalCols, allLocalRows};
}

template <class T>
void Visualizer<T>::loadStepOffsetsPerNode(int nNodeX, int nNodeY, const std::string& filename)
{
    const int totalNodes = nNodeX * nNodeY;
    for (int node = 0; node < totalNodes; ++node)
    {
        const auto fileNameIndex = VisualiserHelpers::giveMeFileNameIndex(filename, node);
        std::cout << "Reading file: " << fileNameIndex << '\n';

        if (! std::filesystem::exists(fileNameIndex))
        {
            throw std::runtime_error("File not found: " + fileNameIndex);
        }

        std::ifstream file(fileNameIndex);
        if (! file)
        {
            throw std::runtime_error("Cannot open file: " + fileNameIndex);
        }

        int stepNumber;
        long positionInFile;

        while (true)
        {
            if (! (file >> stepNumber >> positionInFile))
            { /// enters here if reading error
                if (file.eof())
                    break;
                else
                    throw std::runtime_error("Invalid line format in file: " + fileNameIndex);
            }

            hashMap[node].emplace(stepNumber, positionInFile);
        }
    }
}

template <class T>
template <class Matrix>
void Visualizer<T>::drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor> gridActor)
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

template <class T>
template <class Matrix>
void Visualizer<T>::refreshWindowsVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, int dimLines, vtkSmartPointer<vtkActor> gridActor)
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

template <class T>
void Visualizer<T>::buildLoadBalanceLine(Line *lines, int dimLines,int nCols, int nRows, vtkSmartPointer<vtkPoints> pts, vtkSmartPointer<vtkCellArray> cellLines, vtkSmartPointer<vtkPolyData> grid, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor2D> actorBuildLine)
{
    for (int i = 0; i < dimLines; i++)
    {
        std::cout << "Line (" << lines[i].x1 << ", " << lines[i].y1 << ") -> (" <<lines[i].x2 << ", " <<lines[i].y2 << ")" << std::endl;
        pts->InsertNextPoint(lines[i].x1 * 1, nCols-1-lines[i].y1 * 1, 0.0);
        pts->InsertNextPoint(lines[i].x2 * 1, nCols-1-lines[i].y2 * 1, 0.0);
        cellLines->InsertNextCell(2);
        cellLines->InsertCellPoint(i*2);
        cellLines->InsertCellPoint(i*2+1);
    }

    grid->SetPoints(pts);
    grid->SetLines(cellLines);
    // Set up the coordinate system.
    vtkNew<vtkCoordinate> normCoords;
    normCoords->SetCoordinateSystemToWorld();

    vtkNew<vtkPolyDataMapper2D> mapper;
    mapper->SetInputData(grid);
    mapper->Update();
    mapper->SetTransformCoordinate(normCoords);

    actorBuildLine->SetMapper(mapper);
    actorBuildLine->GetMapper()->Update();
    actorBuildLine->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
   // actorBuildLine->GetProperty()->SetLineWidth(1.5);
    renderer->AddActor2D(actorBuildLine);
}

template <class T>
void Visualizer<T>::refreshBuildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors)
{
    vtkPolyDataMapper2D* mapper = (vtkPolyDataMapper2D*) lineActor->GetMapper();
    mapper->Update();
    vtkNew<vtkPolyData> grid;
    vtkNew<vtkPoints> pts;
    vtkNew<vtkCellArray> cellLines;

    for (int i = 0; i < dimLines; i++)
    {
        pts->InsertNextPoint((lines[i].x1 * 1), ( nCols-1-lines[i].y1 * 1), 0.0);
        pts->InsertNextPoint((lines[i].x2 * 1), ( nCols-1-lines[i].y2 * 1), 0.0);
        cellLines->InsertNextCell(2);
        cellLines->InsertCellPoint(i*2);
        cellLines->InsertCellPoint(i*2+1);
    }

    grid->SetPoints(pts);
    grid->SetLines(cellLines);
    // Set up the coordinate system.
    vtkNew<vtkCoordinate> normCoords;
    normCoords->SetCoordinateSystemToWorld();

    mapper->SetInputData(grid);
    mapper->Update();
    mapper->SetTransformCoordinate(normCoords);
    
    lineActor->SetMapper(mapper);
    lineActor->GetMapper()->Update();
}

template <class T>
vtkTextProperty* Visualizer<T>::buildStepLine(int step,vtkSmartPointer<vtkTextMapper> singleLineTextB,vtkSmartPointer<vtkTextProperty> singleLineTextProp,vtkSmartPointer<vtkNamedColors> colors, std::string color)
{
    std::string stepText = "Step " + std::to_string(step);
    singleLineTextB->SetInput(stepText.c_str());
    singleLineTextB->Update();
    vtkTextProperty* tprop = singleLineTextB->GetTextProperty();
    tprop->ShallowCopy(singleLineTextProp);
    tprop->SetVerticalJustificationToBottom();
    tprop->SetColor(colors->GetColor3d(color).GetData());
    return tprop;
}


template <class T>
vtkNew<vtkActor2D> Visualizer<T>::buildStepText(int step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer)
{
    singleLineTextProp->SetFontSize(font_size);
    singleLineTextProp->SetFontFamilyToArial();
    singleLineTextProp->BoldOn();
    singleLineTextProp->ItalicOff();
    singleLineTextProp->ShadowOff();

    const std::string stringWithStep = "Step " + std::to_string(step);
    stepLineTextMapper->SetInput(stringWithStep.c_str());
    auto tprop = stepLineTextMapper->GetTextProperty();
    tprop->ShallowCopy(singleLineTextProp);

    tprop->SetVerticalJustificationToBottom();
    tprop->SetColor(colors->GetColor3d("Red").GetData());

    vtkNew<vtkActor2D> stepLineTextActor;
    stepLineTextActor->SetMapper(stepLineTextMapper);
    stepLineTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    stepLineTextActor->GetPositionCoordinate()->SetValue(0.05, 0.85);
    renderer->AddActor2D(stepLineTextActor);
    return stepLineTextActor;
}

template <class Matrix>
void buidColor(vtkLookupTable* lut, int nCols, int nRows, Matrix& p)
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
