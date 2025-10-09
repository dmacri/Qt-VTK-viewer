#include "visualiser/Visualizer.hpp"
#include "Line.h"


void Visualizer::buildLoadBalanceLine(Line *lines, int dimLines,int nCols, int nRows, vtkSmartPointer<vtkPoints> pts, vtkSmartPointer<vtkCellArray> cellLines, vtkSmartPointer<vtkPolyData> grid, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkRenderer> renderer,vtkSmartPointer<vtkActor2D> actorBuildLine)
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

void Visualizer::refreshBuildLoadBalanceLine(Line *lines, int dimLines,int nCols,int nRows, vtkActor2D* lineActor,vtkSmartPointer<vtkNamedColors> colors)
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

vtkTextProperty* Visualizer::buildStepLine(StepIndex step, vtkSmartPointer<vtkTextMapper> singleLineTextB, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkNamedColors> colors, std::string color)
{
    std::string stepText = "Step " + std::to_string(step);
    singleLineTextB->SetInput(stepText.c_str());

    vtkTextProperty* textProp = singleLineTextB->GetTextProperty();
    textProp->ShallowCopy(singleLineTextProp);
    textProp->SetVerticalJustificationToBottom();
    textProp->SetColor(colors->GetColor3d(color).GetData());
    return textProp;
}

vtkNew<vtkActor2D> Visualizer::buildStepText(StepIndex step, int font_size, vtkSmartPointer<vtkNamedColors> colors, vtkSmartPointer<vtkTextProperty> singleLineTextProp, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer)
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
