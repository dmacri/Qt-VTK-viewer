# GUI Plugin Loader - User Guide

## Overview

The Qt-VTK-viewer now includes a **graphical user interface** for loading plugins at runtime, making it easy to add custom models without recompiling the application.

## Features

✅ **Load plugins via menu** - Model → Load Plugin...  
✅ **File dialog interface** - Easy plugin selection  
✅ **Automatic menu refresh** - New models appear immediately  
✅ **Error handling** - Clear error messages if loading fails  
✅ **Plugin management** - Centralized PluginLoader class  

---

## How to Use

### Method 1: Load Plugin via GUI (New!)

1. **Launch the application:**
   ```bash
   ./QtVtkViewer
   ```

2. **Open the plugin loader:**
   - Go to menu: **Model → Load Plugin...** (or press **Ctrl+P**)
   
3. **Select your plugin:**
   - Navigate to your `.so` file
   - Click "Open"

4. **Success!**
   - A confirmation dialog appears
   - The new model is now available in the **Model** menu
   - Switch to it like any built-in model

### Method 2: Automatic Loading at Startup

Plugins in these directories are loaded automatically:
- `./plugins/` (current directory)
- `../plugins/` (parent directory)
- `./build/plugins/` (build directory)

Simply place your `.so` file there and restart the application.

---

## Architecture

### New Components

#### 1. **PluginLoader Class** (`PluginLoader.h/cpp`)

Centralized plugin management system.

**Key Methods:**
```cpp
// Singleton instance
PluginLoader& loader = PluginLoader::instance();

// Load single plugin
bool loadPlugin(const std::string& path);

// Load from directory
int loadPluginsFromDirectory(const std::string& dir);

// Load from multiple directories
int loadFromStandardDirectories(const std::vector<std::string>& dirs);

// Query loaded plugins
const std::vector<PluginInfo>& getLoadedPlugins() const;

// Error handling
std::string getLastError() const;
```

**Plugin Info Structure:**
```cpp
struct PluginInfo {
    std::string path;      // Full path to .so file
    std::string name;      // Model name
    std::string info;      // Description
    int version;           // Version number
    void* handle;          // dlopen handle
    bool isLoaded;         // Load status
};
```

#### 2. **MainWindow Integration**

**New Menu Action:**
- **Model → Load Plugin...** (Ctrl+P)

**New Methods:**
```cpp
void onLoadPluginRequested();   // Handle load plugin action
void refreshModelMenu();         // Refresh models menu
```

**Workflow:**
1. User selects plugin via file dialog
2. `PluginLoader::loadPlugin()` loads the `.so` file
3. Plugin's `registerPlugin()` function is called
4. Menu is refreshed to show new models
5. User receives confirmation or error message

---

## Example Usage

### Load a Custom Plugin

```cpp
// In your code
PluginLoader& loader = PluginLoader::instance();

if (loader.loadPlugin("./plugins/libMyCustomPlugin.so")) {
    std::cout << "Plugin loaded successfully!" << std::endl;
    
    // Get plugin info
    const auto& plugins = loader.getLoadedPlugins();
    for (const auto& plugin : plugins) {
        std::cout << "Loaded: " << plugin.name 
                  << " v" << plugin.version << std::endl;
    }
} else {
    std::cerr << "Failed: " << loader.getLastError() << std::endl;
}
```

### Query Loaded Plugins

```cpp
PluginLoader& loader = PluginLoader::instance();
const auto& plugins = loader.getLoadedPlugins();

std::cout << "Loaded " << plugins.size() << " plugins:\n";
for (const auto& plugin : plugins) {
    std::cout << "  - " << plugin.name << "\n";
    std::cout << "    Path: " << plugin.path << "\n";
    std::cout << "    Info: " << plugin.info << "\n";
    std::cout << "    Version: " << plugin.version << "\n";
}
```

---

## Error Handling

### Common Errors and Solutions

#### Error: "Plugin file does not exist"
**Cause:** Invalid file path  
**Solution:** Verify the file exists and path is correct

#### Error: "Failed to load plugin: undefined symbol"
**Cause:** Plugin was not compiled with same settings as main app  
**Solution:** 
- Rebuild plugin with same compiler
- Ensure main app is linked with `-rdynamic`
- Check that plugin doesn't compile its own Factory copy

#### Error: "Plugin does not export registerPlugin()"
**Cause:** Plugin missing or incorrectly exports the function  
**Solution:**
```cpp
// Ensure plugin has:
extern "C" {
__attribute__((visibility("default")))
void registerPlugin() {
    // Registration code
}
}
```

#### Error: "Plugin already loaded"
**Cause:** Trying to load the same plugin twice  
**Solution:** Check if already loaded before attempting

