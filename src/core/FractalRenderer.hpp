#pragma once

#include "core/Image.hpp"

#include <string>

enum class MouseButton { Left, Right, Middle };

// Wspolny interfejs wszystkich rodzajow fraktali.
// Aplikacja trzyma liste rendererow i zawsze rozmawia z aktywnym przez te metody.
//
// ZASADY:
//  1. render() tylko liczy i rysuje do Image. Zadnego ImGui ani SFML w srodku.
//  2. drawControls() tylko rysuje widzety i zmienia parametry. Nic nie liczy.
//  3. Metody zwracajace bool zwracaja true, gdy obraz trzeba przeliczyc;
//     aplikacja wtedy wywola render().
//  4. Wspolrzedne pikseli (px, py) sa liczone wzgledem lewego gornego rogu
//     obszaru fraktala, a nie calego okna.
class FractalRenderer {
public:
    virtual ~FractalRenderer() = default;

    // Nazwa trybu w liscie wyboru, np. "Mandelbrot".
    virtual const char* name() const = 0;

    // Rysuje caly obraz. Rozmiar obrazu = rozmiar obszaru fraktala;
    // przy zmianie rozmiaru okna aplikacja podaje obraz w nowym rozmiarze.
    virtual void render(Image& image) = 0;

    // Widzety ImGui w panelu sterowania (parametry, palety, przyciski).
    // Aplikacja wywoluje ja raz na klatke, wewnatrz okna panelu.
    virtual bool drawControls() = 0;

    // Wywolywana raz na klatke petli glownej. Do pracy rozlozonej na wiele klatek,
    // np. generowania animacji: jedna klatka animacji na jedno wywolanie.
    virtual bool tick() { return false; }

    // --- Nawigacja (domyslnie brak) ----------------------------------------
    // factor > 1 przybliza, factor < 1 oddala; (px, py) to punkt pod kursorem.
    virtual bool onZoom(double factor, int px, int py) { (void)factor; (void)px; (void)py; return false; }
    // Przesuniecie myszy o (dx, dy) pikseli przy wcisnietym lewym przycisku.
    virtual bool onDrag(int dx, int dy) { (void)dx; (void)dy; return false; }
    // Klikniecie bez przeciagania.
    virtual bool onClick(MouseButton button, int px, int py) { (void)button; (void)px; (void)py; return false; }
    // Powrot do widoku startowego (menu Widok).
    virtual void resetView() {}

    // Tekst paska stanu dla piksela pod kursorem, np. "x = -0.7436, y = 0.1318".
    virtual std::string statusText(int px, int py) const { (void)px; (void)py; return {}; }

    // --- Pliki definicji ----------------------------------------------------
    // Filtr dla okna wyboru pliku, np. ".ifs". nullptr = tryb nie ma plikow definicji.
    virtual const char* fileFilter() const { return nullptr; }
    // Rzuca ParseError (z numerem wiersza) albo std::runtime_error.
    virtual void loadDefinition(const std::string& path) { (void)path; }
    // Rzuca std::runtime_error przy bledzie zapisu.
    virtual void saveDefinition(const std::string& path) const { (void)path; }
};
