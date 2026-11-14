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
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

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

} // namespace

App::App()
    : window_(sf::VideoMode({1280u, 800u}), "Eksplorator fraktali") {
    window_.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window_))
        throw std::runtime_error("Nie udalo sie zainicjalizowac ImGui-SFML");
    loadFonts();

    // Kolejnosc = kolejnosc na liscie "Tryb" w panelu.
    renderers_.push_back(std::make_unique<MandelbrotRenderer>());
    renderers_.push_back(std::make_unique<JuliaRenderer>());
    renderers_.push_back(std::make_unique<IfsRenderer>());
    renderers_.push_back(std::make_unique<LSystemRenderer>());
}

App::~App() { ImGui::SFML::Shutdown(); }

// Domyslna czcionka ImGui nie ma polskich znakow - probujemy wczytac systemowa.
void App::loadFonts() {
    static const ImWchar ranges[] = {0x0020, 0x017F, 0}; // ASCII, Latin-1, Latin Extended-A (ąćęłńóśźż)
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
    };
    for (const char* path : candidates) {
        if (!std::filesystem::exists(path)) continue;
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        if (io.Fonts->AddFontFromFileTTF(path, 17.f, nullptr, ranges) && ImGui::SFML::UpdateFontTexture())
            return;
        io.Fonts->Clear(); // nieudane - wracamy do domyslnej
        io.Fonts->AddFontDefault();
        (void)ImGui::SFML::UpdateFontTexture();
        return;
    }
}

void App::run() {
    while (window_.isOpen()) {
        processEvents();
        ImGui::SFML::Update(window_, frameClock_.restart());

        drawUi();
        if (current().tick()) dirty_ = true;
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
// Zdarzenia
// ---------------------------------------------------------------------------

bool App::insideArea(sf::Vector2i p) const {
    return p.x >= 0 && p.x < areaWidth_ && p.y >= areaTop_ && p.y < areaTop_ + areaHeight_;
}

sf::Vector2i App::toAreaPixel(sf::Vector2i p) const { return {p.x, p.y - areaTop_}; }

void App::processEvents() {
    while (const std::optional event = window_.pollEvent()) {
        ImGui::SFML::ProcessEvent(window_, *event);

        if (event->is<sf::Event::Closed>()) {
            window_.close();
            continue;
        }
        if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            // Bez tego SFML rozciagalby obraz; my przeliczamy go w nowym rozmiarze.
            const sf::Vector2f size(resized->size);
            window_.setView(sf::View(sf::FloatRect({0.f, 0.f}, size)));
            continue;
        }

        const bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse;

        if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
            if (!imguiWantsMouse && insideArea(wheel->position)) {
                const sf::Vector2i p = toAreaPixel(wheel->position);
                const double factor = std::pow(1.25, static_cast<double>(wheel->delta));
                dirty_ |= current().onZoom(factor, p.x, p.y);
            }
        } else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (imguiWantsMouse || !insideArea(pressed->position)) continue;
            const sf::Vector2i p = toAreaPixel(pressed->position);
            if (pressed->button == sf::Mouse::Button::Left) {
                dragging_ = true;
                dragDistance_ = 0;
                lastMouse_ = pressed->position;
            } else if (pressed->button == sf::Mouse::Button::Right) {
                dirty_ |= current().onClick(MouseButton::Right, p.x, p.y);
            } else if (pressed->button == sf::Mouse::Button::Middle) {
                dirty_ |= current().onClick(MouseButton::Middle, p.x, p.y);
            }
        } else if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
            cursor_ = moved->position;
            if (dragging_) {
                const sf::Vector2i d = moved->position - lastMouse_;
                if (d.x != 0 || d.y != 0) {
                    dirty_ |= current().onDrag(d.x, d.y);
                    dragDistance_ += std::abs(d.x) + std::abs(d.y);
                    lastMouse_ = moved->position;
                }
            }
        } else if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (released->button == sf::Mouse::Button::Left && dragging_) {
                dragging_ = false;
                if (dragDistance_ < 3 && insideArea(released->position)) {
                    const sf::Vector2i p = toAreaPixel(released->position);
                    dirty_ |= current().onClick(MouseButton::Left, p.x, p.y);
                }
            }
        } else if (event->is<sf::Event::MouseLeft>()) {
            cursor_ = {-1, -1};
        }
    }
}

// ---------------------------------------------------------------------------
// Obraz
// ---------------------------------------------------------------------------

