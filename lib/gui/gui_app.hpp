#pragma once

#include <string>
#include <vector>
#include <memory>

struct GLFWwindow;

namespace itmoloops {
namespace gui {

class GuiApp {
public:
    GuiApp();
    ~GuiApp();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void SetupImGuiStyle();
    void RenderUI();
    
    // UI Components
    void RenderMenuBar();
    void RenderFileEditor();
    void RenderAudioControls();
    void RenderWaveformVisualization();
    void RenderCompositionEditor();
    void RenderProjectSettings();
    void RenderStatusBar();

    // File operations
    void OpenFile(const std::string& filepath);
    void SaveFile();
    void SaveFileAs(const std::string& filepath);
    void AddRecentFile(const std::string& filepath);
    
    // Audio operations
    void PlayAudio();
    void PauseAudio();
    void StopAudio();
    void ExportWAV(const std::string& filepath);

    // State
    GLFWwindow* window_;
    bool show_demo_window_;
    
    // File state
    std::string current_file_;
    std::string file_content_;
    bool file_modified_;
    std::vector<std::string> recent_files_;
    
    // Audio state
    bool is_playing_;
    bool is_paused_;
    float current_time_;
    float total_duration_;
    float volume_;
    
    // Composition state
    int bpm_;
    std::string export_path_;
    
    // UI state
    bool show_file_dialog_;
    bool show_save_dialog_;
    bool show_export_dialog_;
    std::string error_message_;
    
    // Visualization data
    std::vector<float> waveform_data_;
    std::vector<float> spectrum_data_;
};

} // namespace gui
} // namespace itmoloops
