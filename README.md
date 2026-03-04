# HereBeRobots
Lightweight 2D SLAM and navigation stack for mobile robots using lidar, occupancy grids, and probabilistic state estimation.

steps for installing raylib

sudo apt update
sudo apt install build-essential git cmake libx11-dev libxcursor-dev libxrandr-dev libxi-dev libgl1-mesa-dev libwayland-dev
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build
cd build
cmake ..
make -j4
sudo make install

Verify installation
pkg-config --libs raylib

Expected output:
-lraylib -lm -ldl -lpthread -lGL -lrt -lX11

Force software OpenGL (most reliable in containers)
export LIBGL_ALWAYS_SOFTWARE=1
