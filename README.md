# Program
- **src/main.cpp:** Draws textured 3D cubes with OpenGL shaders on a window generated with GLFW, with the possibility to navigate in the scene using the mouse and WASD keys.

# OpenGL
- **GLFW (Graphics Library Framework):** Library for creating windows with OpenGL context and for handling mouse and keyboard interactions. Read this [GLFW tutorial].
- **Glad:** Loader for OpenGL functions. Following this tutorial from [learnopengl.com], glad files can be generated using this [webservice] then copied inside include and src folders.

[GLFW tutorial]: https://www.glfw.org/docs/latest/quick_guide.html
[webservice]: https://glad.dav1d.de/

# Shaders
Shaders are written in GLSL (OpenGL Shading Language). The following two shaders (GPU programs) are needed to draw with OpenGL:

- **Vertex shader:** for each vertex, determines its position on the scene.
- **Fragment shader:** for each pixel, determines its color on the screen.

# Credit
Useful tutorials:

- Getting started with GLFW: [GLFW tutorial] 
- Learn OpenGL tutorial: [learnopengl.com]
- Opengl-tutorial: [opengl-tutorial.org]
- OpenGL tutorial series on Youtube: [TheCherno]
- Similarly to this tutorial, an older version of GLSL was used: [open.gl]

[learnopengl.com]: https://learnopengl.com/Getting-started/Creating-a-window
[opengl-tutorial.org]: http://www.opengl-tutorial.org/beginners-tutorials/
[TheCherno]: https://www.youtube.com/watch?v=W3gAzLwfIP0
[open.gl]: https://open.gl/

# Docs
- [khronos.org]
- [docs.gl]

[docs.gl]: http://docs.gl/
[khronos.org]: https://www.khronos.org/registry/OpenGL-Refpages/gl4/

# Imgui
- Source and header files for imgui were copied directly from [imgui-repo] into this project.
- Follow [imgui-example] to include imgui in an opengl/glsw project.

[imgui-repo]: https://github.com/ocornut/imgui/ 
[imgui-example]: https://github.com/ocornut/imgui/tree/master/examples/example_glfw_opengl3

# Texture image
- As OpenGL doesn't support standard image formats (jpeg, png), texture images are loaded using [stb-image].
- Brick and building texture images downloaded from [textures.com].

[stb-image]: https://github.com/nothings/stb/blob/master/stb_image.h
[textures.com]: https://www.textures.com

# Lights
According to this [blog post][lights], we distinguish three types of colors when constructing a surface material:

1. Ambient: Object's color when under the shadow of an ambient light (e.g. moon).
2. Diffuse: Object's color when illuminated by a directed light, and is often perceived as the true color of the object.
3. Specular: Reflection of the directed light on the object's surface. It depends on the shininess which makes the surface reflects (instead of scattering) the light, as its value increases. This therefore makes the highlight area become smaller, brighter, and more concentrated.

A table of these components values for different materials can be found on [this site][materials].

[lights]: http://blog.lexique-du-net.com/index.php?post/2009/07/24/AmbientDiffuseEmissive-and-specular-colorSome-examples
[materials]: http://devernay.free.fr/cours/opengl/materials.html

# Resources
- [Health bar][health-bar] made by Daniel Zhang (APEXOUS) and available under the CC0 license.

[health-bar]: https://opengameart.org/content/rpg-hud-bars

# OpenGL objects
- **VBO (Vertex Buffer Object):**
  - Holds vertices data.
  - Needs to be bound before attaching vertexes data to it with `glBufferData`.
- **VAO (Vertex Array Object):**
  - Linked to VBO when attributes are defined using `glVertexAttribPointer` (both VAO and VBO being bound).
  - Then inside the main loop, we'll only need to bind the VAO before drawing the vertexes.
