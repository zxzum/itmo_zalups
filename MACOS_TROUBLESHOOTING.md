# macOS Build Troubleshooting

## Common Build Errors and Solutions

### Error: "archive member '/' not a mach-o file in '../lib/libitmoloops_lib.a'"

This error occurs when trying to create a static library from an empty source file on macOS.

**Cause:** The linker (`ld` on macOS) is stricter than GNU's `ld` and rejects empty static libraries.

**Solution:** This has been fixed in the latest version. If you still encounter this error:

1. Make sure `lib/lib.cpp` is not empty:
```bash
cat lib/lib.cpp
```

2. If it's empty, add minimal content:
```bash
cat > lib/lib.cpp << 'EOF'
// ITMO Loops Library
// Placeholder implementation for the main library

namespace itmoloops {

// Version information
const char* GetVersion() {
    return "1.0.0";
}

} // namespace itmoloops
EOF
```

3. Clean and rebuild:
```bash
rm -rf build
./build_gui_macos.sh
```

### Error: "Could NOT find OpenGL"

**Cause:** OpenGL is deprecated on macOS but still available. CMake may not find it automatically.

**Solution:**

For Apple Silicon (M1/M2/M3):
```bash
export CMAKE_PREFIX_PATH="/opt/homebrew"
```

For Intel Macs:
```bash
export CMAKE_PREFIX_PATH="/usr/local"
```

Then rebuild:
```bash
rm -rf build
./build_gui_macos.sh
```

### Error: "Could NOT find glfw3"

**Cause:** GLFW is not installed or CMake can't find it.

**Solution:**

1. Install GLFW:
```bash
brew install glfw
```

2. If already installed, help CMake find it:
```bash
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"  # Apple Silicon
# or
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"     # Intel
```

3. Rebuild:
```bash
rm -rf build
./build_gui_macos.sh
```

### Error: "linker command failed with exit code 1"

**Cause:** Various linking issues, often related to missing frameworks or libraries.

**Solutions:**

1. **Check for missing frameworks:**
```bash
# Make sure you have the required development tools
xcode-select --install
```

2. **Verify all dependencies are installed:**
```bash
brew list | grep -E "glfw|cmake"
```

3. **Try verbose output to see exact error:**
```bash
cd build
make VERBOSE=1
```

4. **Clean build:**
```bash
rm -rf build
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Error: "No rule to make target 'imgui'"

**Cause:** CMake didn't download ImGui properly.

**Solution:**

1. Clean the build and CMake cache:
```bash
rm -rf build
rm -rf _deps
```

2. Ensure internet connection is available (ImGui is fetched from GitHub)

3. Rebuild:
```bash
./build_gui_macos.sh
```

### Warning: "OpenGL is deprecated"

**This is expected.** Apple deprecated OpenGL in favor of Metal, but OpenGL 3.3/4.1 still works fine on current macOS versions.

You can ignore this warning. The GUI will work correctly.

If you want to suppress the warning during compilation:
```bash
export CXXFLAGS="-Wno-deprecated-declarations"
cmake ..
make
```

### Application won't start or crashes immediately

**Solutions:**

1. **Check OpenGL support:**
```bash
system_profiler SPDisplaysDataType | grep "Metal"
```

If Metal is supported, OpenGL should work too.

2. **Grant accessibility permissions:**
   - System Preferences → Security & Privacy → Privacy
   - Add `itmoloops_gui` to Accessibility

3. **Try running from terminal to see error messages:**
```bash
./build/bin/itmoloops_gui
```

4. **Check for missing dynamic libraries:**
```bash
otool -L build/bin/itmoloops_gui
```

All libraries should be found. If you see "not found", reinstall dependencies.

### Retina Display Issues

If the UI appears too small or blurry:

1. ImGui should automatically handle Retina displays
2. Try resizing the window
3. Check if you're running in a non-native resolution

### Performance Issues

If the GUI is slow or choppy:

1. **Check CPU usage:**
```bash
top -pid $(pgrep itmoloops_gui)
```

2. **Try Release build for better performance:**
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
```

3. **Disable VSync if needed** (edit `lib/gui/gui_app.cpp`):
```cpp
// Change this line:
glfwSwapInterval(1);  // VSync enabled
// To:
glfwSwapInterval(0);  // VSync disabled
```

### Build is very slow

**Solutions:**

1. **Use Ninja instead of Make:**
```bash
brew install ninja
cd build
cmake -G Ninja ..
ninja
```

2. **Use all CPU cores:**
```bash
make -j$(sysctl -n hw.ncpu)
```

3. **Use ccache to cache compilation:**
```bash
brew install ccache
export CMAKE_CXX_COMPILER_LAUNCHER=ccache
cmake ..
make
```

## Platform-Specific Notes

### Apple Silicon (M1/M2/M3)

- All dependencies should be ARM64 native when installed via Homebrew
- No Rosetta 2 translation needed
- OpenGL runs natively via Metal translation layer
- Expected performance is excellent

### Intel Macs

- Uses x86_64 binaries
- OpenGL runs natively
- Performance depends on GPU capabilities

### macOS Versions

- **Monterey (12.x) and later**: Fully supported
- **Big Sur (11.x)**: Should work, test if possible
- **Catalina (10.15) and earlier**: May have issues with C++20 features

## Getting More Help

If none of these solutions work:

1. **Check build output carefully** for specific error messages
2. **Verify system compatibility:**
```bash
sw_vers  # macOS version
clang++ --version  # Compiler version
cmake --version  # CMake version
```

3. **Look at detailed build log:**
```bash
cd build
cmake .. 2>&1 | tee cmake.log
make VERBOSE=1 2>&1 | tee build.log
```

4. **Check the main documentation:**
   - `MACOS_SETUP.md` - Setup instructions
   - `GUI_README.md` - General GUI documentation
   - `IMPLEMENTATION_NOTES.md` - Technical details

## Known Limitations

1. **OpenGL Deprecation**: While OpenGL works now, Apple may remove it in future macOS versions
2. **Native File Dialogs**: The GUI uses a custom file browser instead of native macOS dialogs
3. **Keyboard Shortcuts**: Command key behavior may differ from native macOS apps
4. **Window Management**: Some macOS-specific window features may not work as expected

## Reporting Issues

When reporting build issues, please include:
1. macOS version (`sw_vers`)
2. Architecture (Intel or Apple Silicon)
3. CMake version (`cmake --version`)
4. Compiler version (`clang++ --version`)
5. Full error message from build
6. Output of `brew list | grep -E "glfw|cmake"`
