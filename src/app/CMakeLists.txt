add_executable(fractal-explorer
    main.cpp
    App.cpp
)
target_link_libraries(fractal-explorer PRIVATE
    fe_core
    fe_complex
    fe_ifs
    fe_lsystem
    SFML::Graphics
    ImGui-SFML::ImGui-SFML
    ImGuiFileDialog)
fe_setup_target(fractal-explorer)

# Output binary in build/bin (on Windows: build/bin/Release etc.)
set_target_properties(fractal-explorer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
