cmake_minimum_required(VERSION 3.22)
project(Simulator)

# Find raylib
find_package(raylib REQUIRED)

# Add geometry library
find_package(Geometry REQUIRED)

set(SOURCES
    src/entity.cpp
    src/obstacle.cpp
    src/robot.cpp
    src/world.cpp
)

# Interface library for compile flags
add_library(HBR_INTERFACE_SIM INTERFACE)

target_compile_options(HBR_INTERFACE_SIM 
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

add_library(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../geometry/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        HBR_INTERFACE_SIM
        raylib
        Geometry::Geometry
)


add_executable(abcd main.cpp)
target_link_libraries(abcd PRIVATE Simulator)

install(TARGETS ${PROJECT_NAME} abcd
  DESTINATION lib/${PROJECT_NAME}
)