# Program
- **src/hello_opengl.cpp:** Draws a triangle using OpenGL shaders on a window generated with GLFW.

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
