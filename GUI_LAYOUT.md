# GUI Layout Documentation

## Visual Layout

The ITMO Loops GUI is organized into a multi-panel layout with the following structure:

```
┌─────────────────────────────────────────────────────────────────────┐
│ Menu Bar: [File] [Edit] [View] [Help]                     FPS: 60.0│
├─────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌────────────────────────────┐  ┌──────────────────────────────┐  │
│  │ Composition Editor          │  │ Audio Controls                │  │
│  ├────────────────────────────┤  ├──────────────────────────────┤  │
│  │ File: examples/lick.txt    │  │ Transport                     │  │
│  │ [Save] [Reload]            │  │ [Play] [Stop]                 │  │
│  ├────────────────────────────┤  │                               │  │
│  │                            │  │ Time: 0.00 / 60.00 s          │  │
│  │ # ITMO Loops Demo          │  │ ████░░░░░░░░░░░░░░ 25%       │  │
│  │ bpm 140                    │  │                               │  │
│  │                            │  │ Volume                        │  │
│  │ instrument bass sampler    │  │ ████████░░ 0.80              │  │
│  │     sample=./samples/...   │  │                               │  │
│  │     root=C3                │  │ Status: Playing               │  │
│  │     attack=0.01            │  └──────────────────────────────┘  │
│  │     ...                    │                                     │
│  │                            │  ┌──────────────────────────────┐  │
│  │ pattern main resolution 16 │  │ Waveform Visualization        │  │
│  │     00 bass C3  4 70       │  ├──────────────────────────────┤  │
│  │     ...                    │  │ Waveform Display              │  │
│  │                            │  │     ╱╲      ╱╲      ╱╲       │  │
│  │                            │  │    ╱  ╲    ╱  ╲    ╱  ╲      │  │
│  │                            │  │ ──╯    ╰──╯    ╰──╯    ╰─    │  │
│  │                            │  │                               │  │
│  │                            │  │ Frequency Spectrum            │  │
│  │                            │  │ █                             │  │
│  │                            │  │ ██                            │  │
│  │                            │  │ ████▓▒░                       │  │
│  └────────────────────────────┘  └──────────────────────────────┘  │
│                                                                       │
│  ┌────────────────────────────┐  ┌──────────────────────────────┐  │
│  │ Composition Details         │  │ Project Settings              │  │
│  ├────────────────────────────┤  ├──────────────────────────────┤  │
│  │ [Parse Composition]        │  │ Tempo                         │  │
│  │                            │  │ BPM: [140      ]              │  │
│  │ ▼ Instruments              │  │                               │  │
│  │   ▶ bass (sampler)         │  │ Export                        │  │
│  │   ▶ lead (square)          │  │ Path: [output.wav________]    │  │
│  │   ▶ pad (sine)             │  │ [Export WAV]                  │  │
│  │                            │  │                               │  │
│  │ ▼ Patterns                 │  │ Statistics                    │  │
│  │   ▶ main (resolution: 16)  │  │ Instruments: 3                │  │
│  │                            │  │ Patterns: 1                   │  │
│  │ ▼ Notes                    │  │ Notes: 12                     │  │
│  │   Pattern: main            │  │ Duration: 60.00 s             │  │
│  │   ┌─────┬──────┬────┬───┐  │  │                               │  │
│  │   │Time │Inst  │Ptch│Vel│  │  │                               │  │
│  │   ├─────┼──────┼────┼───┤  │  │                               │  │
│  │   │ 00  │bass  │C3  │70 │  │  │                               │  │
│  │   │ 04  │bass  │G2  │65 │  │  │                               │  │
│  │   │ 00  │lead  │C5  │60 │  │  │                               │  │
│  │   └─────┴──────┴────┴───┘  │  │                               │  │
│  └────────────────────────────┘  └──────────────────────────────┘  │
│                                                                       │
├─────────────────────────────────────────────────────────────────────┤
│ Status: Loaded: examples/lick.txt                      FPS: 60.0    │
└─────────────────────────────────────────────────────────────────────┘
```

