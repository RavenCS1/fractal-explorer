add_library(fe_complex STATIC
    MandelbrotRenderer.cpp
    JuliaRenderer.cpp
)
target_link_libraries(fe_complex PUBLIC fe_core PRIVATE ImGui-SFML::ImGui-SFML)
fe_setup_target(fe_complex)
