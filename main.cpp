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

#include <filesystem>
#include <iostream>
#include <dlfcn.h>
#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>

namespace {

/** @brief Load a single plugin from the specified path.
 * 
 * @param pluginPath Path to the plugin shared library (.so file)
 * @return true if plugin was loaded successfully, false otherwise */
bool loadPlugin(const std::string& pluginPath)
{
    // Use RTLD_GLOBAL so plugin can access symbols from main app
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle)
    {
        std::cerr << "Failed to load plugin: " << pluginPath << std::endl;
        std::cerr << "Error: " << dlerror() << std::endl;
        return false;
    }

    dlerror(); // Clear any existing errors

    typedef void (*RegisterFunc)();
    RegisterFunc registerPlugin = (RegisterFunc)dlsym(handle, "registerPlugin");
    
    const char* dlsym_error = dlerror();
    if (dlsym_error || !registerPlugin)
    {
        std::cerr << "Plugin " << pluginPath 
                  << " does not export registerPlugin() function" << std::endl;
        if (dlsym_error)
            std::cerr << "Error: " << dlsym_error << std::endl;
        dlclose(handle);
        return false;
    }

    try
    {
        registerPlugin();
        std::cout << "✓ Loaded plugin: " << pluginPath << std::endl;
        
        // Optionally get and display plugin info
        typedef const char* (*InfoFunc)();
        InfoFunc getPluginInfo = (InfoFunc)dlsym(handle, "getPluginInfo");
        if (getPluginInfo)
        {
            std::cout << "  Info: " << getPluginInfo() << std::endl;
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception while registering plugin: " << e.what() << std::endl;
        dlclose(handle);
        return false;
    }
    
    // Note: We intentionally don't dlclose() - plugin must remain loaded
}

/** @brief Load all plugins from a directory.
 * 
 * Scans the directory for .so files and attempts to load each as a plugin.
 * 
 * @param pluginDir Path to the directory containing plugins
 * @return Number of successfully loaded plugins */
int loadPluginsFromDirectory(const std::string& pluginDir)
{
    namespace fs = std::filesystem;
    
    if (!fs::exists(pluginDir) || !fs::is_directory(pluginDir))
    {
        return 0;
    }

    int loadedCount = 0;
    std::cout << "Scanning for plugins in: " << pluginDir << std::endl;

    for (const auto& entry : fs::directory_iterator(pluginDir))
    {
        if (!entry.is_regular_file())
            continue;
            
        const auto& path = entry.path();
        
        // Check if it's a shared library
        if (path.extension() != ".so")
            continue;

        if (loadPlugin(path.string()))
        {
            loadedCount++;
        }
    }

    if (loadedCount > 0)
    {
        std::cout << "Loaded " << loadedCount << " plugin(s) from " << pluginDir << std::endl;
    }
    
    return loadedCount;
}
} // anonymous namespace

int main(int argc, char *argv[])
{
    // vtkObject::GlobalWarningDisplayOff();

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setApplicationName("Visualiser");

    // Load plugins from standard locations
    // This happens before MainWindow creation so models are available immediately
    std::vector<std::string> pluginDirs = {
        "./plugins",                    // Current directory
        "../plugins",                   // Parent directory
        "./build/plugins"               // Build directory
    };
    
    for (const auto& dir : pluginDirs)
    {
        loadPluginsFromDirectory(dir);
    }

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
