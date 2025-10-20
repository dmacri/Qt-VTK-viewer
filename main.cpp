/** @file main.cpp
 * @brief Main entry point for the Qt-VTK Viewer application.
 *
 * This file initializes the Qt application, sets up the main window,
 * handles command-line arguments for loading initial configurations,
 * and loads model plugins from the plugins directory. */

/** @mainpage Qt-VTK Viewer
 * @tableofcontents
 *
 * @section intro_sec Introduction
 * A Qt-based application for visualizing VTK data with a user-friendly interface.
 *
 * @section features_sec Features
 * - Load and visualize VTK data files
 * - Interactive 3D visualization
 * - Support for multiple model types (runtime switchable)
 * - Plugin system for custom models (no recompilation needed)
 * - Video export functionality
 *
 * @include README.md */

#include "mainwindow.h"
#include "PluginLoader.h"

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

    // Load plugins from standard locations
    // This happens before MainWindow creation so models are available immediately
    PluginLoader& pluginLoader = PluginLoader::instance();
    pluginLoader.loadFromStandardDirectories({
        "./plugins",                    // Current directory
        "../plugins",                   // Parent directory
        "./build/plugins"               // Build directory
    });

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
