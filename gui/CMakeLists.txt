cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)

add_executable(itmoloops_gui
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    syntaxhighlighter.cpp
    syntaxhighlighter.hpp
    waveformwidget.cpp
    waveformwidget.hpp
)

target_link_libraries(itmoloops_gui PRIVATE
    itmoloops_lib
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

target_include_directories(itmoloops_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
