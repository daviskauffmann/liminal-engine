# Project Kilonova

## Getting Started

### Dependencies

Make sure the following are present on your system:

- Assimp
- Bullet
- GLEW
- GLM
- OpenAL
- SDL2
- SDL2_image
- SDL2_mixer

These dependencies are included as submodules:

- Dear ImGui
- stb

### Download

```sh
git clone https://github.com/daviskauffmann/project-kilonova.git
cd project-kilonova
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
- Model loading (WIP)
- 3D sound
- Runtime shader reloading
- Terrain (WIP)
- 2D sprites (WIP)

### Upcoming

- Physics
- Transparency
- Ambient occlusion
- Particles
- Fog and other atmospheric effects
- Underwater effects
- Anti-aliasing
- Animation
- Real-time level editing
- LUA scripting

## Screenshots

![1](screenshots/1.png)
![2](screenshots/2.png)

## Resources Used

[Learn OpenGL](https://learnopengl.com)

[Lazy Foo' Productions](https://lazyfoo.net)

[opengl-tutorial.org](http://www.opengl-tutorial.org)

[thebennybox](https://www.youtube.com/user/thebennybox)

[ThinMatrix](https://www.youtube.com/user/ThinMatrix)

[The Cherno](https://www.youtube.com/user/TheChernoProject)

[Hopson](https://www.youtube.com/channel/UCeQhZOvNKSBRU0Mdg7V44wA)
