#include "gui_app.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace itmoloops {
namespace gui {

GuiApp::GuiApp()
    : window_(nullptr)
    , show_demo_window_(false)
    , current_file_("")
    , file_content_("")
    , file_modified_(false)
    , is_playing_(false)
    , is_paused_(false)
    , current_time_(0.0f)
    , total_duration_(60.0f)
    , volume_(0.8f)
    , bpm_(120)
    , export_path_("output.wav")
    , show_file_dialog_(false)
    , show_save_dialog_(false)
    , show_export_dialog_(false)
{
    // Initialize with some sample waveform data
    waveform_data_.resize(1000);
    for (size_t i = 0; i < waveform_data_.size(); ++i) {
        waveform_data_[i] = std::sin(i * 0.1f) * 0.5f;
    }
    
    spectrum_data_.resize(100);
    for (size_t i = 0; i < spectrum_data_.size(); ++i) {
        spectrum_data_[i] = std::exp(-i * 0.05f);
    }
}

GuiApp::~GuiApp() {
    Shutdown();
}

bool GuiApp::Initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window_ = glfwCreateWindow(1600, 900, "ITMO Loops - Audio Composition Editor", nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui style
    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void GuiApp::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
}

void GuiApp::Run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        RenderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.13f, 0.14f, 0.15f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
        
        // Update playback time
        if (is_playing_ && !is_paused_) {
            current_time_ += ImGui::GetIO().DeltaTime;
            if (current_time_ >= total_duration_) {
                current_time_ = total_duration_;
                is_playing_ = false;
            }
        }
    }
}

void GuiApp::Shutdown() {
    if (window_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window_);
        glfwTerminate();
        window_ = nullptr;
    }
}

