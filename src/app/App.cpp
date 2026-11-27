#include "app/App.hpp"

#include "complex/JuliaRenderer.hpp"
#include "complex/MandelbrotRenderer.hpp"
#include "core/ImageIO.hpp"
#include "core/ParseError.hpp"
#include "ifs/IfsRenderer.hpp"
#include "lsystem/LSystemRenderer.hpp"

#include <ImGuiFileDialog.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <utility>

namespace {

const char* const kDlgSaveImage = "SaveImage";
const char* const kDlgLoadDef = "LoadDefinition";
const char* const kDlgSaveDef = "SaveDefinition";

void openFileDialog(const char* key, const char* title, const char* filter, bool saving) {
    IGFD::FileDialogConfig config;
    config.path = ".";
    config.flags = ImGuiFileDialogFlags_Modal | (saving ? ImGuiFileDialogFlags_ConfirmOverwrite : 0);
    ImGuiFileDialog::Instance()->OpenDialog(key, title, filter, config);
}

// Default ImGui font lacks extended Latin glyphs — try loading a system font.
void loadFonts() {
    // ASCII, Latin-1, Latin Extended-A (ąćęłńóśźż); ImGui keeps a pointer to this, so it must be static.
    static constexpr std::array<ImWchar, 3> kGlyphRanges = {0x0020, 0x017F, 0};
    constexpr std::array<const char*, 4> kCandidates = {
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
    };
    for (const char* path : kCandidates) {
        if (!std::filesystem::exists(path))
            continue;
        const ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        if (io.Fonts->AddFontFromFileTTF(path, 17.f, nullptr, kGlyphRanges.data()) != nullptr &&
            ImGui::SFML::UpdateFontTexture())
            return;
        io.Fonts->Clear(); // failed — fall back to default
        io.Fonts->AddFontDefault();
        (void)ImGui::SFML::UpdateFontTexture();
        return;
    }
}

} // namespace

App::App() : window_(sf::VideoMode({1280u, 800u}), "Fractal Explorer") {
    window_.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window_))
        throw std::runtime_error("Failed to initialize ImGui-SFML");
    loadFonts();

    // Order here matches the "Mode" combo box in the panel.
    auto mandelbrot = std::make_unique<MandelbrotRenderer>();
    auto julia = std::make_unique<JuliaRenderer>();
    // Right click on the Mandelbrot set -> show the Julia set for the clicked c.
    // Capturing `this` is safe: App owns the renderers and cannot be copied or moved.
    const int juliaIndex = 1;
    mandelbrot->onPickC = [this, juliaPtr = julia.get(), juliaIndex](double re, double im) {
        juliaPtr->setC(re, im);
        setMode(juliaIndex);
    };
    renderers_.push_back(std::move(mandelbrot));
    renderers_.push_back(std::move(julia));
    renderers_.push_back(std::make_unique<IfsRenderer>());
    renderers_.push_back(std::make_unique<LSystemRenderer>());
}

App::~App() {
    ImGui::SFML::Shutdown();
}

void App::run() {
    while (window_.isOpen()) {
        processEvents();
        ImGui::SFML::Update(window_, frameClock_.restart());

        drawUi();
        if (current().tick())
            dirty_ = true;
        updateImage();

        window_.clear(sf::Color(30, 30, 30));
        if (!image_.empty()) {
            sf::Sprite sprite(texture_);
            sprite.setPosition({0.f, static_cast<float>(areaTop_)});
            window_.draw(sprite);
        }
        ImGui::SFML::Render(window_);
        window_.display();
    }
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------

bool App::insideArea(sf::Vector2i windowPos) const {
    return windowPos.x >= 0 && windowPos.x < areaWidth_ && windowPos.y >= areaTop_ &&
           windowPos.y < areaTop_ + areaHeight_;
}

sf::Vector2i App::toAreaPixel(sf::Vector2i windowPos) const {
    return {windowPos.x, windowPos.y - areaTop_};
}

void App::processEvents() {
    while (const std::optional event = window_.pollEvent()) {
        ImGui::SFML::ProcessEvent(window_, *event);

        if (event->is<sf::Event::Closed>()) {
            window_.close();
        } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            // Without this SFML would stretch the image; we recalculate at the new size.
            const sf::Vector2f size(resized->size);
            window_.setView(sf::View(sf::FloatRect({0.f, 0.f}, size)));
        } else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
            onMouseWheel(*wheel);
        } else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            onMousePressed(*pressed);
        } else if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
            onMouseMoved(*moved);
        } else if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
            onMouseReleased(*released);
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            onKeyPressed(*key);
        } else if (event->is<sf::Event::MouseLeft>()) {
            cursor_ = {-1, -1};
        }
    }
}

