#!/bin/bash
set -e
BUILD_TYPE=$1
# === Clean build option ===
if [ "$1" == "--clean" ]; then
    echo "Cleaning previous build..."
    rm -rf build install log
    BUILD_TYPE=$2
fi

if [ "$BUILD_TYPE" == "" ]; then
    BUILD_TYPE="Release"
fi

colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "$BUILD_TYPE"