void GuiApp::RenderUI() {
    // Main window with menu bar
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("MainWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    RenderMenuBar();

    ImGui::End();

    // Individual windows
    RenderFileEditor();
    RenderAudioControls();
    RenderWaveformVisualization();
    RenderCompositionEditor();
    RenderProjectSettings();
    RenderStatusBar();

    // Demo window (optional)
    if (show_demo_window_) {
        ImGui::ShowDemoWindow(&show_demo_window_);
    }
}

void GuiApp::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                show_file_dialog_ = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S", false, !current_file_.empty())) {
                SaveFile();
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                show_save_dialog_ = true;
            }
            ImGui::Separator();
            
            if (ImGui::BeginMenu("Recent Files")) {
                if (recent_files_.empty()) {
                    ImGui::MenuItem("(empty)", nullptr, false, false);
                } else {
                    for (const auto& file : recent_files_) {
                        if (ImGui::MenuItem(file.c_str())) {
                            OpenFile(file);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            
            ImGui::Separator();
            if (ImGui::MenuItem("Export WAV...", "Ctrl+E")) {
                show_export_dialog_ = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(window_, true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X", false, false)) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("File Editor", nullptr, true);
            ImGui::MenuItem("Audio Controls", nullptr, true);
            ImGui::MenuItem("Waveform", nullptr, true);
            ImGui::MenuItem("Composition Editor", nullptr, true);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &show_demo_window_);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                error_message_ = "ITMO Loops v1.0\nAudio Composition Editor";
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    // File browser dialog
    if (show_file_dialog_) {
        ImGui::OpenPopup("Open File");
        file_browser_.SetCurrentDirectory("./examples");
        file_browser_.SetFileFilter(".txt");
        show_file_dialog_ = false;
    }
    
    std::string selected_file;
    if (file_browser_.Show("Open File", selected_file)) {
        OpenFile(selected_file);
    }
    
    if (show_save_dialog_) {
        ImGui::OpenPopup("Save File As");
        show_save_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Save File As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char filepath[256] = "composition.txt";
        ImGui::InputText("File Path", filepath, 256);
        
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            SaveFileAs(filepath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    if (show_export_dialog_) {
        ImGui::OpenPopup("Export WAV");
        show_export_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Export WAV", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char filepath[256] = "output.wav";
        ImGui::InputText("File Path", filepath, 256);
        
        if (ImGui::Button("Export", ImVec2(120, 0))) {
            ExportWAV(filepath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void GuiApp::RenderFileEditor() {
    ImGui::Begin("Composition Editor");
    
    if (!current_file_.empty()) {
        ImGui::Text("File: %s%s", current_file_.c_str(), file_modified_ ? " *" : "");
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        if (ImGui::Button("Save", ImVec2(60, 0))) {
            SaveFile();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload", ImVec2(80, 0))) {
            OpenFile(current_file_);
        }
    } else {
        ImGui::TextDisabled("No file loaded - use File > Open to load a composition");
    }
    
    ImGui::Separator();
    
    // Multiline text editor with callback to track modifications
    static std::string temp_buffer;
    if (temp_buffer.capacity() < file_content_.size() + 4096) {
        temp_buffer.reserve(file_content_.size() + 4096);
    }
    temp_buffer = file_content_;
    
    if (ImGui::InputTextMultiline("##source", &temp_buffer[0], temp_buffer.capacity(),
                                  ImVec2(-1.0f, -1.0f),
                                  ImGuiInputTextFlags_AllowTabInput)) {
        file_content_ = temp_buffer;
        file_modified_ = true;
    }
    
    ImGui::End();
}

void GuiApp::RenderAudioControls() {
    ImGui::Begin("Audio Controls");
    
    // Transport controls
    ImGui::Text("Transport");
    ImGui::Separator();
    
    if (ImGui::Button(is_playing_ && !is_paused_ ? "Pause" : "Play", ImVec2(80, 30))) {
        if (is_playing_) {
            PauseAudio();
        } else {
            PlayAudio();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(80, 30))) {
        StopAudio();
    }
    
    // Progress bar
    ImGui::Spacing();
    ImGui::Text("Time: %.2f / %.2f s", current_time_, total_duration_);
    float progress = total_duration_ > 0 ? current_time_ / total_duration_ : 0.0f;
    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
    
    // Volume control
    ImGui::Spacing();
    ImGui::Text("Volume");
    ImGui::SliderFloat("##volume", &volume_, 0.0f, 1.0f, "%.2f");
    
    // Status
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Status: %s", 
                is_playing_ ? (is_paused_ ? "Paused" : "Playing") : "Stopped");
    
    ImGui::End();
}

void GuiApp::RenderWaveformVisualization() {
    ImGui::Begin("Waveform Visualization");
    
    ImGui::Text("Waveform Display");
    ImGui::Separator();
    
    // Plot waveform
    ImGui::PlotLines("##waveform", waveform_data_.data(), 
                     static_cast<int>(waveform_data_.size()),
                     0, nullptr, -1.0f, 1.0f, ImVec2(-1.0f, 150.0f));
    
    ImGui::Spacing();
    ImGui::Text("Frequency Spectrum");
    ImGui::Separator();
    
    // Plot spectrum
    ImGui::PlotHistogram("##spectrum", spectrum_data_.data(),
                         static_cast<int>(spectrum_data_.size()),
                         0, nullptr, 0.0f, 1.0f, ImVec2(-1.0f, 150.0f));
    
    ImGui::End();
}

void GuiApp::RenderCompositionEditor() {
    ImGui::Begin("Composition Details");
    
    if (ImGui::Button("Parse Composition", ImVec2(-1.0f, 0))) {
        composition_ = CompositionParser::Parse(file_content_);
        bpm_ = composition_.bpm;
        error_message_ = "Composition parsed successfully";
    }
    
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("Instruments", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        
        if (composition_.instruments.empty()) {
            ImGui::TextDisabled("No instruments defined");
        } else {
            for (const auto& inst : composition_.instruments) {
                std::string header = inst.name + " (" + inst.type + ")";
                if (ImGui::TreeNode(header.c_str())) {
                    // Display parameters
                    for (const auto& [key, value] : inst.params) {
                        ImGui::TextDisabled("  %s: %s", key.c_str(), value.c_str());
                    }
                    
                    // Display effects
                    if (!inst.effects.empty()) {
                        ImGui::Text("  Effects:");
                        for (const auto& effect : inst.effects) {
                            ImGui::TextDisabled("    - %s", effect.type.c_str());
                            for (const auto& [key, value] : effect.params) {
                                ImGui::TextDisabled("      %s: %s", key.c_str(), value.c_str());
                            }
                        }
                    }
                    
                    ImGui::TreePop();
                }
            }
        }
        
        ImGui::Unindent();
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Patterns", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        
        if (composition_.patterns.empty()) {
            ImGui::TextDisabled("No patterns defined");
        } else {
            for (const auto& pattern : composition_.patterns) {
                std::string header = pattern.name + " (resolution: " + 
                                   std::to_string(pattern.resolution) + ")";
                if (ImGui::TreeNode(header.c_str())) {
                    ImGui::TextDisabled("  Notes: %zu", pattern.notes.size());
                    if (!pattern.pattern_refs.empty()) {
                        ImGui::TextDisabled("  References: %zu", pattern.pattern_refs.size());
                    }
                    ImGui::TreePop();
                }
            }
        }
        
        ImGui::Unindent();
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Notes")) {
        ImGui::Indent();
        
        // Show notes from all patterns
        int total_notes = 0;
        for (const auto& pattern : composition_.patterns) {
            if (pattern.notes.empty()) continue;
            
            ImGui::Text("Pattern: %s", pattern.name.c_str());
            
            if (ImGui::BeginTable(("notes_" + pattern.name).c_str(), 5, 
                                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                                 ImGuiTableFlags_ScrollY, ImVec2(0, 150))) {
                ImGui::TableSetupColumn("Time");
                ImGui::TableSetupColumn("Instrument");
                ImGui::TableSetupColumn("Pitch");
                ImGui::TableSetupColumn("Duration");
                ImGui::TableSetupColumn("Velocity");
                ImGui::TableHeadersRow();
                
                for (const auto& note : pattern.notes) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%02d", note.start);
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", note.instrument.c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", note.pitch.c_str());
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%d", note.duration);
                    ImGui::TableSetColumnIndex(4); ImGui::Text("%d", note.velocity);
                    total_notes++;
                }
                
                ImGui::EndTable();
            }
            
            ImGui::Spacing();
        }
        
        if (total_notes == 0) {
            ImGui::TextDisabled("No notes defined");
        }
        
        ImGui::Unindent();
    }
    
    ImGui::End();
}

void GuiApp::RenderProjectSettings() {
    ImGui::Begin("Project Settings");
    
    ImGui::Text("Tempo");
    if (ImGui::InputInt("BPM", &bpm_)) {
        composition_.bpm = bpm_;
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Export");
    static char export_buffer[256] = "output.wav";
    ImGui::InputText("Output Path", export_buffer, 256);
    if (ImGui::Button("Export WAV", ImVec2(-1.0f, 0))) {
        ExportWAV(export_buffer);
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Statistics");
    ImGui::TextDisabled("Instruments: %zu", composition_.instruments.size());
    ImGui::TextDisabled("Patterns: %zu", composition_.patterns.size());
    
    // Count total notes
    int total_notes = 0;
    for (const auto& pattern : composition_.patterns) {
        total_notes += pattern.notes.size();
    }
    ImGui::TextDisabled("Notes: %d", total_notes);
    ImGui::TextDisabled("Duration: %.2f s", total_duration_);
    
    ImGui::End();
}

void GuiApp::RenderStatusBar() {
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 30));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings;
    
    ImGui::Begin("StatusBar", nullptr, flags);
    
    if (!error_message_.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", error_message_.c_str());
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(0)) {
                error_message_.clear();
            }
        }
    } else {
        ImGui::Text("Ready");
    }
    
    ImGui::SameLine(ImGui::GetWindowWidth() - 150);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    ImGui::End();
}

// File operations
void GuiApp::OpenFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        error_message_ = "Failed to open file: " + filepath;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file_content_ = buffer.str();
    file_content_.reserve(file_content_.size() + 1024); // Reserve extra space
    
    current_file_ = filepath;
    file_modified_ = false;
    AddRecentFile(filepath);
    
    // Parse the composition
    composition_ = CompositionParser::Parse(file_content_);
    bpm_ = composition_.bpm;
    
    error_message_ = "Loaded: " + filepath;
}

void GuiApp::SaveFile() {
    if (current_file_.empty()) {
        show_save_dialog_ = true;
        return;
    }
    SaveFileAs(current_file_);
}

void GuiApp::SaveFileAs(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        error_message_ = "Failed to save file: " + filepath;
        return;
    }
    
    file << file_content_;
    file.close();
    
    current_file_ = filepath;
    file_modified_ = false;
    AddRecentFile(filepath);
    error_message_ = "Saved: " + filepath;
}

void GuiApp::AddRecentFile(const std::string& filepath) {
    // Remove if already exists
    auto it = std::find(recent_files_.begin(), recent_files_.end(), filepath);
    if (it != recent_files_.end()) {
        recent_files_.erase(it);
    }
    
    // Add to front
    recent_files_.insert(recent_files_.begin(), filepath);
    
    // Limit to 10 recent files
    if (recent_files_.size() > 10) {
        recent_files_.resize(10);
    }
}

// Audio operations
void GuiApp::PlayAudio() {
    if (is_paused_) {
        is_paused_ = false;
    } else {
        is_playing_ = true;
        is_paused_ = false;
        // In a real implementation, start audio playback here
    }
    error_message_ = "Playing audio...";
}

void GuiApp::PauseAudio() {
    if (is_playing_) {
        is_paused_ = !is_paused_;
        error_message_ = is_paused_ ? "Paused" : "Resumed";
    }
}

void GuiApp::StopAudio() {
    is_playing_ = false;
    is_paused_ = false;
    current_time_ = 0.0f;
    error_message_ = "Stopped";
}

void GuiApp::ExportWAV(const std::string& filepath) {
    // In a real implementation, export the composition to WAV
    error_message_ = "WAV export not yet implemented";
}

} // namespace gui
} // namespace itmoloops