void App::onMouseWheel(const sf::Event::MouseWheelScrolled& wheel) {
    if (ImGui::GetIO().WantCaptureMouse || !insideArea(wheel.position))
        return;
    const sf::Vector2i p = toAreaPixel(wheel.position);
    const double factor = std::pow(1.25, static_cast<double>(wheel.delta));
    dirty_ |= current().onZoom(factor, p.x, p.y);
}

void App::onMousePressed(const sf::Event::MouseButtonPressed& pressed) {
    if (ImGui::GetIO().WantCaptureMouse || !insideArea(pressed.position))
        return;
    const sf::Vector2i p = toAreaPixel(pressed.position);
    switch (pressed.button) {
    case sf::Mouse::Button::Left:
        // A left click is only reported on release, if the mouse barely moved (see onMouseReleased).
        dragging_ = true;
        dragDistance_ = 0;
        lastMouse_ = pressed.position;
        break;
    case sf::Mouse::Button::Right:
        dirty_ |= current().onClick(MouseButton::Right, p.x, p.y);
        break;
    case sf::Mouse::Button::Middle:
        dirty_ |= current().onClick(MouseButton::Middle, p.x, p.y);
        break;
    default:
        break;
    }
}

void App::onMouseMoved(const sf::Event::MouseMoved& moved) {
    cursor_ = moved.position;
    if (!dragging_)
        return;
    const sf::Vector2i d = moved.position - lastMouse_;
    if (d.x == 0 && d.y == 0)
        return;
    dirty_ |= current().onDrag(d.x, d.y);
    dragDistance_ += std::abs(d.x) + std::abs(d.y);
    lastMouse_ = moved.position;
}

void App::onMouseReleased(const sf::Event::MouseButtonReleased& released) {
    if (released.button != sf::Mouse::Button::Left || !dragging_)
        return;
    dragging_ = false;
    if (dragDistance_ < kClickMaxDistance && insideArea(released.position)) {
        const sf::Vector2i p = toAreaPixel(released.position);
        dirty_ |= current().onClick(MouseButton::Left, p.x, p.y);
    }
}

