cmake_minimum_required(VERSION 3.22)
project(fractal-explorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# dependencies as git submodules, pinned at versions:
#   third_party/sfml            -> 3.1.0
#   third_party/imgui           -> v1.91.9b
#   third_party/imgui-sfml      -> v3.0
#   third_party/imguifiledialog -> v0.6.8

set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
add_subdirectory(third_party/sfml EXCLUDE_FROM_ALL)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

set(IMGUI_SFML_FIND_SFML OFF)
add_subdirectory(third_party/imgui-sfml EXCLUDE_FROM_ALL)

add_subdirectory(third_party/imguifiledialog EXCLUDE_FROM_ALL)
target_include_directories(ImGuiFileDialog PUBLIC ${IMGUI_DIR})

add_executable(fractal-explorer src/main.cpp)
target_link_libraries(fractal-explorer PRIVATE
    SFML::Graphics
    ImGui-SFML::ImGui-SFML
    ImGuiFileDialog)
