# Custom Model Plugin Example

This directory contains a complete plugin example for Qt-VTK-viewer that adds a custom model **without recompiling the main application**.

## Contents

* **CustomCell.h** – Definition of a custom cell class inheriting from `Element`
* **CustomModelPlugin.cpp** – Minimal plugin version (for understanding the structure)
* **CustomModelPlugin_Full.cpp** – Fully working plugin implementation
* **CMakeLists.txt** – Build configuration
* **README.md** – This file

## Model Structure

`CustomCell` is a simple example model that:

* Stores a single integer value (0–255)
* Displays it using a color gradient:

  * 0–63: Blue → Cyan
  * 64–127: Cyan → Green
  * 128–191: Green → Yellow
  * 192–255: Yellow → Red

## Building the Plugin

### Requirements

* CMake 3.16+
* C++17 compiler (g++, clang++)
* VTK (same version as used in the main app)
* Qt5/Qt6 (optional, depending on headers)

### Build Steps

```bash
# In examples/custom_model_plugin/
mkdir build
cd build

# Configure (CMake will auto-detect Qt-VTK-viewer in ../..)
cmake ..

# Alternatively, provide paths manually:
cmake .. -DQTVTKVIEWER_DIR=/path/to/Qt-VTK-viewer2 \
         -DOOPENCAL_DIR=/path/to/OOpenCAL

# Compile
make

# Result: libCustomModelPlugin.so
```

### Verification

Check if the plugin exports the required functions:

```bash
nm -D libCustomModelPlugin.so | grep register
```

Expected output:

```
00000000000XXXXX T registerPlugin
```

## Using the Plugin

### Option 1: Manual loading in code

Add this to `main.cpp`:

```cpp
#include <dlfcn.h>

void loadPlugin(const char* path)
{
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load: " << dlerror() << std::endl;
        return;
    }
    
    typedef void (*RegisterFunc)();
    auto registerPlugin = (RegisterFunc)dlsym(handle, "registerPlugin");
    
    if (registerPlugin) {
        registerPlugin();
        std::cout << "Plugin loaded: " << path << std::endl;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Load plugin
    loadPlugin("./plugins/libCustomModelPlugin.so");
    
    MainWindow w;
    w.show();
    return a.exec();
}
```

### Option 2: Command-line argument

```bash
./QtVtkViewer --plugin=./build/libCustomModelPlugin.so
```

### Option 3: Plugins directory

Place `libCustomModelPlugin.so` in the `plugins/` directory next to the application — it will be loaded automatically on startup.

## After Loading

1. Launch Qt-VTK-viewer
2. A new entry **CustomModel** will appear in the **Model** menu
3. Select **Model → CustomModel** to activate it
4. Load a compatible data file (formatted for `CustomCell`)

## Creating Your Own Plugin

Use this example as a template:

1. Copy the full `custom_model_plugin/` directory
2. Rename it to `my_model_plugin/`
3. In `CustomCell.h`:

   * Rename the class to `MyModelCell`
   * Implement your own logic
4. In `CustomModelPlugin_Full.cpp`:

   * Replace `"CustomModel"` with `"MyModel"`
   * Update `SceneWidgetVisualizerAdapter<MyModelCell>`
5. In `CMakeLists.txt`:

   * Change `project(CustomModelPlugin)` to `project(MyModelPlugin)`
   * Update source file names
6. Build & use!

## Debugging

### Problem: Plugin not loading

```bash
# Check if the file exists and has executable permissions
ls -la libCustomModelPlugin.so

# Check dependencies
ldd libCustomModelPlugin.so

# Check exported symbols
nm -D libCustomModelPlugin.so
```

### Problem: `registerPlugin` function not found

Make sure that:

* The function is marked as `extern "C"`
* It has `__attribute__((visibility("default")))`
* The name is exactly `registerPlugin` (case-sensitive)

### Problem: Application crashes while loading

Verify that:

* The plugin was compiled using the **same compiler** as the app
* VTK versions match exactly
* ABI (Application Binary Interface) is compatible

## Plugin Code Flow

```
registerPlugin()                    // Entry point
    └─> SceneWidgetVisualizerFactory::registerModel()
            └─> Lambda creating SceneWidgetVisualizerAdapter<CustomCell>
                    └─> CustomCell implements Element interface
```

## Advanced: Plugin Metadata

The plugin can export additional functions:

```cpp
extern "C" {

// Plugin info
__attribute__((visibility("default")))
const char* getPluginInfo() {
    return "My Plugin v1.0 - Model description";
}

// Plugin version
__attribute__((visibility("default")))
int getPluginVersion() {
    return 100; // 1.00
}

// Model name
__attribute__((visibility("default")))
const char* getModelName() {
    return "MyModel";
}

}
```

The application can call these to display plugin information.

## License

This example is part of Qt-VTK-viewer and is available under the same license terms.

## Further Reading

See also:

* `../../doc/PLUGIN_SYSTEM.md` – Full plugin system documentation
* `../../doc/CHANGELOG_RUNTIME_MODELS.md` – Runtime model system changelog