// Keyboard navigation (for laptops without a mouse):
//   + / -  zoom in / out at the center of the fractal area
//   arrows move the view, R or Home resets it
void App::onKeyPressed(const sf::Event::KeyPressed& key) {
    if (ImGui::GetIO().WantCaptureKeyboard) // typing into a panel field
        return;
    const int cx = areaWidth_ / 2;
    const int cy = areaHeight_ / 2;
    using K = sf::Keyboard::Key;
    switch (key.code) {
    case K::Add:
    case K::Equal: // the '+' key without Shift on most layouts
        dirty_ |= current().onZoom(kKeyZoomFactor, cx, cy);
        break;
    case K::Subtract:
    case K::Hyphen:
        dirty_ |= current().onZoom(1.0 / kKeyZoomFactor, cx, cy);
        break;
    // Arrow = move the *view* that way, i.e. drag the image the opposite way.
    case K::Left:
        dirty_ |= current().onDrag(kKeyPanPixels, 0);
        break;
    case K::Right:
        dirty_ |= current().onDrag(-kKeyPanPixels, 0);
        break;
    case K::Up:
        dirty_ |= current().onDrag(0, kKeyPanPixels);
        break;
    case K::Down:
        dirty_ |= current().onDrag(0, -kKeyPanPixels);
        break;
    case K::R:
    case K::Home:
        current().resetView();
        dirty_ = true;
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Image
// ---------------------------------------------------------------------------

void App::updateImage() {
    const int w = std::max(1, areaWidth_);
    const int h = std::max(1, areaHeight_);

    if (image_.width() != w || image_.height() != h) {
        image_.resize(w, h);
        if (!texture_.resize({static_cast<unsigned>(w), static_cast<unsigned>(h)}))
            throw std::runtime_error("Failed to create texture");
        dirty_ = true;
    }
    if (!dirty_)
        return;

    const auto start = std::chrono::steady_clock::now();
    current().render(image_);
    lastRenderMs_ = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();

    texture_.update(image_.bytes());
    dirty_ = false;
}

void App::setMode(int index) {
    if (index == mode_)
        return;
    mode_ = index;
    dirty_ = true;
}

// ---------------------------------------------------------------------------
// UI
// ---------------------------------------------------------------------------

void App::drawUi() {
    const sf::Vector2u win = window_.getSize();
    const float barHeight = ImGui::GetFrameHeight();

    areaTop_ = static_cast<int>(barHeight);
    areaWidth_ = std::max(1, static_cast<int>(static_cast<float>(win.x) - kPanelWidth));
    areaHeight_ = std::max(1, static_cast<int>(static_cast<float>(win.y) - (2.f * barHeight)));

    drawMenuBar();
    drawPanel();
    drawStatusBar();
    drawFileDialogs();
    drawErrorPopup();
}

void App::drawMenuBar() {
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File")) {
        const bool hasDefs = current().fileFilter() != nullptr;
        if (ImGui::MenuItem("Load definition...", nullptr, false, hasDefs))
            openFileDialog(kDlgLoadDef, "Load definition", current().fileFilter(), false);
        if (ImGui::MenuItem("Save definition...", nullptr, false, hasDefs))
            openFileDialog(kDlgSaveDef, "Save definition", current().fileFilter(), true);
        ImGui::Separator();
        if (ImGui::MenuItem("Save image..."))
            openFileDialog(kDlgSaveImage, "Save image", ".png,.bmp", true);
        ImGui::Separator();
        if (ImGui::MenuItem("Quit"))
            window_.close();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Reset view")) {
            current().resetView();
            dirty_ = true;
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About"))
            showAbout_ = true;
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (showAbout_) {
        ImGui::OpenPopup("About");
        showAbout_ = false;
    }
    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Fractal Explorer");
        ImGui::Text("Jakub Piela, Marcel Gladysz, Patryk Pilch");
        ImGui::Text("User Interfaces and Graphics Libraries, AGH WFiIS");
        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void App::drawPanel() {
    const sf::Vector2u win = window_.getSize();
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(win.x) - kPanelWidth, static_cast<float>(areaTop_)));
    ImGui::SetNextWindowSize(ImVec2(kPanelWidth, static_cast<float>(areaHeight_)));
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Control panel", nullptr, flags)) {
        int mode = mode_;
        if (ImGui::BeginCombo("Mode", current().name())) {
            for (int i = 0; i < static_cast<int>(renderers_.size()); ++i) {
                if (ImGui::Selectable(renderers_[static_cast<std::size_t>(i)]->name(), i == mode_))
                    mode = i;
            }
            ImGui::EndCombo();
        }
        setMode(mode);

        ImGui::Separator();
        ImGui::PushID(mode_); // separate widget IDs per mode
        if (current().drawControls())
            dirty_ = true;
        ImGui::PopID();

        ImGui::Separator();
        if (ImGui::Button("Save image"))
            openFileDialog(kDlgSaveImage, "Save image", ".png,.bmp", true);
    }
    ImGui::End();
}

void App::drawStatusBar() {
    const sf::Vector2u win = window_.getSize();
    const float h = ImGui::GetFrameHeight();
    ImGui::SetNextWindowPos(ImVec2(0.f, static_cast<float>(win.y) - h));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(win.x), h));
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 2.f));
    if (ImGui::Begin("##status", nullptr, flags)) {
        if (insideArea(cursor_)) {
            const sf::Vector2i p = toAreaPixel(cursor_);
            const std::string text = current().statusText(p.x, p.y);
            if (!text.empty()) {
                ImGui::TextUnformatted(text.c_str());
                ImGui::SameLine(0.f, 24.f);
            }
        }
        ImGui::Text("render time: %.0f ms", lastRenderMs_);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void App::drawFileDialogs() {
    ImGuiFileDialog* dlg = ImGuiFileDialog::Instance();
    const ImVec2 minSize(600.f, 400.f);
    const ImVec2 maxSize(static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y));

    if (dlg->Display(kDlgSaveImage, ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
        if (dlg->IsOk()) {
            try {
                saveImage(image_, dlg->GetFilePathName());
            } catch (const std::exception& e) {
                errorMessage_ = e.what();
            }
        }
        dlg->Close();
    }
    if (dlg->Display(kDlgLoadDef, ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
        if (dlg->IsOk()) {
            try {
                current().loadDefinition(dlg->GetFilePathName());
                dirty_ = true;
            } catch (const std::exception& e) {
                errorMessage_ = e.what();
            }
        }
        dlg->Close();
    }
    if (dlg->Display(kDlgSaveDef, ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
        if (dlg->IsOk()) {
            try {
                current().saveDefinition(dlg->GetFilePathName());
            } catch (const std::exception& e) {
                errorMessage_ = e.what();
            }
        }
        dlg->Close();
    }
}

void App::drawErrorPopup() {
    if (!errorMessage_.empty() && !ImGui::IsPopupOpen("Error"))
        ImGui::OpenPopup("Error");

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted(errorMessage_.c_str());
        if (ImGui::Button("OK")) {
            errorMessage_.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
