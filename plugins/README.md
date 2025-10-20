# Plugins Directory

This directory is used for storing compiled model plugins.

## Usage

1. Compile your plugin (`.so` file)
2. Place it in this directory
3. Launch QtVtkViewer — the plugin will be loaded automatically

## Example

```bash
# Compile the sample plugin
cd ../examples/custom_model_plugin
mkdir build && cd build
cmake ..
make

# Copy it to the plugins directory
cp libCustomModelPlugin.so ../../../plugins/

# Run the application
cd ../../../build
./QtVtkViewer
```

## Structure

```
plugins/
├── README.md              # This file
├── .gitkeep              # Keeps the directory tracked in git
└── *.so                  # Your plugins (not committed to git)
```

## See Also

* [../doc/PLUGIN_QUICKSTART.md](../doc/PLUGIN_QUICKSTART.md) - Quick start guide
* [../doc/PLUGIN_SYSTEM.md](../doc/PLUGIN_SYSTEM.md) - Full documentation
* [../examples/custom_model_plugin/](../examples/custom_model_plugin/) - Sample plugin implementation
