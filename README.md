# fractal-explorer
Interactive fractal explorer in C++ with SFML and Dear ImGui: Mandelbrot and Julia sets, IFS fractals, L-systems, and IFS morphing animations.

# Building
## Budowanie

```bash
git clone --recurse-submodules <url>
# or in the existing clone of the repository
git submodule update --init --recursive
```

**Linux (Ubuntu/Debian)**: najpierw zależności SFML:

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
  libudev-dev libgl1-mesa-dev libfreetype-dev libharfbuzz-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/fractal-explorer
```

**Windows (Visual Studio 2026)**: *Plik → Otwórz → Folder* i wybierz katalog repozytorium (VS sam wykryje CMake)
albo z wiersza poleceń:

```bat
cmake -S . -B build
cmake --build build --config Release
build\bin\Release\fractal-explorer.exe
```


Testy (bez okna): `ctest --test-dir build --output-on-failure` (na Windowsie dodaj `-C Release`).

## Struktura

| Katalog        | Cel CMake          | Właściciel | Zawartość |
|----------------|--------------------|------------|-----------|
| `src/core`     | `fe_core`          | wspólne    | `Image`, `Color`, `FractalRenderer`, `LineReader`, `ParseError`, `saveImage` |
| `src/app`      | `fractal-explorer` | Jakub      | okno, pętla zdarzeń, menu, panel, pasek stanu, okna dialogowe |
| `src/complex`  | `fe_complex`       | Jakub      | Mandelbrot, Julia |
| `src/ifs`      | `fe_ifs`           | Marcel     | IFS, gra w chaos, animacja przejścia |
| `src/lsystem`  | `fe_lsystem`       | Patryk     | L-systemy, grafika żółwia |
| `tests`        | `core_tests` …     | każdy      | testy bez okna |
| `data`         | –                  | każdy      | przykładowe `.ifs`, `.lsys` |

Każdy moduł ma własny `CMakeLists.txt`. Nowy plik `.cpp` dopisujesz tylko w swoim katalogu.

## Zasady

1. **`FractalRenderer` to kontrakt.** Aplikacja zna tylko ten interfejs. Swój tryb robisz,
   nadpisując jego metody. Zaczynasz od zaślepki w swoim katalogu i podmieniasz jej zawartość.
2. **`render()` tylko liczy.** Nie wywołuje ImGui ani SFML, rysuje wyłącznie do `Image`.
3. **`drawControls()` tylko rysuje widżety.** Zwraca `true`, gdy zmieniły się parametry.
   To samo dotyczy `onZoom`, `onDrag`, `onClick` i `tick`: `true` oznacza, że trzeba przeliczyć obraz.
4. **Błędy w plikach** zgłaszamy przez `ParseError` z numerem wiersza. Czytamy je przez `LineReader`.
   Aplikacja pokazuje komunikat w oknie i nie zamyka się.
5. **`src/core` zmieniamy tylko po uzgodnieniu we trójkę**, bo zmiana dotyczy wszystkich.
6. **Git:** każdy pracuje na swojej gałęzi (`jakub/...`, `marcel/...`, `patryk/...`) i wysyła pull requesty do `main`.
   `main` zawsze się kompiluje.

## Formaty plików

`.ifs`: jedno przekształcenie w wierszu `a b c d e f p`, gdzie x' = ax + by + e, y' = cx + dy + f, a p to prawdopodobieństwo.

`.lsys`:

```
axiom F
rule F=F+F-F-F+F
angle 90
iterations 4
```

W obu formatach `#` zaczyna komentarz.
