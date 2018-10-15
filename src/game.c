#include <cglm/cglm.h>

#include "camera.h"
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
struct camera *camera;

GLint standard_program_material_diffuse;
GLint standard_program_material_specular;
GLint standard_program_material_emission;
GLint standard_program_time;
GLint standard_program_camera_projection;
GLint standard_program_camera_view;
GLint standard_program_camera_position;

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
              cube_mesh,
              box_material,
              (vec3){0.0f, 0.0f, 0.0f},
              (vec3){0.0f, 0.0f, 0.0f},
              (vec3){1.0f, 1.0f, 1.0f})))
    {
        return 1;
    }

    if (!(camera = camera_create(
              (vec3){0.0f, 0.0f, 3.0f},
              (vec3){0.0f, 0.0f, -1.0f},
              (vec3){0.0f, 1.0f, 0.0f},
              0.0f,
              -90.0f,
              0.0f,
              45.0f)))
    {
        return 1;
    }

    window_toggle_mouse();

    standard_program_material_diffuse = program_get_location(standard_program, "material.diffuse");
    standard_program_material_specular = program_get_location(standard_program, "material.specular");
    standard_program_material_emission = program_get_location(standard_program, "material.emission");
    standard_program_time = program_get_location(standard_program, "time");
    standard_program_camera_projection = program_get_location(standard_program, "camera.projection");
    standard_program_camera_view = program_get_location(standard_program, "camera.view");
    standard_program_camera_position = program_get_location(standard_program, "camera.position");

    program_bind(standard_program);
    program_set_int(standard_program_material_diffuse, 0);
    program_set_int(standard_program_material_specular, 1);
    program_set_int(standard_program_material_emission, 2);
    program_unbind();

    return 0;
}

bool game_input(float delta_time)
{
    int num_keys;
    const unsigned char *keys = window_keys(&num_keys);

    int mouse_x, mouse_y;
    unsigned int mouse = window_mouse(&mouse_x, &mouse_y);

    SDL_Event event;
    while (window_events(&event))
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
            case SDLK_TAB:
            {
                window_toggle_mouse();
            }
            break;
            }
        }
        break;
        case SDL_MOUSEMOTION:
        {
            camera->pitch -= event.motion.yrel * 0.1f;
            camera->yaw += event.motion.xrel * 0.1f;

            if (camera->pitch > 89.0f)
            {
                camera->pitch = 89.0f;
            }
            if (camera->pitch < -89.0f)
            {
                camera->pitch = -89.0f;
            }

            vec3 front = {
                cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
                sinf(glm_rad(camera->pitch)),
                sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch))};
            glm_normalize(front);
            glm_vec_copy(front, camera->front);
        }
        break;
        case SDL_MOUSEWHEEL:
        {
            if (camera->fov >= 1.0f && camera->fov <= 90.0f)
            {
                camera->fov -= event.wheel.y;
            }
            if (camera->fov <= 1.0f)
            {
                camera->fov = 1.0f;
            }
            if (camera->fov >= 90.0f)
            {
                camera->fov = 90.0f;
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
    }

    float speed = 5.0f * delta_time;

    if (keys[SDL_SCANCODE_LSHIFT])
    {
        speed *= 2.0f;
    }

    if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
        (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
        (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
        (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
    {
        speed *= 0.71f;
    }

    if (keys[SDL_SCANCODE_W])
    {
        vec3 movement;
        glm_vec_scale(camera->front, speed, movement);
        glm_vec_add(camera->position, movement, camera->position);
    }

    if (keys[SDL_SCANCODE_A])
    {
        vec3 direction;
        glm_cross(camera->front, camera->up, direction);
        glm_normalize(direction);
        vec3 movement;
        glm_vec_scale(direction, -speed, movement);
        glm_vec_add(camera->position, movement, camera->position);
    }

    if (keys[SDL_SCANCODE_S])
    {
        vec3 movement;
        glm_vec_scale(camera->front, -speed, movement);
        glm_vec_add(camera->position, movement, camera->position);
    }

    if (keys[SDL_SCANCODE_D])
    {
        vec3 direction;
        glm_cross(camera->front, camera->up, direction);
        glm_normalize(direction);
        vec3 movement;
        glm_vec_scale(direction, speed, movement);
        glm_vec_add(camera->position, movement, camera->position);
    }

    return false;
}

void game_update(unsigned int current_time, float delta_time)
{
    mat4 camera_projection;
    camera_calc_projection(camera, camera_projection);

    mat4 camera_view;
    camera_calc_view(camera, camera_view);

    program_bind(standard_program);
    program_set_int(standard_program_time, current_time);
    program_set_mat4(standard_program_camera_projection, camera_projection);
    program_set_mat4(standard_program_camera_view, camera_view);
    program_set_vec3(standard_program_camera_position, camera->position);
    program_unbind();

    object_update(object);
}

void game_render(void)
{
    program_bind(standard_program);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    object_calc_model(object, model);

    program_set_mat4(program_get_location(standard_program, "object.model"), model);

    program_set_float(program_get_location(standard_program, "material.shininess"), object->material->shininess);
    program_set_float(program_get_location(standard_program, "material.glow"), object->material->glow);

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
