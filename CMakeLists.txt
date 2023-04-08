cmake_minimum_required(VERSION 3.10)
project(FPS)

# use C++17 (variants used for uniforms of different types)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb (needed by valgrind too)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# profiling flag for gprof
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# copy assets folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# needed packages
find_package(Freetype REQUIRED)
find_package(assimp REQUIRED)

# fmod library for playing sounds
list(APPEND CMAKE_PREFIX_PATH "./lib")
find_library(LIB_FMOD fmod)

# glfw-window & opengl-utils (texture, shader, & framebuffer) libraries from submodules
add_subdirectory(glfw-window)
add_subdirectory(opengl-utils)

# main executable
file(GLOB SRC
  "src/geometries/*.cpp"
  "src/navigation/*.cpp"
  "src/render/*.cpp"
  "src/shaders/*.cpp"
  "src/text/*.cpp"
  "src/vertexes/*.cpp"
  "src/models/*.cpp"
  "src/profiling/*.cpp"
  "src/controls/*.cpp"
  "src/physics/*.cpp"
  "src/utils/*.cpp"
  "src/levels/*.cpp"
  "src/math/*.cpp"
  "src/audio/*.cpp"
  "src/globals/*.cpp"
  "src/factories/*.cpp"
  "src/terrain/*.cpp"
)

add_executable(main src/main.cpp ${SRC})
target_include_directories(main PRIVATE
  ${FREETYPE_INCLUDE_DIRS}
  include
)
target_link_libraries(main
  assimp
  ${FREETYPE_LIBRARIES}
  ${LIB_FMOD}
  glfw_window
  opengl_utils
)