void App::updateImage() {
    const int w = std::max(1, areaWidth_);
    const int h = std::max(1, areaHeight_);

    if (image_.width() != w || image_.height() != h) {
        image_.resize(w, h);
        if (!texture_.resize({static_cast<unsigned>(w), static_cast<unsigned>(h)}))
            throw std::runtime_error("Nie udalo sie utworzyc tekstury");
        dirty_ = true;
    }
    if (!dirty_) return;

    const auto start = std::chrono::steady_clock::now();
    current().render(image_);
    lastRenderMs_ = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();

    texture_.update(image_.bytes());
    dirty_ = false;
}

void App::setMode(int index) {
    if (index == mode_) return;
    mode_ = index;
    dirty_ = true;
}

// ---------------------------------------------------------------------------
// Interfejs
// ---------------------------------------------------------------------------

void App::drawUi() {
    const sf::Vector2u win = window_.getSize();
    const float barHeight = ImGui::GetFrameHeight();

    areaTop_ = static_cast<int>(barHeight);
    areaWidth_ = std::max(1, static_cast<int>(static_cast<float>(win.x) - kPanelWidth));
    areaHeight_ = std::max(1, static_cast<int>(static_cast<float>(win.y) - 2.f * barHeight));

    drawMenuBar();
    drawPanel();
    drawStatusBar();
    drawFileDialogs();
    drawErrorPopup();
}

void App::drawMenuBar() {
    if (!ImGui::BeginMainMenuBar()) return;

    if (ImGui::BeginMenu("Plik")) {
        const bool hasDefs = current().fileFilter() != nullptr;
        if (ImGui::MenuItem("Wczytaj definicję...", nullptr, false, hasDefs))
            openFileDialog(kDlgLoadDef, "Wczytaj definicję", current().fileFilter(), false);
        if (ImGui::MenuItem("Zapisz definicję...", nullptr, false, hasDefs))
            openFileDialog(kDlgSaveDef, "Zapisz definicję", current().fileFilter(), true);
        ImGui::Separator();
        if (ImGui::MenuItem("Zapisz obraz..."))
            openFileDialog(kDlgSaveImage, "Zapisz obraz", ".png,.bmp", true);
        ImGui::Separator();
        if (ImGui::MenuItem("Zakończ")) window_.close();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Widok")) {
        if (ImGui::MenuItem("Widok startowy")) {
            current().resetView();
            dirty_ = true;
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Pomoc")) {
        if (ImGui::MenuItem("O programie")) showAbout_ = true;
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (showAbout_) {
        ImGui::OpenPopup("O programie");
        showAbout_ = false;
    }
    if (ImGui::BeginPopupModal("O programie", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Eksplorator fraktali");
        ImGui::Text("Jakub Piela, Marcel Gładysz, Patryk Pilch");
        ImGui::Text("Interfejsy użytkownika i biblioteki graficzne, AGH WFiIS");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void App::drawPanel() {
    const sf::Vector2u win = window_.getSize();
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(win.x) - kPanelWidth, static_cast<float>(areaTop_)));
    ImGui::SetNextWindowSize(ImVec2(kPanelWidth, static_cast<float>(areaHeight_)));
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Panel sterowania", nullptr, flags)) {
        int mode = mode_;
        if (ImGui::BeginCombo("Tryb", current().name())) {
            for (int i = 0; i < static_cast<int>(renderers_.size()); ++i) {
                if (ImGui::Selectable(renderers_[static_cast<std::size_t>(i)]->name(), i == mode_)) mode = i;
            }
            ImGui::EndCombo();
        }
        setMode(mode);

        ImGui::Separator();
        ImGui::PushID(mode_); // osobne ID widzetow dla kazdego trybu
        if (current().drawControls()) dirty_ = true;
        ImGui::PopID();

        ImGui::Separator();
        if (ImGui::Button("Zapisz obraz"))
            openFileDialog(kDlgSaveImage, "Zapisz obraz", ".png,.bmp", true);
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
        ImGui::Text("czas obliczeń: %.0f ms", lastRenderMs_);
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
            } catch (const std::exception& e) { // ParseError dziedziczy po std::runtime_error
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
    if (!errorMessage_.empty() && !ImGui::IsPopupOpen("Błąd"))
        ImGui::OpenPopup("Błąd");

    if (ImGui::BeginPopupModal("Błąd", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted(errorMessage_.c_str());
        if (ImGui::Button("OK")) {
            errorMessage_.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
