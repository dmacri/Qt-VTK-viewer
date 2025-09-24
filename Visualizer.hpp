#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPointSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkLine.h>
#include <vtkLookupTable.h>
#include <vtkStructuredGrid.h>
#include<vtkInteractorStyleImage.h>
#include <vtkProperty.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <stdio.h>
#include <stdlib.h>
#include <vtkDoubleArray.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <regex>
#include <climits>
#include <string>

#include "Visualizer.h"
#include "Element.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

// if typedef doesn't exist (msvc, blah)
typedef intptr_t ssize_t;







template <class T>
long int Visualizer<T>::gotoStep(int step, FILE *fp, int node)
{
    return hashMap[node][step];
}
template <class T>
void Visualizer<T>::stampa(long int fPos)
{
   }
template <class T>
char* Visualizer<T>::giveMeFileName(char *fileName, int node)
{
    char *fileNameTmp = new char[256];
    strcpy(fileNameTmp, fileName);
    strcat(fileNameTmp, std::to_string(node).c_str());
    strcat(fileNameTmp, ".txt");
    return fileNameTmp;
}
template <class T>
char* Visualizer<T>::giveMeFileNameIndex(char *fileName, int node)
{
    char *fileNameTmp = new char[256];
    strcpy(fileNameTmp, fileName);
    strcat(fileNameTmp, std::to_string(node).c_str());
    strcat(fileNameTmp, "_index.txt");
    return fileNameTmp;
}
template <class T>
FILE* Visualizer<T>::giveMeLocalColAndRowFromStep(int step, char *fileName, int node, int &nLocalCols, int &nLocalRows, char *&line, size_t &len)
{
    char *fileNameTmp = giveMeFileName(fileName, node);

    FILE *fp = fopen(fileNameTmp, "r");
    if (fp == NULL)
    {
        cout << "Can't read " << fileNameTmp << " in giveMeLocalColAndRowFromStep function" << endl;
        exit(EXIT_FAILURE);
    }

    long int fPos = gotoStep(step, fp, node);

    fseek(fp, fPos, SEEK_SET);
    //printMatrixFromStepByUser(hashmap, stepUser);
   // generalPorpouseGetline(&line, &len, fp);
    getline(&line, &len, fp);
    char *pch;
    pch = strtok(line, "-");
    nLocalCols = atoi(pch);
    pch = strtok(NULL, "-");
    nLocalRows = atoi(pch);
    return fp;
}

template <class T>
void Visualizer<T>::getElementMatrix(int step, T **&m, int nGlobalCols, int nGlobalRows, int nNodeX, int nNodeY, char *fileName, Line *lines)
{
    int *AlllocalCols = new int[(nNodeX * nNodeY)];
    int *AlllocalRows = new int[(nNodeX * nNodeY)];
    
    // Check if we need to use fallback step
    int actualStep = step;

    // m = new T*[nGlobalRows];
    // for(int i = 0;i < nGlobalRows; i++){
    //     m[i]= new T[nGlobalCols];
    // }

    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        int nLocalCols;
        int nLocalRows;
        char *line = NULL;
        size_t len = 0;

        FILE *fp = giveMeLocalColAndRowFromStep(actualStep, fileName, node, nLocalCols, nLocalRows, line, len);
        

        AlllocalCols[node] = nLocalCols;
        AlllocalRows[node] = nLocalRows;
        fclose(fp);
    }
    
    // Check if all nodes have empty data (step beyond simulation end)
    bool allEmpty = true;
    for (int node = 0; node < (nNodeX * nNodeY); node++) {
        if (AlllocalCols[node] > 0 || AlllocalRows[node] > 0) {
            allEmpty = false;
            break;
        }
    }
    
    // If all nodes are empty and this is step 4000, use previous step
    if (allEmpty && step == 4000) {
        actualStep = 3999;
        
        // Reload data with fallback step
        for (int node = 0; node < (nNodeX * nNodeY); node++)
        {
            int nLocalCols;
            int nLocalRows;
            char *line = NULL;
            size_t len = 0;

            FILE *fp = giveMeLocalColAndRowFromStep(actualStep, fileName, node, nLocalCols, nLocalRows, line, len);
            AlllocalCols[node] = nLocalCols;
            AlllocalRows[node] = nLocalRows;
            fclose(fp);
        }
    }

    bool startStepDone = false;
    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        int nLocalCols;
        int nLocalRows;
        char *line = NULL;
        size_t len = 0;

        FILE *fp = giveMeLocalColAndRowFromStep(actualStep, fileName, node, nLocalCols, nLocalRows, line, len);

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

        
        Line *lineTmp = new Line(offsetX, offsetY, offsetX + nLocalCols, offsetY);
        Line *lineTmp2 = new Line(offsetX, offsetY, offsetX, offsetY + nLocalRows);

        lines[node * 2] = *lineTmp;
        lines[node * 2 + 1] = *lineTmp2;

        // Debug output for line coordinates (commented for performance)
        // cout << "Node " << node << " lineTmp.x1= " << lineTmp->x1 << " lineTmp.y1 " << lineTmp->y1 << " lineTmp.x2= " << lineTmp->x2 << " lineTmp.y2 " << lineTmp->y2<< endl;

        int row = 0;

        while (row < nLocalRows)
        {
           // generalPorpouseGetline(&line, &len, fp);
            getline(&line, &len, fp);
            int col = 0;
            //m[row]=new T[nLocalCols];
            while (col < nLocalCols)
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

                col++;
            }
            row++;
        }

        fclose(fp);
    }

    delete[] AlllocalCols;
    delete[] AlllocalRows;
}



