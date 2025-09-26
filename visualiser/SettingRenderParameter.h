#pragma once

#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <QVTKOpenGLNativeWidget.h>


class SettingRenderParameter : public QVTKOpenGLNativeWidget
{
public:
    vtkNew<vtkNamedColors> colors;
    vtkSmartPointer<vtkRenderer> m_renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
};
