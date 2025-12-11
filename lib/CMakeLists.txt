set(CMAKE_CXX_STANDARD 20)

add_library(itmoloops_lib lib.cpp)
target_include_directories(itmoloops_lib PUBLIC .)

# GUI library
add_library(itmoloops_gui_lib
    gui/gui_app.cpp
    gui/composition_parser.cpp
    gui/widgets/file_browser.cpp
)

target_include_directories(itmoloops_gui_lib PUBLIC .)
target_link_libraries(itmoloops_gui_lib PUBLIC
    imgui
    OpenGL::GL
    glfw
)