template <class T>
void Visualizer<T>::readConfigurationFile(const char *filename, int infoFromFile[8], char *outputFileName)
{
    char str[999];
    int n = 0;
    FILE *file;
    file = fopen(filename, "r");

    if (file)
    {
        int i = 1;
        while (fscanf(file, "%s", str) != EOF && i <= 16)
        {
            if (i % 2 == 0)
                infoFromFile[n++] = atoi(str);
            ++i;
        }

        int x = fscanf(file, "%s", str);
        int y = fscanf(file, "%s", str);
        strcpy(outputFileName, str);

        fclose(file);
    }
}

template <class T>
void Visualizer<T>::loadHashmapFromFile(int nNodeX, int nNodeY, char *filename)
{
    for (int node = 0; node < (nNodeX * nNodeY); node++)
    {
        char *fileNameIndex = giveMeFileNameIndex(filename, node);
        cout << fileNameIndex << endl;
        FILE *fp = fopen(fileNameIndex, "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);
        int step = 0;
        long int nbytes = 0;
        while (fscanf(fp, "%d %ld\n", &step, &nbytes) != EOF)
        {
            // cout << step << " " << nbytes << endl;
            std::pair<int, long int> p(step, nbytes);
            hashMap[node].insert(p);
        }
        // char* line = NULL;
        // size_t len = 0;
        // while( getline(&line, &len, fp)){

        //     char * pch;
        //     pch = strtok (line," ");
        //     int step =atoi(pch);
        //     pch = strtok (NULL, " ");
        //     long int nbytes =strtoll(pch, NULL, 10);//atoi(pch);

        //     cout << step << " " << nbytes << endl;
        //     std::pair<int, long int> p(step,nbytes);
        //     hashMap[node].insert(p);
        //     cout << fileNameIndex << endl;
        // }
        // cout << "finito" << endl;
        fclose(fp);
        //cout << "finito" << endl;
    }
}
template <class T>
size_t Visualizer<T>::generalPorpouseGetline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = (char*) malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr =(char*) realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

template <class T>
void Visualizer<T>::drawWithVTK(T **p, int nRows, int nCols, int step, Line *lines, int dimLines, string edittext,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor)
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
void Visualizer<T>::refreshWindowsVTK(T **p, int nRows, int nCols, int step, Line *lines, int dimLines, vtkSmartPointer<vtkActor> gridActor)
{
    vtkLookupTable* lut =(vtkLookupTable*)gridActor->GetMapper()->GetLookupTable();

    // dynamic_cast<vtkLookupTable*>(gridActor->GetMapper()->GetLookupTable());

    buidColor(lut,nCols,nRows,p);
    gridActor->GetMapper()->SetLookupTable(lut);
    gridActor->GetMapper()->Update();

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
void Visualizer<T>::refreshBuildStepText(int step,vtkActor2D* stepLineTextActor)
{
    vtkTextMapper* stepLineTextMapper = (vtkTextMapper*) stepLineTextActor->GetMapper();
    std::string stepText = "Step " + std::to_string(step);
    stepLineTextMapper->SetInput(stepText.c_str());
    stepLineTextMapper->Update();

}

template <class T>
vtkTextProperty* Visualizer<T>:: buildStepLine(int step,vtkSmartPointer<vtkTextMapper> singleLineTextB,vtkSmartPointer<vtkTextProperty> singleLineTextProp,vtkSmartPointer<vtkNamedColors> colors,string color)
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
    std::stringstream stringStream;
    stringStream << "Step " << step;
    std::string stringWithStep = stringStream.str();

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

template <class T>
void buidColor(vtkLookupTable* lut, int nCols, int nRows,T **p)
{
    for (int r = 0; r < nRows; ++r)
        for (int c = 0; c < nCols; ++c){
            rgb *color=p[r][c].outputValue();
           // lut->SetTableValue(r*nCols+c,(double)color->getRed(),(double)color->getGreen(),(double)color->getBlue());
                lut->SetTableValue((nRows-1-r)*nCols+c,(double)color->getRed(),(double)color->getGreen(),(double)color->getBlue());
        }

}
#endif // VISUALIZER_HPP

