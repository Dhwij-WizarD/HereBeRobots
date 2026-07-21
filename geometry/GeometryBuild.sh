#!/bin/bash
set -e
cd /home/rosdev/ros2_ws/src/geometry || exit
rm -rf build/ install/ log/
mkdir build
cd build || exit
clear 
echo "Running Geometry Build..."
cmake .. 
make -j"$(nproc)"
sudo make install
