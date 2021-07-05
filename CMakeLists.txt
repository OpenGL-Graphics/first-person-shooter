cmake_minimum_required(VERSION 3.10)
project(FPS)

# use C++17 (variants used for uniforms of different types)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# profiling flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# copy assets folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# needed packages
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(assimp REQUIRED)

# glad shared library (opengl)
add_library(glad SHARED src/glad/glad.c)
target_include_directories(glad PUBLIC include)

# imgui library (preprocessor definition to force it to use glad)
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
file(GLOB IMGUI_SRC "src/imgui/*.cpp")
add_library(imgui SHARED ${IMGUI_SRC})
target_include_directories(imgui PUBLIC include/imgui include)

# main executable
file(GLOB SRC
  "src/gui/*.cpp"
  "src/geometries/*.cpp"
  "src/materials/*.cpp"
  "src/navigation/*.cpp"
  "src/render/*.cpp"
  "src/shaders/*.cpp"
  "src/text/*.cpp"
  "src/vertexes/*.cpp"
  "src/models/*.cpp"
  "src/profiling/*.cpp"
)

add_executable(main src/main.cpp ${SRC})
target_include_directories(main PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(main glfw glad imgui assimp ${FREETYPE_LIBRARIES})
