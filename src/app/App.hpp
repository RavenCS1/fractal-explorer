#pragma once

/**
 * @file
 * The application window and main loop.
 */

#include "core/FractalRenderer.hpp"
#include "core/Image.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>
#include <string>
#include <vector>

/**
 * Window, event loop, ImGui panel, and module wiring.
 *
 * Owns one FractalRenderer per mode and forwards input to the active one. Each frame:
 * process events -> draw the UI -> tick() -> render() if something changed -> show the image.
 *
 * Owner: Jakub. Others only add their renderer in the App constructor.
 */
class App {
public:
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    /// Runs the main loop until the window is closed.
    void run();

private:
    void processEvents();
    void onMouseWheel(const sf::Event::MouseWheelScrolled& wheel);
    void onMousePressed(const sf::Event::MouseButtonPressed& pressed);
    void onMouseMoved(const sf::Event::MouseMoved& moved);
    void onMouseReleased(const sf::Event::MouseButtonReleased& released);
    void onKeyPressed(const sf::Event::KeyPressed& key);
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

    sf::RenderWindow window_;
    sf::Clock frameClock_;

    std::vector<std::unique_ptr<FractalRenderer>> renderers_;
    int mode_ = 0;

    Image image_;
    sf::Texture texture_;
    bool dirty_ = true; ///< The image must be recalculated (render()) before it is shown.
    double lastRenderMs_ = 0.0;

    // Fractal area in the window (computed each frame in drawUi()).
    int areaTop_ = 0;
    int areaWidth_ = 1;
    int areaHeight_ = 1;

    // Mouse
    bool dragging_ = false;
    int dragDistance_ = 0;
    sf::Vector2i lastMouse_{0, 0};
    sf::Vector2i cursor_{-1, -1}; ///< Mouse position in the window; (-1, -1) when outside.

    std::string errorMessage_; // non-empty = show error popup
    bool showAbout_ = false;

    static constexpr float kPanelWidth = 340.f;
    static constexpr double kKeyZoomFactor = 1.25; ///< One +/- key press zooms like one mouse wheel notch.
    static constexpr int kKeyPanPixels = 50;       ///< One arrow key press pans by this many pixels.
    static constexpr int kClickMaxDistance = 3;    // px; a left press that moves less than this is a click
};
