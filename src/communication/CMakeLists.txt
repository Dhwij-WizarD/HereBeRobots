cmake_minimum_required(VERSION 3.8)
project(communication)

# Enable additional compiler warnings for GCC/Clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

# Find ament and other dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(diagnostics REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(include/)

set(SOURCES
  src/RosCommunicatorApp.cpp
  src/Communicator.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  diagnostics
)

install(
  TARGETS
    ${PROJECT_NAME}
  DESTINATION
    lib/${PROJECT_NAME}
)

# -------------------------------------------------------------------
# Pub/Sub integration test executables
# -------------------------------------------------------------------
foreach(app pub_app sub_app)
  add_executable(${app} test/${app}.cpp)
  target_include_directories(${app} PRIVATE include/)
  ament_target_dependencies(${app} rclcpp std_msgs diagnostics)
  target_link_libraries(${app} ${PROJECT_NAME})
  install(TARGETS ${app} DESTINATION lib/${PROJECT_NAME})
endforeach()

# -------------------------------------------------------------------
# Testing and Linting (if enabled)
# -------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # Optionally skip some checks:
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(std_srvs REQUIRED)

  ament_add_gtest(test_ros_communicator_app
    test/test_ros_communicator_app.cpp
  )
  target_include_directories(test_ros_communicator_app PRIVATE include/)
  ament_target_dependencies(test_ros_communicator_app
    rclcpp
    std_msgs
    std_srvs
    diagnostics
  )
  target_link_libraries(test_ros_communicator_app
    ${PROJECT_NAME}
  )
endif()

# Export the runtime dependencies for the generated messages.
# ament_export_dependencies(rosidl_default_runtime)

ament_package()
