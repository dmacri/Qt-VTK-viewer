#pragma once

#include <cstdlib>
#include <unordered_map>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkTextMapper.h>
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

struct Line;

namespace
{
void KeypressCallbackFunction(vtkObject* caller, long unsigned int eventId,
                              void* clientData, void* callData);
} // namespace


// extern int pixelsQuadrato; // Not needed for VTK
extern int *maxStepVisited;
extern std::unordered_map<int, long int> *hashMap;
extern Line *lines;


template <class T>
class Visualizer
{
public:
    T** p;

    Visualizer() = default;

    long int gotoStep(int step, FILE *fp, int node);
    void stampa(long int fPos);
    char *giveMeFileName(char *fileName, int node);
    char *giveMeFileNameIndex(char *fileName, int node);
    FILE *giveMeLocalColAndRowFromStep(int step, char *fileName, int node, int &nLocalCols, int &nLocalRows, char *&line, size_t &len);
    void getElementMatrix(int step, T **&m, int nGlobalCols, int nGlobalRows, int nNodeX, int nNodeY, char *fileName, Line *lines);
    void drawWithVTK(T **p, int nRows, int nCols, int step, Line *lines, int dimLines, std::string edittext,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor> gridActor);
    void refreshWindowsVTK(T **p, int nRows, int nCols, int step, Line *lines, int dimLines,  vtkSmartPointer<vtkActor> gridActor);
    void readConfigurationFile(const char *filename, int infoFromFile[8], char *outputFileName);
    void loadHashmapFromFile(int nNodeX, int nNodeY, char *filename);
    size_t generalPorpouseGetline(char **lineptr, size_t *n, FILE *stream);
    void buildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows,vtkSmartPointer<vtkPoints> pts,vtkSmartPointer<vtkCellArray> cellLines,vtkSmartPointer<vtkPolyData> grid,vtkSmartPointer<vtkNamedColors> colors,vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor2D> actorBuildLine);
    void refreshBuildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors);
    vtkTextProperty* buildStepLine(int step,vtkSmartPointer<vtkTextMapper> ,vtkSmartPointer<vtkTextProperty> singleLineTextProp,vtkSmartPointer<vtkNamedColors> colors, std::string color);
    vtkNew<vtkActor2D> buildStepText(int step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer);
    void refreshBuildStepText(int step,vtkActor2D* stepLineTextActor);
};