## Color Scheme

The GUI uses a dark theme with the following color palette:

- **Background**: Dark gray (#21222629)
- **Window Background**: Darker gray (#191A1B)
- **Text**: Light gray (#FFFFFF)
- **Accent**: Blue (#42A5F5) for buttons and selections
- **Hover**: Lighter blue (#42A5F5CC)
- **Active**: Bright blue (#42A5F5FF)
- **Borders**: Dark borders for subtle separation

## Window Sizes

Default window configuration:
- **Main Window**: 1600 × 900 pixels
- **Composition Editor**: ~40% of width, full height
- **Audio Controls**: ~25% of width, ~20% of height
- **Waveform Visualization**: ~25% of width, ~35% of height
- **Composition Details**: ~40% of width, ~40% of height
- **Project Settings**: ~25% of width, ~40% of height
- **Status Bar**: Full width, 30 pixels height

## Interaction Flow

### Opening a File
```
User → File Menu → Open
  ↓
File Browser Dialog (modal)
  ↓
Select File → examples/lick.txt
  ↓
File loads into Composition Editor
  ↓
Automatic parsing
  ↓
Composition Details updates
  ↓
Project Settings updates statistics
```

### Playing Audio
```
User → Audio Controls → [Play]
  ↓
is_playing_ = true
  ↓
Progress bar starts animating
  ↓
Waveform updates (real-time)
  ↓
Spectrum updates (real-time)
  ↓
Time display updates
```

### Editing Composition
```
User → Composition Editor → Edit text
  ↓
file_modified_ = true
  ↓
File indicator shows "*"
  ↓
User → [Save] button or Ctrl+S
  ↓
File saves to disk
  ↓
file_modified_ = false
  ↓
Composition Details → [Parse Composition]
  ↓
Structure updates in UI
```

## Keyboard Shortcuts

| Shortcut | Action | Window |
|----------|--------|--------|
| Ctrl+O | Open file | File Browser |
| Ctrl+S | Save file | - |
| Ctrl+Shift+S | Save As | File Browser |
| Ctrl+E | Export WAV | Export Dialog |
| Alt+F4 | Exit | - |
| Tab | Insert tab | Composition Editor |

## Responsive Design

The GUI adapts to different window sizes:
- Minimum practical size: 1024 × 768
- Recommended: 1600 × 900 or higher
- Panels can be resized by dragging borders
- Text editor scrolls when content exceeds window
- Tables have scrollbars when needed

## Accessibility Features

- High contrast dark theme reduces eye strain
- Clear visual hierarchy with borders and spacing
- Status messages for user feedback
- Error messages in red color
- FPS counter for performance monitoring
- Keyboard navigation support (ImGui built-in)

## Future Layout Enhancements

Potential improvements for the layout:

1. **Dockable Windows**: Enable ImGui docking for user-customizable layouts
2. **Tabbed Interface**: Multiple composition files in tabs
3. **Collapsible Panels**: Hide unused panels to maximize workspace
4. **Split View**: Side-by-side composition comparison
5. **Fullscreen Mode**: Distraction-free editing mode
6. **Zoom Controls**: For waveform and text editor
7. **Minimap**: Overview of entire composition
8. **Piano Roll**: Visual note editor alternative to text

## Panel Descriptions

### Menu Bar
Top-level navigation with standard menu items (File, Edit, View, Help)

### Composition Editor
Main text editing area for composition files with syntax support

### Audio Controls
Transport controls (Play/Pause/Stop), progress tracking, and volume

### Waveform Visualization
Real-time waveform and spectrum analysis display

### Composition Details
Hierarchical tree view of parsed composition structure

### Project Settings
Global settings, statistics, and export functionality

### Status Bar
Application messages, errors, and performance metrics
