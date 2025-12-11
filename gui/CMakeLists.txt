cmake_minimum_required(VERSION 3.24)

# Поиск Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Автоматическая обработка MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Исходные файлы GUI
set(GUI_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    syntaxhighlighter.cpp
    syntaxhighlighter.hpp
    waveformwidget.cpp
    waveformwidget.hpp
)

# Создание исполняемого файла
add_executable(itmoloops_gui ${GUI_SOURCES})

# Линковка с Qt6 и библиотекой itmoloops
target_link_libraries(itmoloops_gui 
    PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Gui
    itmoloops_lib
)

# C++20
target_compile_features(itmoloops_gui PRIVATE cxx_std_20)

# Установка
install(TARGETS itmoloops_gui DESTINATION bin)
