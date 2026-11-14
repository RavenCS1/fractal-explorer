#pragma once

#include "core/FractalRenderer.hpp"
#include "core/Image.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <string>
#include <vector>

// Window, event loop, ImGui panel, and module wiring.
// Owner: Jakub. Others only add their renderer in the App constructor.
class App {
public:
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();

private:
    void processEvents();
    void drawUi();
    void drawMenuBar();
    void drawPanel();
    void drawStatusBar();
    void drawFileDialogs();
    void drawErrorPopup();
    void updateImage();

    void setMode(int index);
    FractalRenderer& current() { return *renderers_[static_cast<std::size_t>(mode_)]; }
    bool insideArea(sf::Vector2i windowPos) const;
    sf::Vector2i toAreaPixel(sf::Vector2i windowPos) const;
    void loadFonts();

    sf::RenderWindow window_;
    sf::Clock frameClock_;

    std::vector<std::unique_ptr<FractalRenderer>> renderers_;
    int mode_ = 0;

    Image image_;
    sf::Texture texture_;
    bool dirty_ = true; // need to call render()
    double lastRenderMs_ = 0.0;

    // Fractal area in the window (computed each frame in drawUi()).
    int areaTop_ = 0;
    int areaWidth_ = 1;
    int areaHeight_ = 1;

    // Mouse
    bool dragging_ = false;
    int dragDistance_ = 0;
    sf::Vector2i lastMouse_{0, 0};
    sf::Vector2i cursor_{-1, -1}; // position in window

    std::string errorMessage_; // non-empty = show error popup
    bool showAbout_ = false;

    static constexpr float kPanelWidth = 340.f;
};
