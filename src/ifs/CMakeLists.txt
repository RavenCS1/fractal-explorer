add_library(fe_ifs STATIC
    IfsRenderer.cpp
)
target_link_libraries(fe_ifs PUBLIC fe_core PRIVATE ImGui-SFML::ImGui-SFML)
fe_setup_target(fe_ifs)
