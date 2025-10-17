#include "visualiser/Visualizer.hpp"
#include "Line.h"
#include "widgets/ColorSettings.h"


void Visualizer::buildLoadBalanceLine(const std::vector<Line> &lines, int nCols, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkActor2D> actorBuildLine)
{
    vtkNew<vtkCellArray> cellLines;
    vtkNew<vtkPoints> pts;

    for (size_t i{}; i < lines.size(); i++)
    {
        std::cout << "Line (" << lines[i].x1 << ", " << lines[i].y1 << ") -> (" <<lines[i].x2 << ", " <<lines[i].y2 << ")" << std::endl;
        pts->InsertNextPoint(lines[i].x1 * 1, nCols-1-lines[i].y1 * 1, 0.0);
        pts->InsertNextPoint(lines[i].x2 * 1, nCols-1-lines[i].y2 * 1, 0.0);
        cellLines->InsertNextCell(2);
        cellLines->InsertCellPoint(i*2);
        cellLines->InsertCellPoint(i*2+1);
    }

    vtkNew<vtkPolyData> grid;
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

    const QColor gridColorFromSettings = ColorSettings::instance().gridColor();
    vtkColor3d vtkColor{
        gridColorFromSettings.redF(),
        gridColorFromSettings.greenF(),
        gridColorFromSettings.blueF()
    };
    actorBuildLine->GetProperty()->SetColor(vtkColor.GetData());

    // actorBuildLine->GetProperty()->SetLineWidth(1.5);
    renderer->AddActor2D(actorBuildLine);
}

void Visualizer::refreshBuildLoadBalanceLine(const std::vector<Line>& lines, int nCols, vtkActor2D* lineActor)
{
    vtkPolyDataMapper2D* mapper = (vtkPolyDataMapper2D*) lineActor->GetMapper();
    mapper->Update();
    vtkNew<vtkPolyData> grid;
    vtkNew<vtkPoints> pts;
    vtkNew<vtkCellArray> cellLines;

    for (int i = 0; i < lines.size(); i++)
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

vtkTextProperty* Visualizer::buildStepLine(StepIndex step, vtkSmartPointer<vtkTextMapper> singleLineTextB)
{
    std::string stepText = "Step " + std::to_string(step);
    singleLineTextB->SetInput(stepText.c_str());

    vtkTextProperty* singleLineTextProp = singleLineTextB->GetTextProperty();
    singleLineTextProp->SetVerticalJustificationToBottom();

    const QColor textColorFromSettings = ColorSettings::instance().textColor();
    vtkColor3d vtkColor{
        textColorFromSettings.redF(),
        textColorFromSettings.greenF(),
        textColorFromSettings.blueF()
    };
    singleLineTextProp->SetColor(vtkColor.GetData());

    return singleLineTextProp;
}

vtkNew<vtkActor2D> Visualizer::buildStepText(StepIndex step, int font_size, vtkSmartPointer<vtkTextMapper> stepLineTextMapper, vtkSmartPointer<vtkRenderer> renderer)
{
    stepLineTextMapper->SetInput(("Step " + std::to_string(step)).c_str());

    auto textProp = stepLineTextMapper->GetTextProperty();
    textProp->SetFontSize(font_size);
    textProp->SetFontFamilyToArial();
    textProp->BoldOn();
    textProp->ItalicOff();
    textProp->ShadowOff();
    textProp->SetVerticalJustificationToBottom();

    const QColor textColorFromSettings = ColorSettings::instance().textColor();
    vtkColor3d vtkColor{
        textColorFromSettings.redF(),
        textColorFromSettings.greenF(),
        textColorFromSettings.blueF()
    };
    textProp->SetColor(vtkColor.GetData());

    vtkNew<vtkActor2D> stepLineTextActor;
    stepLineTextActor->SetMapper(stepLineTextMapper);
    stepLineTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    stepLineTextActor->GetPositionCoordinate()->SetValue(0.05, 0.85);
    renderer->AddActor2D(stepLineTextActor);
    return stepLineTextActor;
}
