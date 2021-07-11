cmake_minimum_required(VERSION 3.19)
project(RISC_Console)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)

add_executable(RISC_Console
        source/devices/cpu/core/core.cpp

        source/ui/window.cpp

        source/main.cpp
)

#add_definitions("-DDEBUG")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/ImGui ${CMAKE_CURRENT_BINARY_DIR}/external/ImGui)

target_include_directories(RISC_Console PUBLIC include ${FMT_INCLUDE_DIRS})
target_link_libraries(RISC_Console PUBLIC fmt imgui)