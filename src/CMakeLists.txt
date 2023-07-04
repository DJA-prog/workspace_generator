cmake_minimum_required(VERSION 3.10)
project(workspace_generator)

# Find Boost
find_package(Boost REQUIRED COMPONENTS filesystem system)

# Add an executable
add_executable(main main.cpp)
set_target_properties(main PROPERTIES OUTPUT_NAME "workspace-gen")
target_link_libraries(main PRIVATE Boost::filesystem Boost::system)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)
