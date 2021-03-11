# Liminal Engine

## Getting Started

### Dependencies

Make sure the following are present on your system:

- Assimp
- Bullet
- cxxopts
- GLEW
- GLM
- Lua
- OpenAL
- SDL2
- SDL2_image
- SDL2_mixer
- sol2

These dependencies are included as submodules:

- Dear ImGui
- stb

### Download

```sh
git clone https://github.com/daviskauffmann/liminal-engine.git
cd liminal-engine
git submodule update --init
```

### Build

```sh
make
```

### Build & Run

```sh
make run
```

### Cleanup

```sh
make clean
```

## Features

- Deferred shading
- PBR w/ metallic workflow
- HDR
- Bloom
- Light sources and shadow mapping
  - Directional
  - Point
  - Spot
- Water
  - Planar reflections
  - Refraction
- 3D sound
- Runtime shader reloading
- Model loading (WIP)
- Terrain (WIP)
- 2D sprites (WIP)
- Physics (WIP)
- LUA scripting (WIP)
- Animation (WIP)

### Upcoming

- Transparency
- Ambient occlusion
- Particles
- Fog and other atmospheric effects
- Underwater effects
- Anti-aliasing
- Real-time level editing

## Screenshots

![Backpack](screenshots/backpack.png)

## Resources Used

[Learn OpenGL](https://learnopengl.com)

[opengl-tutorial.org](http://www.opengl-tutorial.org)

[ogldev](http://ogldev.atspace.co.uk)

[Lazy Foo' Productions](https://lazyfoo.net)

[thebennybox](https://www.youtube.com/user/thebennybox)

[ThinMatrix](https://www.youtube.com/user/ThinMatrix)

[The Cherno](https://www.youtube.com/user/TheChernoProject)

[Hopson](https://www.youtube.com/channel/UCeQhZOvNKSBRU0Mdg7V44wA)

[Byungil Jeong - CS594 - Project 2](https://www.evl.uic.edu/bijeong/cs594/proj2)

[Bones animation with OpenGL, ASSIMP and GLM](http://www.xphere.me/2019/05/bones-animation-with-openglassimpglm/)
