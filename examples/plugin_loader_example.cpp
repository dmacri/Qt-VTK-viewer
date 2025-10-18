/** plugin_loader_example.cpp
 * 
 * Example code showing how to load plugins in Qt-VTK-viewer
 * 
 * You can add this code to main.cpp to enable plugin loading functionality.
 * Choose one or combine multiple approaches based on your needs. */

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QFileDialog>
#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

// ============================================================================
// Approach 1: Load single plugin from path
// ============================================================================

bool loadPlugin(const std::string& pluginPath)
{
    // Open the shared library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle)
    {
        std::cerr << "Failed to load plugin: " << pluginPath << std::endl;
        std::cerr << "Error: " << dlerror() << std::endl;
        return false;
    }

    // Clear any existing errors
    dlerror();

    // Find the registerPlugin function
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

    // Call the registration function
    try
    {
        registerPlugin();
        std::cout << "✓ Loaded plugin: " << pluginPath << std::endl;
        
        // Optionally get plugin info
        typedef const char* (*InfoFunc)();
        InfoFunc getPluginInfo = (InfoFunc)dlsym(handle, "getPluginInfo");
        if (getPluginInfo)
        {
            std::cout << "  " << getPluginInfo() << std::endl;
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception while registering plugin: " << e.what() << std::endl;
        dlclose(handle);
        return false;
    }
    
    // Note: We intentionally don't dlclose() the handle
    // The plugin needs to remain loaded for the lifetime of the application
}

// ============================================================================
// Approach 2: Load all plugins from a directory
// ============================================================================

int loadPluginsFromDirectory(const std::string& pluginDir)
{
    if (!fs::exists(pluginDir))
    {
        std::cerr << "Plugin directory does not exist: " << pluginDir << std::endl;
        return 0;
    }

    if (!fs::is_directory(pluginDir))
    {
        std::cerr << "Not a directory: " << pluginDir << std::endl;
        return 0;
    }

    int loadedCount = 0;
    
    std::cout << "Scanning for plugins in: " << pluginDir << std::endl;

    for (const auto& entry : fs::directory_iterator(pluginDir))
    {
        if (!entry.is_regular_file())
            continue;
            
        const auto& path = entry.path();
        
        // Check if it's a shared library (.so on Linux, .dylib on macOS, .dll on Windows)
        std::string extension = path.extension().string();
        if (extension != ".so" && extension != ".dylib" && extension != ".dll")
            continue;

        // Try to load it
        if (loadPlugin(path.string()))
        {
            loadedCount++;
        }
    }

    std::cout << "Loaded " << loadedCount << " plugin(s) from " << pluginDir << std::endl;
    return loadedCount;
}

// ============================================================================
// Approach 3: Interactive plugin loading with file dialog
// ============================================================================

void loadPluginInteractive(QWidget* parent = nullptr)
{
    QString pluginPath = QFileDialog::getOpenFileName(
        parent,
        QObject::tr("Load Plugin"),
        "./plugins",
        QObject::tr("Shared Libraries (*.so *.dylib *.dll);;All Files (*)")
    );

    if (pluginPath.isEmpty())
        return;

    if (loadPlugin(pluginPath.toStdString()))
    {
        QMessageBox::information(parent, 
            QObject::tr("Plugin Loaded"),
            QObject::tr("Plugin loaded successfully!\n"
                       "New models may now be available in the Model menu.\n\n"
                       "Path: %1").arg(pluginPath));
    }
    else
    {
        QMessageBox::critical(parent,
            QObject::tr("Plugin Load Failed"),
            QObject::tr("Failed to load plugin from:\n%1\n\n"
                       "Check the console output for details.").arg(pluginPath));
    }
}

// ============================================================================
// Approach 4: Command line argument handling
// ============================================================================

void setupCommandLineParser(QCommandLineParser& parser)
{
    // Add plugin-related options
    parser.addOption(QCommandLineOption(
        {"p", "plugin"},
        QObject::tr("Load a plugin from the specified path"),
        QObject::tr("path")
    ));
    
    parser.addOption(QCommandLineOption(
        {"P", "plugin-dir"},
        QObject::tr("Load all plugins from the specified directory"),
        QObject::tr("directory")
    ));
}

void handleCommandLinePlugins(const QCommandLineParser& parser)
{
    // Load individual plugins
    if (parser.isSet("plugin"))
    {
        QStringList plugins = parser.values("plugin");
        for (const QString& plugin : plugins)
        {
            loadPlugin(plugin.toStdString());
        }
    }

    // Load plugins from directory
    if (parser.isSet("plugin-dir"))
    {
        QString pluginDir = parser.value("plugin-dir");
        loadPluginsFromDirectory(pluginDir.toStdString());
    }
}

// ============================================================================
// EXAMPLE: Integration into main.cpp
// ============================================================================

/*
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Setup command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Qt-VTK-viewer - Cellular Automata Visualizer");
    parser.addHelpOption();
    parser.addVersionOption();
    setupCommandLineParser(parser);
    parser.process(a);
    
    // Load plugins from standard directory (if exists)
    std::vector<std::string> standardPluginDirs = {
        "./plugins",
        "../plugins",
        "/usr/local/lib/qtvtkviewer/plugins",
        std::string(getenv("HOME") ? getenv("HOME") : "") + "/.local/share/qtvtkviewer/plugins"
    };
    
    for (const auto& dir : standardPluginDirs)
    {
        if (!dir.empty() && fs::exists(dir))
        {
            loadPluginsFromDirectory(dir);
        }
    }
    
    // Handle command line specified plugins
    handleCommandLinePlugins(parser);
    
    // Create and show main window
    MainWindow w;
    w.show();
    
    return a.exec();
}
*/

// ============================================================================
// EXAMPLE: Add menu action for loading plugins
// ============================================================================

/*
// In MainWindow.h:
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private slots:
    void on_actionLoadPlugin_triggered();
};

// In MainWindow.cpp:
void MainWindow::on_actionLoadPlugin_triggered()
{
    loadPluginInteractive(this);
    
    // Optionally refresh the models menu
    // refreshModelsMenu();
}
*/

// ============================================================================
// Helper: Get list of loaded plugins (requires keeping track)
// ============================================================================

struct PluginInfo
{
    std::string path;
    std::string name;
    std::string info;
    int version;
    void* handle;
};

class PluginManager
{
public:
    static PluginManager& instance()
    {
        static PluginManager pm;
        return pm;
    }
    
    bool loadPlugin(const std::string& path)
    {
        void* handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle)
            return false;
            
        typedef void (*RegisterFunc)();
        typedef const char* (*InfoFunc)();
        typedef int (*VersionFunc)();
        
        RegisterFunc registerPlugin = (RegisterFunc)dlsym(handle, "registerPlugin");
        if (!registerPlugin)
        {
            dlclose(handle);
            return false;
        }
        
        // Get metadata
        PluginInfo info;
        info.path = path;
        info.handle = handle;
        
        InfoFunc getPluginInfo = (InfoFunc)dlsym(handle, "getPluginInfo");
        if (getPluginInfo)
            info.info = getPluginInfo();
            
        VersionFunc getPluginVersion = (VersionFunc)dlsym(handle, "getPluginVersion");
        if (getPluginVersion)
            info.version = getPluginVersion();
            
        InfoFunc getModelName = (InfoFunc)dlsym(handle, "getModelName");
        if (getModelName)
            info.name = getModelName();
        
        // Register the plugin
        registerPlugin();
        
        // Store info
        plugins.push_back(info);
        
        return true;
    }
    
    const std::vector<PluginInfo>& getLoadedPlugins() const
    {
        return plugins;
    }
    
    ~PluginManager()
    {
        // Clean up all handles
        for (auto& plugin : plugins)
        {
            if (plugin.handle)
                dlclose(plugin.handle);
        }
    }
    
private:
    PluginManager() = default;
    std::vector<PluginInfo> plugins;
};

// Usage:
// PluginManager::instance().loadPlugin("./plugins/libCustomModel.so");
// auto loaded = PluginManager::instance().getLoadedPlugins();
