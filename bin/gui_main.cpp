#include "../lib/gui/gui_app.hpp"
#include <iostream>

int main(int argc, char** argv) {
    itmoloops::gui::GuiApp app;
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize GUI application" << std::endl;
        return 1;
    }
    
    app.Run();
    app.Shutdown();
    
    return 0;
}
