# Screenshot
![Screenshot](/screenshots/screenshot-2022-03-27.png)

# How to run
```console
# clone repo with its submodules
$ git clone --recursive https://github.com/OpenGL-Graphics/first-person-shooter

# build & run
$ cd first-person-shooter
$ mkdir build && cd build
$ cmake .. && make -j && ./main

# to get new commits from submodules
$ git submodule update --init --recursive --remote
```

# Dependencies
To install the needed dependencies on Ubuntu:

```console
$ apt install libglm-dev libglfw3-dev libassimp-dev
```

# Requirements
- Opengl 3.3+
- GLSL 3.3+

# Resources
- [Health bar][health-bar] made by Daniel Zhang (APEXOUS) and available under the CC0 license.
- Textures (incl. skybox) downloaded from [textures.com][textures.com].
- [Low poly sniper][lowpoly-sniper]
- [Samurai][samurai]

[health-bar]: https://opengameart.org/content/rpg-hud-bars
[textures.com]: https://www.textures.com
[lowpoly-sniper]: https://opengameart.org/content/low-poly-stylized-sniper
[samurai]: https://opengameart.org/content/3d-lowpoly-samurai-with-animations-fbx-format

---

# Tutorials
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

---

# Program
- **src/main.cpp:** Draws player's gun, scene from tilemap, and colored 3D cubes targets with OpenGL shaders on a window generated with GLFW, with the possibility to navigate in the scene using WASD keys and the mouse (LMB is for shooting).

# OpenGL
- **GLFW (Graphics Library Framework):** Library for creating windows with OpenGL context and for handling mouse and keyboard interactions. Read this [GLFW tutorial].
- **Glad:** Loader for OpenGL functions. Following this tutorial from [learnopengl.com], glad files can be generated using this [webservice] then copied inside include and src folders.

[GLFW tutorial]: https://www.glfw.org/docs/latest/quick_guide.html
[webservice]: https://glad.dav1d.de/

# Shaders
Shaders are written in GLSL (OpenGL Shading Language). The following two shaders (GPU programs) are needed to draw with OpenGL:

- **Vertex shader:** for each vertex, determines its position on the scene.
- **Fragment shader:** for each pixel, determines its color on the screen.

# Texture image
- Images used as textures are loaded using [stb-image] then sent to the GPU.
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

# OpenGL objects
- **VBO (Vertex Buffer Object):**
  - Holds vertices data.
  - Needs to be bound before attaching vertexes data to it with `glBufferData`.
- **VAO (Vertex Array Object):**
  - Linked to VBO when attributes are defined using `glVertexAttribPointer` (both VAO and VBO being bound).
  - Then inside the main loop, we'll only need to bind the VAO (and to use the appropriate shaders program) before drawing the vertexes.

# Loading 3D models
- [Assimp][assimp] was used to load 3D models in `\*.obj` format in OpenGL:

```
sudo apt install assimp-utils
```

- 3D models in [\*.obj][obj-format] format are ASCII text files that can be exported with Blender.

[assimp]: http://assimp.sourceforge.net/lib_html/index.html
[obj-format]: https://en.wikipedia.org/wiki/Wavefront_.obj_file


# Profiling with gprof
- Install gprof:

```console
$ apt-get install binutils
```

- Compile program with profiling support:

```console
$ cc -pg script.cpp -o app
```

- Run with gprof and look for function to profile:

```console
$ gprof -p app | less
```

# Valgrind
- Install `valgrind` from source following its [official webpage][valgrind-build] or with package manager:

```console
$ pacman -S valgrind
```

- Build project with debug symbols (`-g` in gcc, `set(CMAKE_BUILD_TYPE Debug)` in cmake)

[valgrind-build]: https://valgrind.org/docs/manual/dist.readme.html

## Detecting memory leaks
```console
$ valgrind --tool=memcheck ./main
```

## Memory usage
### With massif
```console
$ valgrind --tool=massif ./main
$ ms_print ./massif.out.12345
```

In the graph:
- Snapshot: Vertical bar (memory usage at time = t).
- `:`: Normal snapshot.
- `@`: Detailed snapshot.
- `#`: Peak snapshot.

### Massif-visualizer
For a better visualization on a gui:

```console
$ pacman -S massif-visualizer
$ massif-visualizer ./massif.out.12345
```

### Programmatic snapshots
To take a snapshot at a specific time, add to your source code:

```c
#include "valgrind/valgrind.h"

int main() {
    ...
    VALGRIND_MONITOR_COMMAND("detailed_snapshot");
    ...
}
```

And run program with massif:

```console
$ valgrind --tool=massif ./main
$ massif-visualizer massif.vgdb.out
```
