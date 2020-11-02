cmake_minimum_required(VERSION 3.10)
project(VIDEO)

# autocomplete with clang in vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# debug with gdb
set(CMAKE_BUILD_TYPE Debug)

# glfw
find_package(glfw3 REQUIRED)

# glad shared library
add_library(glad SHARED src/glad.c)
target_include_directories(glad PUBLIC include)

# main executable
add_executable(video src/main.cpp)
target_link_libraries(video glfw glad)
