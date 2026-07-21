#!/bin/bash
set -e
#Temporary fix for raylib not being found
# sudo apt update
# sudo apt install raylib-dev


cd /home/rosdev/ros2_ws/src/simulator || exit
rm -rf build/ install/ log/
mkdir build
cd build || exit
# clear 
echo "Running Simulator Build..."
cmake .. 
make -j"$(nproc)"
