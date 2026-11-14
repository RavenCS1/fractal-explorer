# fractal-explorer
Interactive fractal explorer in C++ with SFML and Dear ImGui: Mandelbrot and Julia sets, IFS fractals, L-systems, and IFS morphing animations.

# To get the whole repo
git clone
git submodule update --init --recursive

# To build
mkdir build
cd build
cmake ..
make

# To run
./fractal_explorer

# Resources
sudo apt install build-essential cmake \
  libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
  libgl1-mesa-dev libfreetype-dev libharfbuzz-dev
