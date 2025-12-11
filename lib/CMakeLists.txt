set(CMAKE_CXX_STANDARD 20)

add_library(itmoloops_lib
        lib.cpp
        lib.hpp
        types.hpp
        utils.hpp
        utils.cpp
        effects.hpp
        effects.cpp
        instruments.hpp
        instruments.cpp
        parser.hpp
        parser.cpp
)

target_include_directories(itmoloops_lib PUBLIC .)