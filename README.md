![Build Status: Linux](https://github.com/dmacri/Qt-VTK-viewer/actions/workflows/release--deb-package--ubuntu24.04.yml/badge.svg)

# Qt-VTK-viewer

A CMake/C++ project for embedding a VTK 3D view in a Qt window. This application provides a powerful interface for visualizing and interacting with 2D and 3D models and scientific data using the Visualization Toolkit (VTK) within a Qt-based graphical user interface.

![Qt VTK Viewer on Windows](doc/screenshot.png)

## Project Overview

The Qt-VTK-viewer is designed to:
- Load and visualize 3D models and scientific data
- Provide interactive 3D navigation and manipulation
- Support various visualization techniques through VTK
- Offer a user-friendly interface built with Qt

## Prerequisites

- **Qt 6.x** - The application framework
- **VTK 9.x** - Visualization Toolkit (must be compiled from source)
  - See [Build-VTK.md](doc/Build-VTK.md) for compilation instructions
- **OOpenCAL** - Required for certain model types
  - The project uses headers from the OOpenCAL library
  - Set the `OOPENCAL_DIR` CMake variable to point to your OOpenCAL base directory

### OOpenCAL Setup

In CMake, set the `OOPENCAL_DIR` variable to the base directory of your OOpenCAL installation. For example, if your OOpenCAL files are located at:
```
~/BestSoftwareEver/OOpenCAL/models/Ball/BallCell.h
```
Then set:
```
OOPENCAL_DIR=~/BestSoftwareEver
```

## Documentation

Comprehensive API documentation is available and can be generated using Doxygen. To generate the documentation:

```bash
doxygen Doxyfile
```

The generated documentation will be available in the `doc/html` directory. Open `index.html` in a web browser to view it.

## Project Structure

Key components of the project include:

- **ModelReader**: Handles reading and parsing model data from files
- **MainWindow**: The main application window and UI implementation
- **Config**: Manages application configuration and settings
- **Visualization**: Contains VTK-based visualization components

## Building the Project

1. Create a build directory and configure the project:
   ```bash
   mkdir build && cd build
   cmake .. -DOOPENCAL_DIR=/path/to/your/OOpenCAL
   ```

2. Build the project:
   ```bash
   make
   ```

3. Run the application:
   ```bash
   ./QtVtkViewer
   ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
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

### ✨ Runtime Model Switching & Configuration Loading

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
