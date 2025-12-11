cmake_minimum_required(VERSION 3.24)

project(
    itmoloops
    VERSION 1.0
    DESCRIPTION "C++ Year 2025 labwork5"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

# Попытаться собрать GUI, если доступен Qt6
find_package(Qt6 QUIET COMPONENTS Core Widgets Gui)
if(Qt6_FOUND)
    message(STATUS "Qt6 found, building GUI")
    add_subdirectory(gui)
else()
    message(STATUS "Qt6 not found, skipping GUI build")
endif()
