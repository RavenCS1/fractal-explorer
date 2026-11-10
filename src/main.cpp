#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <stdlib.h>

int main()
{
    sf::Vector2u size{800, 600};
    sf::RenderWindow window(sf::VideoMode{size}, "Fractal Explorer");
    if (!ImGui::SFML::Init(window))
        return 1;

    sf::Texture texture(size);

    int iterations = 100;
    sf::Clock clock;
    bool running = true;

    while (running)
    {
        std::optional<sf::Event> event;
        while ((event = window.pollEvent()))
        {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                running = false;
            if (auto* resized = event->getIf<sf::Event::Resized>())
                std::ignore = texture.resize(resized->size);
        }

        ImGui::SFML::Update(window, clock.restart());
        ImGui::SFML::Render(window);

        window.clear(sf::Color::Black);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return EXIT_SUCCESS;
}
