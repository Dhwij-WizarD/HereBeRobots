#!/bin/bash
set -e

CLEAN=false
TEST=false
BUILD_TYPE=""

for arg in "$@"; do
    case "$arg" in
        --clean) CLEAN=true ;;
        --test)  TEST=true ;;
        *)       BUILD_TYPE="$arg" ;;
    esac
done

if [ -z "$BUILD_TYPE" ]; then
    BUILD_TYPE="Release"
fi

if [ "$CLEAN" = true ]; then
    echo "Cleaning previous build..."
    rm -rf build install log
fi

if [ "$TEST" = true ]; then
    BUILD_TESTING="ON"
else
    BUILD_TESTING="OFF"
fi

# ament_uncrustify --reformat
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DBUILD_TESTING="$BUILD_TESTING" 2>&1 | tee build.log

if [ "$TEST" = true ]; then
    colcon test | tee test.log
    colcon test-result --verbose | tee test-result.log
fi

echo "$BUILD_TYPE"
