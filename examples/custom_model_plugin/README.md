# Custom Model Plugin Example

This directory contains a complete plugin example for **Qt-VTK-viewer** that adds a custom model **without recompiling the main application**.

---

## Contents

* **CustomCell.h** – Definition of a custom cell class inheriting from `Element`
* **Plugin_MinimalExample.cpp** – Minimal plugin version (for understanding the structure)
* **Plugin_FullTemplate.cpp** – Fully working plugin implementation
* **CMakeLists.txt** – Build configuration
* **README.md** – This file

---

## Model Structure

`CustomCell` is a simple example model that:

* Stores a single integer value (0–255)
* Displays it using a color gradient:
  * 0–63: Blue → Cyan  
  * 64–127: Cyan → Green  
  * 128–191: Green → Yellow  
  * 192–255: Yellow → Red  

---

## ⚠️ IMPORTANT: Symbols from the Main Application

The plugin **MUST NOT** compile its own copy of `SceneWidgetVisualizerFactory.cpp`!

### Why?

* The plugin relies on symbols (functions, static variables) defined in the main application  
* If the plugin compiles its own copy of the Factory, it will create **a separate, isolated registry**
* Result: models registered inside the plugin **will not be visible** to the main application

### Solution used in this example:

* `CMakeLists.txt` only builds `CustomModelPlugin_Full.cpp`
* The main application is linked with `-rdynamic` to export symbols
* The plugin accesses these symbols via `RTLD_GLOBAL`

---

## Plugin Compilation

### Requirements

* CMake 3.16+
* C++20 or newer compiler (g++, clang++)
* VTK (same version as in the main application)
* Qt5 or Qt6 (optional – only if referenced in headers)
* The main application **must be linked with `-rdynamic`**

---

## 🔧 CMake Configuration Options

The plugin supports the following **customizable arguments**:

| Argument | Default Value | Description |
|-----------|----------------|-------------|
| `PLUGIN_MODEL_NAME` | `"Custom Model"` | Human-readable name of the model shown in the application's **Model** menu. |
| `PLUGIN_CELL_CLASS` | `CustomCell` | C++ class implementing the cell logic (must have a corresponding header file `<ClassName>.h`). |
| `QTVTKVIEWER_DIR` | `../..` | Path to the **Qt-VTK-viewer** source directory. |
| `OOPENCAL_DIR` | `${QTVTKVIEWER_DIR}/..` | Path to the **OOpenCAL** root directory. |

---

### Example: Default Build

```bash
mkdir build && cd build
cmake ..
make -j8
````

This will build a plugin named:

```
libCustomModelPlugin.so
```

containing a model called **Custom Model** based on class **CustomCell**.

---

### Example: Custom Model and Cell

You can override model name and cell class directly from the command line:

```bash
cmake .. \
  -DPLUGIN_MODEL_NAME="\"My Fire Model\"" \
  -DPLUGIN_CELL_CLASS=FireCell
make -j8
```

💡 **Note:**
If your model name contains spaces, it **must** be wrapped in escaped quotes (`\"My Model\"`) so the compiler interprets it as a string literal.
Example:

```bash
-DPLUGIN_MODEL_NAME="\"My Custom Model\""
```

---

### Example: Full Configuration with Paths

```bash
cmake .. \
  -DQTVTKVIEWER_DIR=/home/user/Qt-VTK-viewer \
  -DOOPENCAL_DIR=/home/user/OOpenCAL \
  -DPLUGIN_MODEL_NAME="\"Ball Simulation\"" \
  -DPLUGIN_CELL_CLASS=BallCell
make -j8
```

After compilation you’ll get:

```
libCustomModelPlugin.so  →  Model name: "Ball Simulation", Cell class: BallCell
```

---

### Output Files

The build process produces symbolic links for versioned libraries:

```
libCustomModelPlugin.so      → main plugin file
libCustomModelPlugin.so.1
libCustomModelPlugin.so.1.0
```

All point to the same compiled binary.

---

## 🧩 Plugin Build Info (CMake Output)

During compilation, the console will display information like:

```
========================================
CustomModelPlugin Configuration
========================================
Build type:          Release
Qt-VTK-viewer dir:   /path/to/Qt-VTK-viewer
OOpenCAL directory:  /path/to/OOpenCAL
VTK version:         9.3.0
----------------------------------------
Plugin model name:   "My Fire Model"
Plugin cell class:   FireCell
----------------------------------------
Output: libCustomModelPlugin.so
========================================
```

