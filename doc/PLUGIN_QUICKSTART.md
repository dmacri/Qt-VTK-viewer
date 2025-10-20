# Plugin Quick Start Guide

A quick guide to creating your own plugin for Qt-VTK-viewer in 5 minutes.

## 📦 What You’ll Need

* Installed and compiled Qt-VTK-viewer
* C++ compiler (g++, clang++)
* CMake 3.16+
* VTK (same version as the application)

## 🚀 Step by Step

### 1. Copy the plugin template

```bash
cd /path/to/Qt-VTK-viewer2
cp -r examples/custom_model_plugin my_plugin
cd my_plugin
```

### 2. Modify the model (CustomCell.h)

Open `CustomCell.h` and adapt the `CustomCell` class to your needs:

```cpp
class CustomCell : public Element
{
private:
    int value;  // Your data
    rgb outputColor{0, 0, 0};

public:
    // Constructors
    CustomCell() : value(0) {}
    CustomCell(int val) : value(val) {}

    // Parse from string
    void composeElement(char* str) override
    {
        this->value = atoi(str);
    }

    // Convert to string
    char* stringEncoding() override
    {
        char* zstr = new char[16];
        sprintf(zstr, "%d", value);
        return zstr;
    }

    // MOST IMPORTANT METHOD: defines visualization color
    rgb* outputValue(char* str) override
    {
        // Your coloring logic
        if (value == 0)
            outputColor = rgb(255, 255, 255);  // White
        else
            outputColor = rgb(255, 0, 0);      // Red
        
        return &outputColor;
    }

    void startStep(int /*step*/) override {}
};
```

### 3. Change the model name (CustomModelPlugin_Full.cpp)

Open `CustomModelPlugin_Full.cpp`:

```cpp
extern "C" {

__attribute__((visibility("default")))
void registerPlugin()
{
    SceneWidgetVisualizerFactory::registerModel("MyModel", []() {
        //                                        ^^^^^^^
        //                                     Change here!
        return std::make_unique<SceneWidgetVisualizerAdapter<CustomCell>>(
            999, "MyModel"  // And here!
        );
    });
    
    std::cout << "✓ MyModel plugin loaded!" << std::endl;
}

}
```

### 4. Compile the plugin

```bash
mkdir build
cd build
cmake ..
make
```

You will get: `libCustomModelPlugin.so`

### 5. Install the plugin

```bash
# Create plugins directory next to the application
mkdir -p ../../build/plugins

# Copy the plugin
cp libCustomModelPlugin.so ../../build/plugins/
```

### 6. Run the application

```bash
cd ../../build
./QtVtkViewer
```

In the console you should see:

```
Scanning for plugins in: ./plugins
✓ Loaded plugin: ./plugins/libCustomModelPlugin.so
  Info: CustomModel Plugin v1.0 - Example model for Qt-VTK-viewer
✓ MyModel plugin loaded!
Loaded 1 plugin(s) from ./plugins
```

In the **Model** menu, **MyModel** will appear!

## 🎯 Ready Examples

### Example 1: Temperature Gradient

```cpp
rgb* outputValue(char* str) override
{
    double temp = value / 100.0;  // Normalize 0-100 to 0.0-1.0
    
    if (temp < 0.5)
        outputColor = rgb(0, 0, static_cast<int>(255 * temp * 2));  // Black → Blue
    else
        outputColor = rgb(static_cast<int>(255 * (temp - 0.5) * 2), 0, 255);  // Blue → Purple
    
    return &outputColor;
}
```

### Example 2: Height Map

```cpp
rgb* outputValue(char* str) override
{
    int height = value;  // Assume value = terrain height
    
    if (height < 10)
        outputColor = rgb(0, 0, 255);        // Water - blue
    else if (height < 50)
        outputColor = rgb(0, 255, 0);        // Plain - green
    else if (height < 100)
        outputColor = rgb(139, 69, 19);      // Mountains - brown
    else
        outputColor = rgb(255, 255, 255);    // Snow - white
    
    return &outputColor;
}
```

### Example 3: Cell State (Cellular Automata)

```cpp
rgb* outputValue(char* str) override
{
    switch (value)
    {
    case 0:  // Dead
        outputColor = rgb(0, 0, 0);
        break;
    case 1:  // Alive
        outputColor = rgb(0, 255, 0);
        break;
    case 2:  // Infected
        outputColor = rgb(255, 0, 0);
        break;
    default: // Unknown
        outputColor = rgb(128, 128, 128);
    }
    return &outputColor;
}
```

## 🔧 Troubleshooting

### Plugin does not load

**Problem:** No plugin loaded message

**Solution:**

1. Check if the file is in `./plugins/`:

   ```bash
   ls -la ./plugins/
   ```

2. Check permissions:

   ```bash
   chmod +x ./plugins/libCustomModelPlugin.so
   ```

3. Check dependencies:

   ```bash
   ldd ./plugins/libCustomModelPlugin.so
   ```

### Error: "does not export registerPlugin()"

**Problem:** Plugin does not export `registerPlugin`

**Solution:**
Check symbols in the library:

```bash
nm -D libCustomModelPlugin.so | grep registerPlugin
```

It should show:

```
00000000000XXXXX T registerPlugin
```

If not, make sure that:

* The function has `extern "C"`
* It has `__attribute__((visibility("default")))`

### Application crashes on load

**Problem:** Segmentation fault

**Possible causes:**

1. **Compiler mismatch** – use the same compiler as for the application
2. **Different VTK versions** – must be identical
3. **Missing include guards** – add `#ifndef/#define/#endif` in headers

## 📚 Next Steps

* 📖 Read **[doc/PLUGIN_SYSTEM.md](doc/PLUGIN_SYSTEM.md)** for advanced techniques
* 🔍 Check **[examples/custom_model_plugin/README.md](examples/custom_model_plugin/README.md)** for more details
* 💡 Experiment with different coloring algorithms
* 🚀 Add metadata to your plugin (getPluginInfo, getPluginVersion)

## 💡 Tips

1. **Test iteratively** – change color, recompile, restart
2. **Start with simple colors** – rgb(255, 0, 0) is red
3. **Debug with prints** – `std::cout << "value=" << value << std::endl;`
4. **Make a backup** – before experimenting
5. **Check existing models** – `BallCell.h`, `SciddicaTCell.h` for inspiration

## ✅ Checklist

Before distributing your plugin:

* [ ] Plugin compiles without warnings
* [ ] `registerPlugin()` function is exported correctly
* [ ] Model has a unique name
* [ ] Colors are readable and meaningful
* [ ] Comments added in the code
* [ ] Tested with various input data
* [ ] Metadata added (getPluginInfo, getPluginVersion)
* [ ] README created for the plugin

---

**Good luck creating your own models!** 🎉

Got questions? See the full documentation in `doc/PLUGIN_SYSTEM.md`
