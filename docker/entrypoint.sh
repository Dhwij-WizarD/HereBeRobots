#!/bin/bash
# =============================================================================
# entrypoint.sh — shared by both Gazebo and sim2d containers
# Sources ROS2 Jazzy + workspace install, then runs the provided command
# =============================================================================
set -e

source /opt/ros/jazzy/setup.bash

# Source the workspace if it has been built
if [ -f /ros2_ws/install/setup.bash ]; then
    source /ros2_ws/install/setup.bash
fi

exec "$@"