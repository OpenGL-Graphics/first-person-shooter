cmake_minimum_required(VERSION 3.10)
project(VIDEO)

# glfw
find_package(glfw3 REQUIRED)

# glad shared library
add_library(glad SHARED src/glad.c)
target_include_directories(glad PUBLIC include)

# main executable
add_executable(video src/main.c)
target_link_libraries(video glfw glad)
