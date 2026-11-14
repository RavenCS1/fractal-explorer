add_library(fe_lsystem STATIC
    LSystemRenderer.cpp
)
target_link_libraries(fe_lsystem PUBLIC fe_core PRIVATE ImGui-SFML::ImGui-SFML)
fe_setup_target(fe_lsystem)
