![Build Status: Linux](https://github.com/dmacri/Qt-VTK-viewer/actions/workflows/release--deb-package--ubuntu24.04.yml/badge.svg)

# Qt-VTK-viewer

A CMake/C++ project for embedding a VTK 3D view in a Qt window. This application provides a powerful interface for visualizing and interacting with 2D and 3D models and scientific data using the Visualization Toolkit (VTK) within a Qt-based graphical user interface.



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

### 🎥 2D and 3D View Modes

The application supports switching between **2D** and **3D** visualization modes!

**2D Mode (Default):**
- Top-down orthographic view optimized for 2D cellular automata
- Rotation disabled to prevent accidental camera movement
- Zoom and pan enabled
- **2D Ruler axes** with scale, tick marks, and grid lines around the scene

**3D Mode:**
- Full 3D perspective view with interactive camera controls
- Mouse-based rotation, pan, and zoom
- Precise camera control via sliders:
  - **Azimuth** slider: Horizontal rotation (-180° to +180°)
  - **Elevation** slider: Vertical tilt (-90° to +90°)
- **Orientation axes widget** in bottom-left corner (X=red, Y=green, Z=blue)
- Perfect for examining data from different angles

**How to Use:**
- Switch modes via **View → 2D Mode** or **View → 3D Mode**
- In 3D mode, camera control sliders appear above playback controls
- See [VIEW_MODES.md](doc/VIEW_MODES.md) for detailed documentation

**Quick Start:**
1. Run the application
2. **Model** → Select model (Ball or SciddicaT)
3. **File → Open Configuration** → Select config file
4. **View** → Choose 2D or 3D mode
5. Done! Visualization loaded
## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

* Icons from [icons8.com](https://icons8.com/)
