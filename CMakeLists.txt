cmake_minimum_required(VERSION 3.10.0)
project(p4 VERSION 0.1.0 LANGUAGES C CXX)

if (MSVC)
    add_compile_options(/W3)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
    add_compile_options(/utf-8)
    add_compile_options(/O2)
else()
    add_compile_options(-Wall)
    add_compile_options(-std=c++20)
    add_compile_options(-fexceptions)
    add_compile_options(-O3)
    find_package(Threads REQUIRED)
endif()

add_executable(p4
    program.cpp
    data.cpp
)

if (NOT MSVC)
    target_link_libraries(p4 Threads::Threads)
endif()
