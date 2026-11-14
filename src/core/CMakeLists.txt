add_library(fe_core STATIC
    Image.cpp
    ImageIO.cpp
    LineReader.cpp
)
# Headers are included as "core/Image.hpp", "ifs/IfsRenderer.hpp" etc.
target_include_directories(fe_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(fe_core PRIVATE SFML::Graphics)   # only ImageIO.cpp
fe_setup_target(fe_core)
