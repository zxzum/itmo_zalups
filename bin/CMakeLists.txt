add_executable(itmoloops main.cpp)
target_link_libraries(itmoloops PRIVATE itmoloops_lib)

# GUI executable
add_executable(itmoloops_gui gui_main.cpp)
target_link_libraries(itmoloops_gui PRIVATE 
    itmoloops_lib
    itmoloops_gui_lib
    imgui
    OpenGL::GL
    glfw
)
