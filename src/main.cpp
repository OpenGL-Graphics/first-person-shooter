#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "navigation/camera.hpp"
#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "vertexes/vbo.hpp"
#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "gui/dialog.hpp"

// keys & mouse events listeners
static void on_key(GLFWwindow* window);
static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

// camera (global variable modified inside callbacks)
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// last position of mouse cursor (to calculate offset on movement)
float x_mouse = 0.0f;
float y_mouse = 0.0f;

// background color
const glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);

int main() {
  // initialize glfw
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << "\n";
    return 1;
  }

  // get monitor width/height & init mouse position
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  const int width_monitor = mode->width;
  const int height_monitor = mode->height;
  x_mouse = width_monitor / 2.0f;
  y_mouse = height_monitor / 2.0f;

  // create window and OpenGL context
  GLFWwindow* window = glfwCreateWindow(width_monitor, height_monitor, "OpenGL test", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create window or OpenGL context" << "\n";
    return 1;
  }

  // initialize glad before calling gl functions
  glfwMakeContextCurrent(window);
  if (!gladLoadGL()) {
    std::cout << "Failed to load Glad (OpenGL)" << "\n";
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // callback for processing mouse click
  glfwSetMouseButtonCallback(window, on_mouse_click);

  // create then install vertex & fragment shaders on GPU
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_color("assets/shaders/color.vert", "assets/shaders/color.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_text("assets/shaders/texture_surface.vert", "assets/shaders/texture_text.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  if (pgm_color.has_failed() || pgm_texture_cube.has_failed() || pgm_texture_surface.has_failed() || pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  // textures
  std::vector<std::string> paths_images {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  std::vector<Image> images;
  std::transform(paths_images.begin(), paths_images.end(), std::back_inserter(images), [](const std::string& path) { return Image(path); });
  Texture3D texture_brick(images);
  Texture2D texture_grass(Image("assets/images/grass.png"));
  Texture2D texture_hud_health(Image("assets/images/health.png"));
  Texture2D texture_glass(Image("assets/images/window.png"));

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_color(pgm_color, vbo_cube, {{0, "position", 3, 12, 0}, {0, "color", 3, 12, 3}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {0, "texture_coord", 3, 12, 6}});
  Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});

  // load font & assign its bitmap glyphs to textures
  VBO vbo_glyph(Surface(), true, GL_DYNAMIC_DRAW);
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(pgm_text, vbo_glyph, {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}, font);


  // load 3d model in obj ascii format with assimp
  // flags ensures each face has 3 vertexes indices
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile("assets/models/backpack.obj", aiProcess_Triangulate);
  // const aiScene* scene = importer.ReadFile("assets/models/cube.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
  // const aiScene* scene = importer.ReadFile("assets/models/cube.obj", aiProcess_Triangulate);
  if (scene == NULL) {
    std::cout << "Failed to load 3D model" << '\n';
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  // get meshes from scene
  std::vector<float> vertexes;
  std::vector<unsigned int> indices;

  // for (size_t i_mesh = 0; i_mesh < scene->mNumMeshes; ++i_mesh) {
  for (size_t i_mesh = 0; i_mesh < 1; ++i_mesh) {
    aiMesh* mesh = scene->mMeshes[i_mesh];
    aiVector3D* mesh_positions = mesh->mVertices;
    aiVector3D* mesh_normals = mesh->mNormals;
    aiVector3D* mesh_texture_coords = mesh->mTextureCoords[0];

    // get vertexes position/normals/texture coords from each mesh
    unsigned int n_vertexes = mesh->mNumVertices;
    std::vector<glm::vec3> positions(n_vertexes);
    std::vector<glm::vec3> normals(n_vertexes);
    std::vector<glm::vec2> texture_coords(n_vertexes);
    unsigned int n_coords_position = 3;
    unsigned int n_coords_normal = 3;
    unsigned int n_coords_vertex = n_coords_position + n_coords_normal;
    vertexes.resize(n_coords_vertex * n_vertexes);

    for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
      positions[i_vertex] = {mesh_positions[i_vertex].x, mesh_positions[i_vertex].y, mesh_positions[i_vertex].z};
      normals[i_vertex] = {mesh_normals[i_vertex].x, mesh_normals[i_vertex].y, mesh_normals[i_vertex].z};
      texture_coords[i_vertex] = {mesh_texture_coords[i_vertex].x, mesh_texture_coords[i_vertex].y};

      vertexes[n_coords_vertex*i_vertex] = positions[i_vertex].x;
      vertexes[n_coords_vertex*i_vertex + 1] = positions[i_vertex].y;
      vertexes[n_coords_vertex*i_vertex + 2] = positions[i_vertex].z;

      vertexes[n_coords_vertex*i_vertex + 3] = normals[i_vertex].x;
      vertexes[n_coords_vertex*i_vertex + 4] = normals[i_vertex].y;
      vertexes[n_coords_vertex*i_vertex + 5] = normals[i_vertex].z;
    }

    // get mesh faces (triangles formed by vertexes indices)
    unsigned int n_faces = mesh->mNumFaces;
    indices.resize(3 * n_faces);

    for (size_t i_face = 0; i_face < n_faces; ++i_face) {
      aiFace face = mesh->mFaces[i_face];
      unsigned int n_indices = face.mNumIndices;

      for (size_t i_indice = 0; i_indice < n_indices; ++i_indice) {
        indices[i_face*n_indices + i_indice] = face.mIndices[i_indice];
      }
    }
  }

  // renderer for first mesh in model
  VBO vbo_mesh(Geometry(vertexes, indices));
  // Renderer mesh_renderer(pgm_basic, vbo_mesh, {{0, "position", 3, 6, 0}, {0, "normal", 3, 6, 3}});
  Renderer mesh_renderer(pgm_light, vbo_mesh, {{0, "position", 3, 6, 0}, {0, "normal", 3, 6, 3}});


  // initialize dialog with imgui
  Dialog dialog(window, "Dialog title", "Dialog text");

  // enable depth test & blending
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // keyboard input (move camera, quit application)
    on_key(window);

    // before render, clear color buffer & depth buffer
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // transformation matrices
    glm::mat4 view = camera.get_view();
    glm::mat4 projection3d = glm::perspective(glm::radians(camera.get_fov()), (float) width_monitor / (float) height_monitor, 1.0f, 50.f);
    glm::mat4 projection2d = glm::ortho(0.0f, (float) width_monitor, 0.0f, (float) height_monitor);

    // draw 3x texture cubes
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -1.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });

    // cube & light colors
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);

    // draw light cube (scaling then translation)
    glm::vec3 position_light(-2.0f, -1.0f, 2.0f);
    glm::mat4 model_light(glm::scale(
      glm::translate(glm::mat4(1.0f), position_light),
      glm::vec3(0.2f)
    ));
    cube_basic.draw({
      {"model", model_light},
      {"view", view},
      {"projection", projection3d},
      {"color", color_light},
    });

    // draw illuminated cube
    cube_light.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 4.0f))},
      {"view", view},
      {"projection", projection3d},
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
      {"position_camera", camera.get_position()},
    });

    // draw color cube
    cube_color.draw({
      {"model", glm::mat4(1.0f)},
      {"view", view},
      {"projection", projection3d},
    });

    // draw 3d model mesh (illuminated with normals)
    /*
    glm::vec3 color_mesh = {1.0f, 0.0f, 0.0f};
    mesh_renderer.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
      {"color", color_mesh},
    });
    */
    mesh_renderer.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
      {"material.ambiant", glm::vec3(1.0f, 0.0f, 1.0f)},
      {"material.diffuse", glm::vec3(1.0f, 0.0f, 1.0f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
      {"position_camera", camera.get_position()},
    });

    // draw 2d grass surface (non-centered)
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_grass},
    });

    // last to render: transparent surfaces to ensure blending with background
    // draw half-transparent 3d window
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 2.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_glass},
    });

    // draw 2d health bar HUD surface (scaling then translation to lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(glm::mat4(1.0f), glm::vec3(width_monitor - texture_hud_health.get_width(), 0.0f, 0.0f)),
      glm::vec3(texture_hud_health.get_width(), texture_hud_health.get_height(), 1.0f)
    ));
    surface.draw({
      {"model", model_hud_health},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
      {"texture2d", texture_hud_health},
    });

    // draw 2d text surface (origin: left baseline)
    Uniforms uniforms = {
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f))},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
    };
    surface_glyph.draw_text(uniforms, "Player");

    // render imgui dialog
    dialog.render();

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy imgui
  dialog.free();

  // destroy textures
  texture_brick.free();
  texture_grass.free();
  texture_glass.free();
  texture_hud_health.free();

  Glyphs glyphs(surface_glyph.get_glyphs());
  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    Glyph glyph(glyphs.at(c));
    glyph.texture.free();
  }

  // destroy renderers of each shape (frees vao & vbo)
  cube_basic.free();
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  surface.free();
  surface_glyph.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_color.free();
  pgm_texture_cube.free();
  pgm_texture_surface.free();
  pgm_light.free();
  pgm_text.free();

  // destroy window & terminate glfw
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

static void on_key(GLFWwindow* window) {
  // close window on escape key press
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // move camera
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.move(Direction::FORWARD);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.move(Direction::BACKWARD);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.move(Direction::LEFT);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.move(Direction::RIGHT);
}

static void on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
  // switch mouse mode & listen for mouse movement/scroll
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetCursorPosCallback(window, on_mouse_move);
      glfwSetScrollCallback(window, on_mouse_scroll);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPosCallback(window, NULL);
      glfwSetScrollCallback(window, NULL);
    }
  }
}

static void on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
  // see: https://www.reddit.com/r/opengl/comments/831vpb/
  // calculate offset in mouse cursor position
  float x_offset = xpos - x_mouse;
  float y_offset = ypos - y_mouse;
  x_mouse = xpos;
  y_mouse = ypos;

  // horizontal/vertical rotation around y-axis/x-axis accord. to offset
  camera.rotate(x_offset, y_offset);
}

static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
  // zoom in/out using mouse wheel
  if (yoffset == 1) {
    camera.zoom(Zoom::IN);
  } else if (yoffset == -1) {
    camera.zoom(Zoom::OUT);
  }
}
