# GUI Implementation Notes

## Overview

This document describes the implementation of the ImGui-based graphical user interface for the ITMO Loops audio composition system.

## Architecture

### Component Structure

```
lib/gui/
├── gui_app.hpp/cpp           # Main GUI application class
├── composition_parser.hpp/cpp # Parser for composition files
└── widgets/
    └── file_browser.hpp/cpp   # File selection widget

bin/
└── gui_main.cpp               # Entry point for GUI executable
```

### Key Design Decisions

1. **Separation of Concerns**: The GUI is implemented as a separate library (`itmoloops_gui_lib`) that can be linked independently from the audio engine.

2. **Stub Implementation**: All audio functionality (playback, synthesis, export) is implemented as stubs. This allows the GUI to be developed and tested independently while waiting for the audio engine implementation.

3. **ImGui Integration**: Using ImGui v1.91.5 fetched automatically via CMake's FetchContent. This ensures consistent versions and simplifies dependency management.

4. **Composition Parser**: A lightweight parser extracts the structure of composition files for display. This parser is separate from the actual audio engine parser and serves only for visualization purposes.

5. **File Browser Widget**: Custom implementation providing directory navigation and file filtering, avoiding the need for external file dialog libraries.

## Implementation Details

### Main Application Loop

The `GuiApp` class manages:
- GLFW window creation and event handling
- OpenGL context management
- ImGui initialization and rendering
- UI state and data management

### UI Panels

Each UI component is implemented as a separate method:
- `RenderMenuBar()`: Top menu with File, Edit, View, Help
- `RenderFileEditor()`: Text editor for composition files
- `RenderAudioControls()`: Transport controls and volume
- `RenderWaveformVisualization()`: Audio visualization (stub data)
- `RenderCompositionEditor()`: Hierarchical view of composition
- `RenderProjectSettings()`: BPM, export settings, statistics
- `RenderStatusBar()`: Messages and FPS counter

### Data Flow

```
File Load → Parser → Composition Structure → UI Display
         ↓
    Text Editor (editable)
         ↓
    Save → File System
```

### Future Integration Points

#### Audio Engine Integration

When integrating with the actual audio engine:

1. **Replace Stubs**: 
   - `PlayAudio()`, `PauseAudio()`, `StopAudio()`
   - `ExportWAV()`

2. **Add Callbacks**:
   - Audio engine → GUI for playback position updates
   - Audio engine → GUI for waveform/spectrum data

3. **Composition Parsing**:
   - Use the actual parser from `libItmoloops`
   - Convert parsed data to GUI display structures

4. **Real-time Updates**:
   - Parameter changes in GUI → Audio engine
   - Live audio synthesis preview

#### Example Integration

```cpp
// In PlayAudio()
if (audio_engine_) {
    auto composition = audio_engine_->Parse(file_content_);
    audio_engine_->Play(composition);
    is_playing_ = true;
}

// In RenderWaveformVisualization()
if (audio_engine_ && is_playing_) {
    waveform_data_ = audio_engine_->GetCurrentWaveform();
    spectrum_data_ = audio_engine_->GetCurrentSpectrum();
}
```

## Build System

### CMake Structure

```cmake
# Root CMakeLists.txt
- Finds OpenGL, GLFW3
- Fetches ImGui via FetchContent
- Creates imgui library target
- Adds lib/ and bin/ subdirectories

# lib/CMakeLists.txt
- Creates itmoloops_lib (main library)
- Creates itmoloops_gui_lib (GUI library)
- Links GUI lib with imgui, OpenGL, GLFW

# bin/CMakeLists.txt
- Creates itmoloops (CLI executable)
- Creates itmoloops_gui (GUI executable)
```

### Dependencies

**Required System Packages** (Ubuntu/Debian):
- `libglfw3-dev`: GLFW3 windowing library
- `libgl1-mesa-dev`: OpenGL development files
- `libglu1-mesa-dev`: OpenGL Utility library
- `xorg-dev`: X11 development files

**Automatically Fetched**:
- ImGui v1.91.5 from GitHub

## Testing

Since the GUI requires a display, testing in CI/CD or headless environments is not straightforward. The implementation includes:

1. **Build Verification**: CMake and compilation succeed
2. **Code Review**: Automated review for common issues
3. **Security Scan**: CodeQL analysis for vulnerabilities

For manual testing:
1. Build with `./build_gui.sh`
2. Run `./build/bin/itmoloops_gui`
3. Test file operations with example files
4. Verify UI responsiveness and layout

## Known Limitations

1. **No Docking**: ImGui docking feature is not enabled (requires newer ImGui or manual enable)
2. **Basic File Browser**: Simple implementation, no advanced features like file preview
3. **Stub Audio**: All audio functionality is placeholder
4. **Single Window**: No multi-document interface
5. **No Undo/Redo**: Would require implementing command pattern
6. **Static Visualization**: Waveform/spectrum use sample data

## Performance Considerations

1. **VSync Enabled**: Limits framerate to monitor refresh rate (typically 60 FPS)
2. **Efficient Rendering**: ImGui only redraws changed elements
3. **Parser Caching**: Composition is parsed on load/change, not every frame
4. **Buffer Management**: Text editor uses pre-allocated buffers to avoid reallocation

## Future Enhancements

### High Priority
- [ ] Integrate with actual audio engine
- [ ] Real-time audio synthesis
- [ ] Live parameter editing with immediate feedback
- [ ] Proper syntax highlighting in text editor

### Medium Priority
- [ ] Undo/Redo system
- [ ] Drag-and-drop file support
- [ ] Save/load window layout
- [ ] Customizable keyboard shortcuts
- [ ] Better error handling with modal dialogs

### Low Priority
- [ ] Multi-document interface
- [ ] Theme customization
- [ ] Plugin system for instruments/effects
- [ ] MIDI input support
- [ ] Export to multiple formats

## Troubleshooting

### Build Issues

**"Could NOT find OpenGL"**
```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

**"Could NOT find glfw3"**
```bash
sudo apt-get install libglfw3-dev
```

**CMake version too old**
```bash
# Ubuntu 22.04+ should have CMake 3.24+
cmake --version
```

### Runtime Issues

**Window doesn't open / Black screen**
- Check OpenGL driver support: `glxinfo | grep "OpenGL version"`
- Try updating graphics drivers

**Slow/laggy interface**
- Disable VSync in gui_app.cpp: `glfwSwapInterval(0)`
- Reduce window size

**File browser crashes**
- Check file permissions in directory
- Verify path exists

## Contributing

When extending the GUI:

1. **Add New Widgets**: Create in `lib/gui/widgets/`
2. **Add New Panels**: Add render method to `GuiApp` class
3. **Update Parser**: Modify `CompositionParser` for new syntax
4. **Document Changes**: Update GUI_README.md
5. **Test Build**: Run `./build_gui.sh`

## License

Part of ITMO Loops project - C++ Year 2025 labwork5