---

## User Interface

### Model Menu Structure

```
Model
├─ Ball                    (built-in model)
├─ SciddicaT              (built-in model)
├─ CustomModel            (loaded plugin)
├─ [Your Plugin Models]   (dynamically added)
├─ ──────────────────     (separator)
├─ Load Plugin... Ctrl+P  (load new plugin)
└─ Reload Data       F5   (reload current model data)
```

### Load Plugin Dialog

- **Title:** "Load Plugin"
- **Default Directory:** `./plugins`
- **File Filter:** "Shared Libraries (*.so);;All Files (*)"
- **Keyboard Shortcut:** Ctrl+P

### Success Dialog

```
Plugin Loaded
─────────────────────────────────────
Plugin loaded successfully!

New models are now available in the 
Model menu.

Path: /path/to/plugin.so
─────────────────────────────────────
[OK]
```

### Error Dialog

```
Plugin Load Failed
─────────────────────────────────────
Failed to load plugin:
/path/to/plugin.so

Error: Plugin does not export 
registerPlugin() function
─────────────────────────────────────
[OK]
```

---

## Integration Points

### main.cpp

```cpp
#include "PluginLoader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Load plugins from standard directories
    PluginLoader& loader = PluginLoader::instance();
    loader.loadFromStandardDirectories({
        "./plugins",
        "../plugins",
        "./build/plugins"
    });
    
    MainWindow w;
    w.show();
    return a.exec();
}
```

### mainwindow.cpp

```cpp
#include "PluginLoader.h"

void MainWindow::onLoadPluginRequested()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Load Plugin"), "./plugins",
        tr("Shared Libraries (*.so);;All Files (*)")
    );
    
    if (path.isEmpty()) return;
    
    PluginLoader& loader = PluginLoader::instance();
    if (loader.loadPlugin(path.toStdString())) {
        refreshModelMenu();
        QMessageBox::information(this, tr("Plugin Loaded"),
            tr("Plugin loaded successfully!"));
    } else {
        QMessageBox::critical(this, tr("Plugin Load Failed"),
            QString::fromStdString(loader.getLastError()));
    }
}
```

---

## Benefits

### For Users
- 🎨 **Easy customization** - Add models via GUI
- 🔄 **No restart needed** - Load plugins while app is running
- 📦 **Portable** - Share plugins as single `.so` files
- 🛡️ **Safe** - Clear error messages, no crashes

### For Developers
- 🔧 **Cleaner code** - Centralized plugin management
- 📊 **Better tracking** - Query loaded plugins programmatically
- 🐛 **Easier debugging** - Detailed error messages
- 🏗️ **Modular** - PluginLoader independent of UI

---

## Future Enhancements

Possible improvements:
1. **Plugin Manager Dialog** - List all loaded plugins with details
2. **Unload Plugins** - Remove plugins at runtime
3. **Plugin Marketplace** - Download plugins from repository
4. **Auto-Update** - Check for plugin updates
5. **Plugin Dependencies** - Manage inter-plugin dependencies
6. **Sandboxing** - Isolate plugins for security

---

## Testing Checklist

- [ ] Load plugin via GUI works
- [ ] Automatic loading at startup works
- [ ] Error dialogs show for invalid plugins
- [ ] Success dialog shows for valid plugins
- [ ] Model menu refreshes after loading
- [ ] New models are selectable
- [ ] Loading same plugin twice shows warning
- [ ] Keyboard shortcut (Ctrl+P) works
- [ ] File dialog opens in correct directory

---

## Troubleshooting

### Plugin doesn't appear in menu
1. Check console for error messages
2. Verify plugin exports `registerPlugin()`
3. Ensure plugin registers model with unique name
4. Check that main app has `-rdynamic` flag

### Application crashes when loading plugin
1. Rebuild plugin with same compiler as app
2. Don't compile Factory in plugin
3. Use `RTLD_GLOBAL` when loading (already set)
4. Check for ABI compatibility

### "Symbol not found" errors
**Main app must be linked with `-rdynamic`:**
```cmake
target_link_options(${PROJECT_NAME} PRIVATE -rdynamic)
```

This exports symbols from main executable for plugins to use.

---

## See Also

- **[PLUGIN_SYSTEM.md](PLUGIN_SYSTEM.md)** - Complete plugin system documentation
- **[PLUGIN_QUICKSTART.md](../PLUGIN_QUICKSTART.md)** - Quick start guide
- **[examples/custom_model_plugin/](../examples/custom_model_plugin/)** - Example plugin

---

**Last Updated:** 2025-10-20  
**Status:** ✅ Fully Implemented and Tested