Additionally, compile-time messages (via `#pragma message`) will show:

```
[Plugin build info]
  Model name: "My Fire Model"
  Cell class: FireCell
```

---

## Verification

After building, you can verify that the plugin exports the required symbols:

```bash
nm -D libCustomModelPlugin.so | grep register
```

Expected output:

```
00000000000XXXXX T registerPlugin
```

---

## Using the Plugin

### Option 1: Manual loading in code

Add this to `main.cpp`:

```cpp
#include <dlfcn.h>

void loadPlugin(const char* path)
{
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) 
    {
        std::cerr << "Failed to load: " << dlerror() << std::endl;
        return;
    }
    
    typedef void (*RegisterFunc)();
    auto registerPlugin = (RegisterFunc)dlsym(handle, "registerPlugin");
    
    if (registerPlugin) 
    {
        registerPlugin();
        std::cout << "Plugin loaded: " << path << std::endl;
    }
}
```

Then, in `main()`:

```cpp
loadPlugin("./plugins/libCustomModelPlugin.so");
```

---

### Option 2: Command-line argument

```bash
./QtVtkViewer --plugin=./build/libCustomModelPlugin.so
```

---

### Option 3: Plugins directory

Place the plugin file (`libCustomModelPlugin.so`) into the `plugins/` directory next to the main application — it will be loaded automatically on startup.

---

## After Loading

1. Launch **Qt-VTK-viewer**
2. A new entry (e.g., **My Fire Model**) will appear in the **Model** menu
3. Select it to activate the model
4. Load a compatible dataset (formatted for your custom cell class)

---

## Creating Your Own Plugin

Use this directory as a starting point:

1. Copy `custom_model_plugin/` to a new folder (e.g. `ball_model_plugin/`)
2. Rename files:

   * `CustomCell.h` → `BallCell.h`
   * `CustomModelPlugin_Full.cpp` → `BallModelPlugin_Full.cpp`
3. In `BallCell.h`:

   * Rename the class to `BallCell`
   * Implement your logic
4. In `CMakeLists.txt`:

   * Update `project(BallModelPlugin)`
   * Adjust source file names
5. Build using:

   ```bash
   cmake .. -DPLUGIN_MODEL_NAME="\"Ball Simulation\"" -DPLUGIN_CELL_CLASS=BallCell
   make -j8
   ```

---

## Debugging

### Problem: Plugin not loading

```bash
ls -la libCustomModelPlugin.so
ldd libCustomModelPlugin.so
nm -D libCustomModelPlugin.so
```

### Problem: `registerPlugin` function not found

* Ensure function is marked as `extern "C"`
* Has `__attribute__((visibility("default")))`
* Named exactly `registerPlugin`

### Problem: Crash while loading

* Plugin and main app must be compiled with **the same compiler**
* VTK and Qt versions must match
* ABI (Application Binary Interface) must be compatible

---

## Plugin Code Flow

```
registerPlugin()                    // Entry point
    └─> SceneWidgetVisualizerFactory::registerModel()
            └─> Lambda creating SceneWidgetVisualizerAdapter<CustomCell>
                    └─> CustomCell implements Element interface
```

---

## Advanced: Plugin Metadata

The plugin may also provide extra functions for metadata:

```cpp
extern "C" 
{
__attribute__((visibility("default")))
const char* getPluginInfo()
{
    return "My Plugin v1.0 - Model description";
}

__attribute__((visibility("default")))
int getPluginVersion()
{
    return 100; // 1.00
}

__attribute__((visibility("default")))
const char* getModelName()
{
    return "MyModel";
}
} // extern "C"
```

The main application can call these to display plugin information.

---

## License

This example is part of **Qt-VTK-viewer**.

---

## See Also

* **[../../doc/PLUGIN_USER_GUIDE.md](../../doc/PLUGIN_USER_GUIDE.md)** – Complete user guide for creating plugins
* **[../../doc/PLUGIN_ARCHITECTURE.md](../../doc/PLUGIN_ARCHITECTURE.md)** – Technical implementation details
* **[../crowdsimulation_model_plugin/](../crowdsimulation_model_plugin/)** – Another example plugin
