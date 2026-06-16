cmake_minimum_required(VERSION 3.8)
project(motor_msgs)

# Enable additional compiler warnings for GCC/Clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

# Find ament and other dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_srvs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)  # Needed for generating interfaces

# -------------------------------------------------------------------
# Message Generation
# -------------------------------------------------------------------

# List your custom message files here (ensure these files exist in the 'msg' folder)
set(MSG_FILES
  "msg/Feedback.msg"
  "msg/FeedbackStamped.msg"
  "msg/FeedbackArrayStamped.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${MSG_FILES}
  DEPENDENCIES builtin_interfaces std_msgs geometry_msgs
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rosidl_generator_cpp/
  DESTINATION share/${PROJECT_NAME}/rosidl_generator_cpp/
)

# -------------------------------------------------------------------
# Testing and Linting (if enabled)
# -------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # Optionally skip some checks:
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Export the runtime dependencies for the generated messages.
# ament_export_dependencies(rosidl_default_runtime)

ament_package()
