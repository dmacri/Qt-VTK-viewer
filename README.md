![Build Status: Linux](https://github.com/dmacri/Qt-VTK-viewer/actions/workflows/release--deb-package--ubuntu24.04.yml/badge.svg)

# Qt-VTK-viewer
A CMake/C++ project for embedding a VTK 3D view in a Qt window.

![Qt VTK Viewer on Windows](doc/screenshot.png)

## Prerequisites
- Qt 6.x downloaded.
- VTK 9.x source downloaded & compiled. See [Build-VTK.md](doc/Build-VTK.md) for a guide to compile VTK.
- **OOpenCAL source code available locally.**  
  The project uses headers from the OOpenCAL library.  
  In CMake you must set the variable **`OOPENCAL_DIR`** to point to the **base directory** of your OOpenCAL checkout.  
  Example: if your local folder structure is  
```
~/BestSoftwareEver/OOpenCAL/models/Ball/BallCell.h
```
then `OOPENCAL_DIR` should be set to:
```
~/BestSoftwareEver
````
so that includes like:
```cpp
#include <OOpenCAL/models/Ball/BallCell.h>
````
work correctly.

## Usage

1. Clone or download this repository.
2. Open CMakeLists.txt in Qt Creator.
3. Configure project.

   1. Set build directory.
   2. Specify VTK\_DIR.
   3. Specify OOPENCAL\_DIR (see above).
4. Build project.

Done. Happy coding :)

## Features

### ✨ Runtime Model Switching & Configuration Loading (NEW!)

The application now supports **dynamic model switching and configuration loading at runtime** without recompilation!

**Model Switching:**
- Switch between different cell models (Ball, SciddicaT) via the **Model menu**
- No need to edit `SceneWidgetVisualizerProxyDefault.h` and recompile
- Easy to extend with new models

**Configuration Loading:**
- Load new configuration files via **File → Open Configuration** (Ctrl+O)
- Automatically clears scene and reloads with new parameters
- Updates GUI (steps, dimensions) automatically

**Quick Start:**
1. Run the application
2. **Model** → Select model (Ball or SciddicaT)
3. **File → Open Configuration** → Select config file
4. Done! Visualization loaded
## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

* Icons from [icons8.com](https://icons8.com/)
