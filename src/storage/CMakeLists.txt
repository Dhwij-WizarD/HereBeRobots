cmake_minimum_required(VERSION 3.8)
project(storage)

# Enable additional compiler warnings for GCC/Clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

# Find ament dependencies
find_package(ament_cmake REQUIRED)

include_directories(include/)

set(SOURCES
  src/FileIO.cpp
)

# -------------------------------------------------------------------
# 1. Build the Storage Library Target
# -------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES})

install(
  TARGETS
    ${PROJECT_NAME}
  DESTINATION
    lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include/
)

# -------------------------------------------------------------------
# 2. Storage Application / Sample Executables
# (Ensure app files like src/file_io_app.cpp contain an 'int main()')
# -------------------------------------------------------------------
foreach(app file_io_app)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${app}.cpp")
    add_executable(${app} src/${app}.cpp)
    target_include_directories(${app} PRIVATE include/)
    target_link_libraries(${app} ${PROJECT_NAME})
    install(TARGETS ${app} DESTINATION lib/${PROJECT_NAME})
  endif()
endforeach()

# -------------------------------------------------------------------
# 3. Testing and Linting (if enabled)
# -------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_storage_concepts test/test_storage_concepts.cpp)
  ament_add_gtest(test_file_io test/test_file_io.cpp)

  foreach(test_target test_storage_concepts test_file_io)
    target_include_directories(${test_target} PRIVATE include/)
    target_link_libraries(${test_target} ${PROJECT_NAME})
  endforeach()
endif()

# -------------------------------------------------------------------
# 4. Export Dependencies for Downstream Packages
# -------------------------------------------------------------------
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()