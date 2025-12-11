# macOS Setup Guide for ITMO Loops GUI

This guide provides instructions for building and running the ITMO Loops GUI on macOS.

## Prerequisites

### 1. Install Homebrew

If you don't have Homebrew installed, install it first:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 2. Install Xcode Command Line Tools

Install the Xcode Command Line Tools for the compiler:

```bash
xcode-select --install
```

This will open a dialog. Click "Install" and follow the prompts.

### 3. Install Dependencies

Install the required dependencies using Homebrew:

```bash
# Install CMake
brew install cmake

# Install GLFW3
brew install glfw

# Optional: Install pkg-config for dependency checking
brew install pkg-config
```

## Building the GUI

### Automated Build (Recommended)

Use the provided build script:

```bash
./build_gui_macos.sh
```

### Manual Build

If you prefer to build manually:

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build (use all CPU cores)
make -j$(sysctl -n hw.ncpu)
```

## Running the GUI

After building successfully:

```bash
./build/bin/itmoloops_gui
```

Or from the build directory:

```bash
cd build/bin
./itmoloops_gui
```

## Troubleshooting

### Build Error: "archive member '/' not a mach-o file"

This error means the `libitmoloops_lib.a` library is empty or corrupted. This has been fixed in the latest version.

**Solution:** Pull the latest changes and rebuild:
```bash
git pull
rm -rf build
./build_gui_macos.sh
```

See [MACOS_TROUBLESHOOTING.md](./MACOS_TROUBLESHOOTING.md) for detailed solutions to this and other build errors.

### CMake can't find GLFW

If CMake reports that it can't find GLFW, try:

```bash
# Make sure pkg-config is installed
brew install pkg-config

# Check if GLFW is properly installed
pkg-config --modversion glfw3

# If GLFW is installed but not found, you may need to set PKG_CONFIG_PATH
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"
```

For Intel Macs, use:
```bash
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
```

### OpenGL Issues

macOS uses OpenGL 4.1 by default. The GUI is compatible with OpenGL 3.3+, so it should work fine. If you encounter OpenGL-related issues:

1. Make sure your macOS is up to date
2. Check that you're not running in a virtual machine (OpenGL support may be limited)

### Permission Issues

If the app requests accessibility permissions:

1. Go to System Preferences → Security & Privacy → Privacy
2. Select "Accessibility" in the left sidebar
3. Add the `itmoloops_gui` executable to the list of allowed apps

### Retina Display Support

The GUI should automatically support Retina displays. If the interface appears too small or too large:

1. The window size can be adjusted by dragging the corners
2. ImGui will automatically handle high-DPI scaling

## Architecture Differences

### Apple Silicon (M1/M2/M3) vs Intel

The build process is the same for both architectures. Homebrew will install the appropriate binaries:

- **Apple Silicon**: `/opt/homebrew/`
- **Intel**: `/usr/local/`

The CMake configuration will automatically detect the correct paths.

## Alternative Installation Methods

### Using MacPorts (Alternative to Homebrew)

If you prefer MacPorts over Homebrew:

```bash
# Install dependencies
sudo port install cmake glfw pkg-config

# Then build normally
./build_gui_macos.sh
```

### Building GLFW from Source

If you need to build GLFW from source:

```bash
# Clone GLFW
git clone https://github.com/glfw/glfw.git
cd glfw

# Build and install
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

## Performance Tips

### Building with Ninja (Faster builds)

For faster builds, use Ninja instead of Make:

```bash
# Install Ninja
brew install ninja

# Configure with Ninja
cd build
cmake -G Ninja ..

# Build with Ninja
ninja
```

### Optimization Flags

For maximum performance, build in Release mode:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
```

## Uninstallation

To remove the installed dependencies:

```bash
brew uninstall glfw cmake
```

To remove the build artifacts:

```bash
rm -rf build
```

## Additional Notes

### Detailed Troubleshooting

For comprehensive troubleshooting including linker errors, build issues, and runtime problems, see [MACOS_TROUBLESHOOTING.md](./MACOS_TROUBLESHOOTING.md).

### File Paths

On macOS, file paths work similarly to Linux. The example files are in:

```
./examples/lick.txt
./examples/demo.txt
```

### Keyboard Shortcuts

macOS keyboard shortcuts in the GUI:

- ⌘O (Cmd+O) - Open file (uses Ctrl+O internally)
- ⌘S (Cmd+S) - Save file
- ⌘⇧S (Cmd+Shift+S) - Save As
- ⌘E (Cmd+E) - Export WAV
- ⌘Q (Cmd+Q) - Quit application

Note: The GUI uses Ctrl key bindings internally, but macOS may interpret Command key as Ctrl in some contexts.

## Known Limitations on macOS

1. **OpenGL Deprecation**: macOS has deprecated OpenGL in favor of Metal. The GUI will still work with OpenGL 3.3/4.1, but Apple may remove OpenGL support in future macOS versions. Consider migrating to Metal in the future.

2. **Window Management**: Some window management features may behave differently on macOS compared to Linux.

3. **File Dialog**: The custom file browser widget is used instead of native macOS file dialogs for consistency across platforms.

## Getting Help

If you encounter issues:

1. Check the build output for specific error messages
2. Verify all dependencies are installed: `brew list | grep -E "glfw|cmake"`
3. Check the main documentation: `GUI_README.md` and `IMPLEMENTATION_NOTES.md`
4. Ensure your macOS version supports OpenGL 3.3 or higher

## Version Information

Tested on:
- macOS Monterey (12.x) and later
- Xcode Command Line Tools 14.x+
- CMake 3.24+
- GLFW 3.3.10+

For older macOS versions, you may need to adjust the OpenGL version in the code.
