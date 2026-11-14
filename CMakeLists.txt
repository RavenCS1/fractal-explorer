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


# Shared settings for our targets (not for external libraries).
function(fe_setup_target target)
    if(MSVC)
        # /utf-8: support Unicode characters in source code string literals
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Each module has its own CMakeLists.txt — when adding files you only edit yours.
add_subdirectory(src/core)      # shared       -> fe_core
add_subdirectory(src/complex)   # Jakub        -> fe_complex
add_subdirectory(src/ifs)       # Marcel       -> fe_ifs
add_subdirectory(src/lsystem)   # Patryk       -> fe_lsystem
add_subdirectory(src/app)       # Jakub        -> fractal-explorer

if(FE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
