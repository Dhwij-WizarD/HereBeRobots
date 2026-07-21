#!/bin/bash
set -e

echo "Running Script 1..............................................................."
bash /home/rosdev/ros2_ws/src/geometry/GeometryBuild.sh

echo "Running Script 2..............................................................."
bash /home/rosdev/ros2_ws/src/simulator/SimulatorBuild.sh
