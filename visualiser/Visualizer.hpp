#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <format>
#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCoordinate.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLine.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
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
public:
    std::vector<std::unordered_map<int, long int>> hashMap;

    Visualizer() = default;

    long int stepStartingPositionInFile(int step, int node)
    {
        return hashMap[node][step];
    }

    std::string giveMeFileName(const std::string& fileName, int node) const
    {
        return std::format("{}{}.txt", fileName, node);
    }
    std::string giveMeFileNameIndex(const std::string &fileName, int node) const
    {
        return std::format("{}{}_index.txt", fileName, node);
    }

    FILE *giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node, int &nLocalCols, int &nLocalRows);
    std::pair<int,int> giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node);
    template<class Matrix>
    void getElementMatrix(int step, Matrix& m, int nGlobalCols, int nGlobalRows, int nNodeX, int nNodeY, const std::string& fileName, Line *lines);
    template<class Matrix>
    void drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, int dimLines, std::string edittext,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor);
    template<class Matrix>
    void refreshWindowsVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, int dimLines,  vtkSmartPointer<vtkActor> gridActor);
    // void readConfigurationFile(const char *filename, int infoFromFile[8], char *outputFileName);
    void loadHashmapFromFile(int nNodeX, int nNodeY, const std::string &filename);
    void buildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows,vtkSmartPointer<vtkPoints> pts,vtkSmartPointer<vtkCellArray> cellLines,vtkSmartPointer<vtkPolyData> grid,vtkSmartPointer<vtkNamedColors> colors,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor2D> actorBuildLine);
    void refreshBuildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors);
    vtkTextProperty* buildStepLine(int step,vtkSmartPointer<vtkTextMapper> ,vtkSmartPointer<vtkTextProperty> singleLineTextProp,vtkSmartPointer<vtkNamedColors> colors, std::string color);
    vtkNew<vtkActor2D> buildStepText(int step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer);

private:
    bool allNodesHaveEmptyData(const std::vector<int>& AlllocalCols, const std::vector<int>& AlllocalRows, int nodesCount);

    static std::pair<int,int> getColumnAndRowFromLine(const std::string& line);
};
////////////////////////////////////////////////////////////////////

template <class T>
FILE* Visualizer<T>::giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node, int &nLocalCols, int &nLocalRows)
{
    const auto fileNameTmp = giveMeFileName(fileName, node);

    FILE *fp = fopen(fileNameTmp.c_str(), "r");
    if (nullptr == fp)
    {
        throw std::runtime_error(std::format("Can't read '{}' in {} function", fileNameTmp, __FUNCTION__));
    }

    const auto fPos = stepStartingPositionInFile(step, node);
    fseek(fp, fPos, SEEK_SET);
    //printMatrixFromStepByUser(hashmap, stepUser);
   // generalPorpouseGetline(&line, &len, fp);

    char *line = {};
    size_t len{};
    getline(&line, &len, fp);

    std::tie(nLocalCols, nLocalRows) = getColumnAndRowFromLine(line);

    return fp;
}
template <class T>
std::pair<int,int> Visualizer<T>::getColumnAndRowFromLine(const std::string& line)
{
    /// input format: "C-R" where C and R are numbers
    if (line.empty())
    {
        throw std::invalid_argument("Line is empty, but it should dontain columns and row!");
    }

    const auto delimiterPos = line.find('-');
    if (delimiterPos == std::string::npos)
    {
        throw std::runtime_error("No delimiter '-' found in the line: >" + line + "<");
    }

    auto nLocalCols = std::stoi(line.substr(0, delimiterPos));
    auto nLocalRows = std::stoi(line.substr(delimiterPos + 1));

    return {nLocalCols, nLocalRows};
}

