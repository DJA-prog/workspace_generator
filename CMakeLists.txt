cmake_minimum_required(VERSION 3.0)
project(workspace_generator)

# Add the include directories and link the library
include_directories("libs")
# include_directories("fuctions")

add_subdirectory(src)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
