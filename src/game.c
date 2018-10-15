#include <cglm/cglm.h>

#include "game.h"
#include "material.h"
#include "mesh.h"
#include "object.h"
#include "program.h"
#include "texture.h"
#include "window.h"

struct program *standard_program;
struct mesh *quad_mesh;
struct mesh *triangle_mesh;
struct mesh *cube_mesh;
struct texture *box_diffuse_texture;
struct texture *box_specular_texture;
struct material *box_material;
struct object *object;

int game_init(void)
{
    if (!(standard_program = program_create(
              "assets/shaders/standard.vs",
              "assets/shaders/standard.fs")))
    {
        return 1;
    }

    float quad_vertices[] = {
        // position          // normal            // uv
        +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f, // top right
        +1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int quad_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    if (!(quad_mesh = mesh_create(
              quad_vertices,
              sizeof(quad_vertices),
              quad_indices,
              sizeof(quad_indices))))
    {
        return 1;
    }

    float triangle_vertices[] = {
        +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f,
        +1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f};

    unsigned int triangle_indices[] = {
        0, 1, 2};

    if (!(triangle_mesh = mesh_create(
              triangle_vertices,
              sizeof(triangle_vertices),
              triangle_indices,
              sizeof(triangle_indices))))
    {
        return 1;
    }

    float cube_vertices[] = {
        -1.0f, -1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 0.0f, 0.0f,
        +1.0f, -1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 1.0f, 1.0f,
        +1.0f, +1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, +1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, +0.0f, +0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 0.0f, 0.0f,
        +1.0f, -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 1.0f, 1.0f,
        +1.0f, +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 1.0f, 1.0f,
        -1.0f, +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, 0.0f, 0.0f,
        -1.0f, +1.0f, +1.0f, -1.0f, +0.0f, +0.0f, 1.0f, 0.0f,
        -1.0f, +1.0f, -1.0f, -1.0f, +0.0f, +0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, +0.0f, +0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, +0.0f, +0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, +1.0f, -1.0f, +0.0f, +0.0f, 0.0f, 0.0f,
        -1.0f, +1.0f, +1.0f, -1.0f, +0.0f, +0.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +0.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, -1.0f, +1.0f, +0.0f, +0.0f, 1.0f, 1.0f,
        +1.0f, -1.0f, -1.0f, +1.0f, +0.0f, +0.0f, 0.0f, 1.0f,
        +1.0f, -1.0f, -1.0f, +1.0f, +0.0f, +0.0f, 0.0f, 1.0f,
        +1.0f, -1.0f, +1.0f, +1.0f, +0.0f, +0.0f, 0.0f, 0.0f,
        +1.0f, +1.0f, +1.0f, +1.0f, +0.0f, +0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, +0.0f, -1.0f, +0.0f, 0.0f, 1.0f,
        +1.0f, -1.0f, -1.0f, +0.0f, -1.0f, +0.0f, 1.0f, 1.0f,
        +1.0f, -1.0f, +1.0f, +0.0f, -1.0f, +0.0f, 1.0f, 0.0f,
        +1.0f, -1.0f, +1.0f, +0.0f, -1.0f, +0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, +1.0f, +0.0f, -1.0f, +0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, +0.0f, -1.0f, +0.0f, 0.0f, 1.0f,
        -1.0f, +1.0f, -1.0f, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f,
        +1.0f, +1.0f, -1.0f, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f,
        +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f,
        -1.0f, +1.0f, +1.0f, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f,
        -1.0f, +1.0f, -1.0f, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f};

    unsigned int cube_indices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35};

    if (!(cube_mesh = mesh_create(
              cube_vertices,
              sizeof(cube_vertices),
              cube_indices,
              sizeof(cube_indices))))
    {
        return 1;
    }

    if (!(box_diffuse_texture = texture_create("assets/images/box_diffuse.png")))
    {
        return 1;
    }

    if (!(box_specular_texture = texture_create("assets/images/box_specular.png")))
    {
        return 1;
    }

    if (!(box_material = material_create(
              box_diffuse_texture,
              box_specular_texture,
              NULL,
              32.0f,
              1.0f)))
    {
        return 1;
    }

    if (!(object = object_create(
              quad_mesh,
              box_material,
              (vec3){0.0f, 0.0f, 0.0f},
              (vec3){0.0f, 0.0f, 0.0f},
              (vec3){1.0f, 1.0f, 1.0f})))
    {
        return 1;
    }

    return 0;
}

bool game_input(
    int num_keys,
    const unsigned char *keys,
    int mouse_x,
    int mouse_y,
    unsigned int mouse,
    SDL_Event event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_F4:
        {
            if (keys[SDL_SCANCODE_LALT])
            {
                return true;
            }
        }
        break;
        case SDLK_ESCAPE:
        {
            return true;
        }
        break;
        case SDLK_RETURN:
        {
            if (keys[SDL_SCANCODE_LALT])
            {
                window_toggle_fullscreen();
            }
        }
        break;
        }
    }
    break;
    case SDL_QUIT:
    {
        return true;
    }
    break;
    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
        {
            int width = event.window.data1;
            int height = event.window.data2;

            window_resize(width, height);
        }
        break;
        }
    }
    break;
    }

    return false;
}

void game_update(float delta_time)
{
    object_update(object);
}

void game_render(void)
{
    program_bind(standard_program);

    object_draw(object);

    program_unbind();
}

void game_quit(void)
{
    object_destroy(object);
    material_destroy(box_material);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);
    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);
    program_destroy(standard_program);
}