template <class T>
std::pair<int,int> Visualizer<T>::giveMeLocalColAndRowFromStep(int step, const std::string& fileName, int node)
{
    const auto fileNameTmp = giveMeFileName(fileName, node);

    std::ifstream file(fileNameTmp);
    if (! file.is_open())
    {
        throw std::runtime_error(std::format("Can't read '{}' in {} function", fileNameTmp, __func__));
    }

    const auto fPos = stepStartingPositionInFile(step, node);
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

    return getColumnAndRowFromLine(line);
}

template <class T>
bool Visualizer<T>::allNodesHaveEmptyData(const std::vector<int>& AlllocalCols, const std::vector<int>& AlllocalRows, int nodesCount)
{
    for (int node = 0; node < nodesCount; ++node)
    {
        if (AlllocalCols[node] > 0 || AlllocalRows[node] > 0)
        {
            return false;
        }
    }
    return true;
}

template<class T>
template<class Matrix>
void Visualizer<T>::getElementMatrix(int step, Matrix& m, int nGlobalCols, int nGlobalRows, int nNodeX, int nNodeY, const std::string& fileName, Line *lines)
{
    std::vector<int> AlllocalCols, AlllocalRows;
    AlllocalCols.resize(nNodeX * nNodeY);
    AlllocalRows.resize(nNodeX * nNodeY);
    
    // Check if we need to use fallback step
    int actualStep = step;

    // m = new T*[nGlobalRows];
    // for(int i = 0;i < nGlobalRows; i++){
    //     m[i]= new T[nGlobalCols];
    // }

    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        auto [nLocalCols, nLocalRows] = giveMeLocalColAndRowFromStep(actualStep, fileName, node);
        
        AlllocalCols[node] = nLocalCols;
        AlllocalRows[node] = nLocalRows;
    }
    
    const bool allEmpty = allNodesHaveEmptyData(AlllocalCols, AlllocalRows, nNodeX * nNodeY);

    
    // If all nodes are empty and this is step 4000, use previous step
    if (allEmpty && step == 4000)
    {
        actualStep = 3999;
        
        // Reload data with fallback step
        for (int node = 0; node < (nNodeX * nNodeY); node++)
        {
            const auto [nLocalCols, nLocalRows] = giveMeLocalColAndRowFromStep(actualStep, fileName, node);

            AlllocalCols[node] = nLocalCols;
            AlllocalRows[node] = nLocalRows;
        }
    }

    bool startStepDone = false;
    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        int nLocalCols, nLocalRows;

        FILE *fp = giveMeLocalColAndRowFromStep(actualStep, fileName, node, nLocalCols, nLocalRows);

        // if (step == 4)
        //     cout << "node " << node << " cols = " << nLocalCols << " rows= " << nLocalRows << endl;

        int offsetX = 0; //= //(node % nNodeX)*nLocalCols;//-this->borderSizeX;
        int offsetY = 0; //= //(node / nNodeX)*nLocalRows;//-this->borderSizeY;

        if (nNodeY == 1)
        {
            for (int k = 0; k < node % nNodeX; k++)
            {
                offsetX += AlllocalCols[k];
            }
        }
        else
        {
            for (int k = (node / nNodeX) * nNodeX; k < node; k++)
            {
                offsetX += AlllocalCols[k];
            }
        }

        if (node >= nNodeX)
        {
            for (int k = node - nNodeX; k >= 0;)
            {
                offsetY += AlllocalRows[k];
                k -= nNodeX;
            }
        }

        lines[node * 2] = Line(offsetX, offsetY, offsetX + nLocalCols, offsetY);
        lines[node * 2 + 1] = Line(offsetX, offsetY, offsetX, offsetY + nLocalRows);

        for (int row = 0; row < nLocalRows; row++)
        {
            char *line = NULL;
            size_t len = 0;

           // generalPorpouseGetline(&line, &len, fp);
            getline(&line, &len, fp);

            //m[row]=new T[nLocalCols];
            for (int col = 0; col < nLocalCols; ++col)
            {
                char *elem;
                if (col == 0)
                    elem = strtok(line, " ");
                else
                    elem = strtok(NULL, " ");

                if (!startStepDone)
                {
                    m[row + offsetY][col + offsetX].T::startStep(step);
                    startStepDone = true;
                }

                m[row + offsetY][col + offsetX].T::composeElement(elem);
                //rgb* color= m[row+offsetY][col+offsetX].outputValue();
                //   cout << color->getRed()<<" " << color->getGreen()<<" " <<color->getBlue() << endl;
            }
        }

        fclose(fp);
    }
}



