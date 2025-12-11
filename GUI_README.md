# ITMO Loops GUI

A graphical user interface for the ITMO Loops audio composition system, built with ImGui, GLFW3, and OpenGL.

## Features

### 1. File Management
- **Open Files**: Load `.txt` composition files via File → Open or Ctrl+O
- **Save Files**: Save changes to current file with Ctrl+S
- **Save As**: Save to a new file with Ctrl+Shift+S
- **Recent Files**: Quick access to recently opened files in the File menu
- **File Editor**: Multi-line text editor for viewing and editing composition files

### 2. Audio Playback Controls
- **Play/Pause**: Start or pause audio playback
- **Stop**: Stop playback and reset to the beginning
- **Progress Bar**: Visual display of playback progress with time indicators
- **Volume Control**: Adjustable volume slider (0.0 to 1.0)
- **Status Display**: Shows current playback status (Playing, Paused, Stopped)

### 3. Visualization
- **Waveform Display**: Real-time waveform visualization of the audio signal
- **Frequency Spectrum**: Histogram showing frequency distribution
- Interactive plots with ImGui's built-in plotting capabilities

### 4. Composition Editor
- **Instrument Tree**: Hierarchical view of instruments with their parameters
  - Instrument type (sampler, square, sine, triangle)
  - Parameters (sample path, root note, loop points, attack/release)
  - Effects chain display
- **Pattern List**: View and edit patterns with resolution settings
- **Note Table**: Tabular view of notes with columns for:
  - Time (start position in units)
  - Instrument name
  - Pitch (e.g., C5, D5, E5)
  - Duration (in units)
  - Velocity (0-100)

### 5. Project Settings
- **BPM Control**: Edit the beats-per-minute tempo setting
- **Export Path**: Configure the output WAV file path
- **Export WAV**: Export composition to WAV file (Ctrl+E)
- **Statistics Display**: 
  - Number of instruments
  - Number of patterns
  - Number of notes
  - Total duration

### 6. User Interface
- **Dark Theme**: Professional dark color scheme for comfortable editing
- **Modular Layout**: Separate windows for different functions
- **Menu Bar**: Standard menu system with keyboard shortcuts
- **Status Bar**: Bottom status bar with messages and FPS counter
- **Error Handling**: User-friendly error messages for file operations

## Building

### Dependencies
- CMake 3.24+
- C++20 compatible compiler
- OpenGL
- GLFW3
- ImGui (automatically fetched via CMake)

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev xorg-dev
```

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake glfw
```

See [MACOS_SETUP.md](./MACOS_SETUP.md) for detailed macOS setup instructions.

### Building the GUI

**Linux:**
```bash
./build_gui.sh
```

**macOS:**
```bash
./build_gui_macos.sh
```

**Manual Build:**
```bash
mkdir build
cd build
cmake ..
make -j4
```

### Running
```bash
./build/bin/itmoloops_gui
```

## Usage

1. **Open a Composition**: 
   - Click File → Open
   - Navigate to `examples/lick.txt` or another composition file
   - The file content will appear in the Composition Editor window

2. **Edit Composition**:
   - Modify the composition text in the editor
   - Changes can be saved with File → Save or Ctrl+S

3. **Play Audio** (Note: Audio backend integration pending):
   - Click Play in the Audio Controls panel
   - Use the Stop button to reset playback
   - Adjust volume with the slider

4. **View Details**:
   - Check the Composition Details panel for instrument and pattern information
   - View waveform and spectrum in the Visualization window
   - Monitor statistics in Project Settings

5. **Export**:
   - Set the output path in Project Settings
   - Click File → Export WAV or Ctrl+E
   - Note: WAV export requires audio backend integration

## Architecture

### Main Components

#### `GuiApp` Class (`lib/gui/gui_app.hpp/cpp`)
The main application class that manages:
- GLFW window and OpenGL context
- ImGui initialization and rendering loop
- UI state and data management
- File I/O operations
- Audio control interface (stub implementation)

#### UI Panels
- **Menu Bar**: File operations and application controls
- **File Editor**: Text editor with syntax highlighting (planned)
- **Audio Controls**: Transport controls and volume
- **Waveform Visualization**: Real-time audio visualization
- **Composition Editor**: Hierarchical view of composition structure
- **Project Settings**: Global settings and export controls
- **Status Bar**: Messages and application status

### Design Principles
- **Modular**: Each UI component is implemented as a separate method
- **Extensible**: Easy to add new widgets and features
- **Responsive**: Smooth 60 FPS rendering with VSync
- **Intuitive**: Standard UI patterns and keyboard shortcuts

## Integration Points

The GUI provides stub interfaces for integration with the audio engine:

### Audio Playback
```cpp
void PlayAudio();    // Start/resume playback
void PauseAudio();   // Pause playback
void StopAudio();    // Stop and reset
```

### File Processing
```cpp
void OpenFile(const std::string& filepath);
void SaveFile();
void ExportWAV(const std::string& filepath);
```

### Real-time Updates
The GUI is designed to support:
- Real-time parameter editing
- Live waveform visualization
- Interactive note editing
- Dynamic composition preview

## Future Enhancements

- [ ] Integrate with libItmoloops audio engine
- [ ] Implement syntax highlighting in text editor
- [ ] Add real-time audio synthesis
- [ ] Interactive waveform editing
- [ ] MIDI input support
- [ ] Plugin system for custom instruments/effects
- [ ] Undo/Redo system
- [ ] Drag-and-drop file support
- [ ] Multi-document interface
- [ ] Save/load GUI state (window positions, sizes)
- [ ] Customizable keyboard shortcuts
- [ ] Touch input support

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+O | Open file |
| Ctrl+S | Save file |
| Ctrl+Shift+S | Save file as |
| Ctrl+E | Export WAV |
| Alt+F4 | Exit application |
| Ctrl+Z | Undo (planned) |
| Ctrl+Y | Redo (planned) |
| Ctrl+X | Cut (planned) |
| Ctrl+C | Copy (planned) |
| Ctrl+V | Paste (planned) |

## Technical Details

### Dependencies Managed by CMake
- **ImGui v1.91.5**: Fetched from GitHub via FetchContent
- **GLFW3**: System-installed via package manager
- **OpenGL**: System GL library

### Window Configuration
- Initial size: 1600×900
- OpenGL 3.3 Core Profile
- VSync enabled for smooth rendering
- Support for multiple windows (docking-ready if enabled)

## Troubleshooting

### Build Issues
- **OpenGL not found**: Install mesa-common-dev and libgl1-mesa-dev
- **GLFW not found**: Install libglfw3-dev
- **CMake version**: Ensure CMake 3.24 or higher

### Runtime Issues
- **Window doesn't open**: Check OpenGL drivers
- **Slow rendering**: Disable VSync or reduce window size
- **File not loading**: Check file path and permissions

## Contributing

When adding new features to the GUI:
1. Add UI rendering methods to `GuiApp` class
2. Update state variables in the header file
3. Handle user input in the rendering methods
4. Integrate with audio backend when available
5. Update this README with new features

## License

Part of the ITMO Loops project - C++ Year 2025 labwork5
