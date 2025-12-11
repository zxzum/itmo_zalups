#include "file_browser.hpp"
#include <imgui.h>

#include <filesystem>
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

namespace itmoloops {
namespace gui {
namespace widgets {

FileBrowser::FileBrowser()
    : current_directory_(".")
    , file_filter_("")
    , selected_index_(-1)
    , show_hidden_(false)
{
    std::memset(path_buffer_, 0, sizeof(path_buffer_));
    UpdateDirectoryListing();
}

void FileBrowser::SetCurrentDirectory(const std::string& dir) {
    current_directory_ = dir;
    UpdateDirectoryListing();
}

void FileBrowser::SetFileFilter(const std::string& filter) {
    file_filter_ = filter;
    UpdateDirectoryListing();
}

void FileBrowser::UpdateDirectoryListing() {
    files_.clear();
    directories_.clear();
    
    try {
        if (!fs::exists(current_directory_)) {
            current_directory_ = ".";
        }
        
        for (const auto& entry : fs::directory_iterator(current_directory_)) {
            std::string name = entry.path().filename().string();
            
            // Skip hidden files if not showing them
            if (!show_hidden_ && !name.empty() && name[0] == '.') {
                continue;
            }
            
            if (entry.is_directory()) {
                directories_.push_back(name);
            } else if (file_filter_.empty() || 
                      name.size() >= file_filter_.size() &&
                      name.compare(name.size() - file_filter_.size(), 
                                 file_filter_.size(), file_filter_) == 0) {
                files_.push_back(name);
            }
        }
        
        std::sort(directories_.begin(), directories_.end());
        std::sort(files_.begin(), files_.end());
        
    } catch (const std::exception& e) {
        // Silently fail and keep current listing
    }
}

bool FileBrowser::Show(const char* title, std::string& selected_path) {
    bool file_selected = false;
    
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Current directory display
        ImGui::Text("Directory: %s", current_directory_.c_str());
        ImGui::Separator();
        
        // Directory listing
        ImGui::BeginChild("FileBrowser", ImVec2(500, 300), true);
        
        // Parent directory
        if (current_directory_ != "." && current_directory_ != "/") {
            if (ImGui::Selectable("../", false)) {
                fs::path p(current_directory_);
                current_directory_ = p.parent_path().string();
                if (current_directory_.empty()) {
                    current_directory_ = ".";
                }
                UpdateDirectoryListing();
            }
        }
        
        // Directories
        for (const auto& dir : directories_) {
            std::string display = "[DIR] " + dir;
            if (ImGui::Selectable(display.c_str(), false)) {
                fs::path p(current_directory_);
                p /= dir;
                current_directory_ = p.string();
                UpdateDirectoryListing();
            }
        }
        
        // Files
        for (size_t i = 0; i < files_.size(); ++i) {
            bool is_selected = (static_cast<int>(i) == selected_index_);
            if (ImGui::Selectable(files_[i].c_str(), is_selected)) {
                selected_index_ = static_cast<int>(i);
                std::strncpy(path_buffer_, files_[i].c_str(), sizeof(path_buffer_) - 1);
            }
        }
        
        ImGui::EndChild();
        
        ImGui::Separator();
        
        // File name input
        ImGui::Text("File name:");
        ImGui::SameLine();
        ImGui::InputText("##filename", path_buffer_, sizeof(path_buffer_));
        
        ImGui::Checkbox("Show hidden files", &show_hidden_);
        if (ImGui::IsItemEdited()) {
            UpdateDirectoryListing();
        }
        
        ImGui::Separator();
        
        // Buttons
        if (ImGui::Button("Select", ImVec2(120, 0))) {
            if (std::strlen(path_buffer_) > 0) {
                fs::path p(current_directory_);
                p /= path_buffer_;
                selected_path = p.string();
                file_selected = true;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
    
    return file_selected;
}

} // namespace widgets
} // namespace gui
} // namespace itmoloops