// template <class T>
// void Visualizer<T>::readConfigurationFile(const char *filename, int infoFromFile[8], char *outputFileName)
// {
//     char str[999];
//     int n = 0;
//     FILE *file = fopen(filename, "r");

//     if (file)
//     {
//         int i = 1;
//         while (fscanf(file, "%s", str) != EOF && i <= 16)
//         {
//             if (i % 2 == 0)
//                 infoFromFile[n++] = atoi(str);
//             ++i;
//         }

//         int x = fscanf(file, "%s", str);
//         int y = fscanf(file, "%s", str);
//         strcpy(outputFileName, str);

//         fclose(file);
//     }
// }

template <class T>
void Visualizer<T>::loadHashmapFromFile(int nNodeX, int nNodeY, const std::string& filename)
{
    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        auto fileNameIndex = giveMeFileNameIndex(filename, node);
        cout << fileNameIndex << endl;
        FILE *fp = fopen(fileNameIndex.c_str(), "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);
        int step = 0;
        long int nbytes = 0;
        while (fscanf(fp, "%d %ld\n", &step, &nbytes) != EOF)
        {
            std::pair<int, long int> p(step, nbytes);
            hashMap[node].insert(p);
        }
        fclose(fp);
    }
}

template <class T>
template <class Matrix>
void Visualizer<T>::drawWithVTK(/*const*/ Matrix& p, int nRows, int nCols, int step, Line *lines, int dimLines, std::string edittext,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor)
{
    vtkNew<vtkStructuredGrid> structuredGrid;
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPoints> points;
    vtkNew<vtkLookupTable> lut;

    auto numberOfPoints = nRows * nCols;
    vtkNew<vtkDoubleArray> pointValues;
    pointValues->SetNumberOfTuples(numberOfPoints);
    for (size_t i = 0; i < numberOfPoints; ++i)
    {
        pointValues->SetValue(i, i);
    }

    lut->SetNumberOfTableValues(numberOfPoints);
   // lut->Build();
    // Assign some specific colors in this case


    for (int row = 0; row < nRows; row++)
    {
        for (int col = 0; col < nCols; col++)
        {
            points->InsertNextPoint(col, row, 1);
        }
    }

    structuredGrid->SetDimensions(nCols, nRows, 1);
    structuredGrid->SetPoints(points);
    structuredGrid->GetPointData()->SetScalars(pointValues);

    buidColor(lut,nCols,nRows,p);

    vtkNew<vtkDataSetMapper> gridMapper;
    gridMapper->UpdateDataObject();
    gridMapper->SetInputData(structuredGrid);
    gridMapper->SetLookupTable(lut);
    gridMapper->SetScalarRange(0, numberOfPoints - 1);
    // gridMapper->ScalarVisibilityOn();

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
void Visualizer<T>::buildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows,vtkSmartPointer<vtkPoints> pts,vtkSmartPointer<vtkCellArray> cellLines,vtkSmartPointer<vtkPolyData> grid,vtkSmartPointer<vtkNamedColors> colors,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor2D> actorBuildLine)
{
    for (int i = 0; i < dimLines; i++)
    {
        cout << lines[i].x1 << "  " << lines[i].y1 << "  " <<lines[i].x2 << "  " <<lines[i].y2 << endl;
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
            rgb *color=p[r][c].outputValue();
           // lut->SetTableValue(r*nCols+c,(double)color->getRed(),(double)color->getGreen(),(double)color->getBlue());
            lut->SetTableValue((nRows-1-r)*nCols+c,(double)color->getRed(),(double)color->getGreen(),(double)color->getBlue());
        }
    }
}
