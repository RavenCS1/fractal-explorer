# fractal-explorer

Interactive fractal explorer in C++ with SFML and Dear ImGui: Mandelbrot and Julia sets, IFS fractals, L-systems, and IFS morphing animations.

## Building

```bash
git clone --recurse-submodules <url>
# or in an existing clone of the repository
git submodule update --init --recursive
```

**Linux (Ubuntu/Debian)** — install SFML dependencies first:

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
  libudev-dev libgl1-mesa-dev libfreetype-dev libharfbuzz-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/fractal-explorer
```

**Windows (Visual Studio 2026)**: *File → Open → Folder* and select the repository directory (VS detects CMake automatically),
or from the command line:

```bat
cmake -S . -B build
cmake --build build --config Release
build\bin\Release\fractal-explorer.exe
```

**Tests** (headless): `ctest --test-dir build --output-on-failure` (on Windows add `-C Release`).

## Project structure

| Directory      | CMake target       | Owner      | Contents |
|----------------|--------------------|------------|----------|
| `src/core`     | `fe_core`          | shared     | `Image`, `Color`, `FractalRenderer`, `LineReader`, `ParseError`, `saveImage` |
| `src/app`      | `fractal-explorer` | Jakub      | window, event loop, menu, panel, status bar, file dialogs |
| `src/complex`  | `fe_complex`       | Jakub      | Mandelbrot, Julia |
| `src/ifs`      | `fe_ifs`           | Marcel     | IFS, chaos game, transition animation |
| `src/lsystem`  | `fe_lsystem`       | Patryk     | L-systems, turtle graphics |
| `tests`        | `core_tests` …     | everyone   | headless tests |
| `data`         | –                  | everyone   | sample `.ifs`, `.lsys` files |

Each module has its own `CMakeLists.txt`. New `.cpp` files go only in your own directory.

## Formatting and static analysis

The project includes `.clang-format` and `.clang-tidy` configuration files in the root directory.

Install the tools (Ubuntu/Debian):

```bash
sudo apt install clang-format clang-tidy
```

**clang-format** — automatic code formatting:

```bash
# Format a single file:
clang-format -i src/app/App.cpp

# Format all project files:
find src tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
```

**clang-tidy** — static analysis (only `src/`, skips `third_party/`):

```bash
# Generate the compilation database (once, or after CMake changes):
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Check a single file:
clang-tidy -p build src/app/App.cpp

# Check the entire project:
find src -name '*.cpp' | xargs clang-tidy -p build
```

Headers from `third_party/` are automatically ignored via `HeaderFilterRegex` in `.clang-tidy`.

> **Note:** clang-tidy ships its own compiler front-end. If you build with GCC and see
> `file not found` errors for project headers, pass the source include path explicitly:
> ```bash
> find src -name '*.cpp' | xargs clang-tidy -p build --extra-arg=-Isrc
> ```

## Rules

1. **`FractalRenderer` is the contract.** The application only knows this interface. You implement your mode by overriding its methods, starting from a stub in your directory.
2. **`render()` only computes.** It must not call ImGui or SFML — it draws exclusively to `Image`.
3. **`drawControls()` only draws widgets.** Returns `true` when parameters changed. The same applies to `onZoom`, `onDrag`, `onClick`, and `tick`: `true` means the image needs to be recalculated.
4. **File errors** are reported via `ParseError` with a line number. Files are read through `LineReader`. The application shows an error dialog and does not crash.
5. **Changes to `src/core`** require agreement from all three team members, since they affect everyone.
6. **Git:** each person works on their own branch (`jakub/...`, `marcel/...`, `patryk/...`) and opens pull requests to `main`. `main` always compiles.

## File formats

`.ifs`: one transformation per line — `a b c d e f p`, where x' = ax + by + e, y' = cx + dy + f, and p is the probability.

`.lsys`:

```
axiom F
rule F=F+F-F-F+F
angle 90
iterations 4
```

In both formats `#` starts a comment.
