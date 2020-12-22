cmake_minimum_required(VERSION 3.10)
project(VIDEO)

# autocomplete with clang in vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# debug with gdb
set(CMAKE_BUILD_TYPE Debug)

# copy video assets folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})


# opengl main
# glfw
find_package(glfw3 REQUIRED)

# glad shared library (opengl)
add_library(glad SHARED src/glad/glad.c)
target_include_directories(glad PUBLIC include)

# imgui
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
file(GLOB IMGUI_SRC "src/imgui/*.cpp")
add_library(imgui SHARED ${IMGUI_SRC})
target_include_directories(imgui PUBLIC include/imgui include)

# main executable
add_executable(main src/main.cpp)
target_link_libraries(main glfw glad imgui)
