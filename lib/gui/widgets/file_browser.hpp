#pragma once

#include <string>
#include <vector>
#include <functional>

namespace itmoloops {
namespace gui {
namespace widgets {

// Simple file browser widget for selecting files
class FileBrowser {
public:
    FileBrowser();
    
    // Shows the file browser dialog
    // Returns true if a file was selected
    bool Show(const char* title, std::string& selected_path);
    
    // Set initial directory
    void SetCurrentDirectory(const std::string& dir);
    
    // Set file filter (e.g., ".txt")
    void SetFileFilter(const std::string& filter);

private:
    void UpdateDirectoryListing();
    
    std::string current_directory_;
    std::string file_filter_;
    std::vector<std::string> files_;
    std::vector<std::string> directories_;
    int selected_index_;
    char path_buffer_[512];
    bool show_hidden_;
};

} // namespace widgets
} // namespace gui
} // namespace itmoloops
