/** @file main.cpp
 * @brief Main entry point for the Qt-VTK Viewer application.
 *
 * This file initializes the Qt application, sets up the main window,
 * and handles command-line arguments for loading initial configurations. */

/** @mainpage Qt-VTK Viewer
 * @tableofcontents
 *
 * @section intro_sec Introduction
 * A Qt-based application for visualizing VTK data with a user-friendly interface.
 *
 * @section features_sec Features
 * - Load and visualize VTK data files
 * - Interactive 3D visualization
 * - Support for multiple model types
 * - Video export functionality
 *
 * @include README.md */

#include "mainwindow.h"

#include <filesystem>
#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>


int main(int argc, char *argv[])
{
    // vtkObject::GlobalWarningDisplayOff();

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setApplicationName("Visualiser");

    MainWindow mainWindow;

    if (argc > 1)
    {
        const auto configurationFile = argv[1];
        if (std::filesystem::exists(configurationFile))
        {
            mainWindow.loadInitialConfiguration(configurationFile);
        }
        else
        {
            std::cerr << "Provided argument is not valid file path: '" << configurationFile << "'!" << std::endl;
        }
    }

    if (QFile styleFile("style.qss"); styleFile.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        mainWindow.setStyleSheet(styleSheet);
    }

    mainWindow.show();
    return a.exec();
}
