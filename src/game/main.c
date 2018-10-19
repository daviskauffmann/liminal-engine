#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS_CAP 60

enum lighting
{
    LIGHTING_BASIC,
    LIGHTING_PHONG,
    LIGHTING_DEFERRED,
};

int main(int argc, char *argv[])
{
    // setup engine
    if (core_init())
    {
        return 1;
    }

    if (window_init(
            WINDOW_TITLE,
            WINDOW_WIDTH,
            WINDOW_HEIGHT))
    {
        return 1;
    }

    if (audio_init(
            MIX_DEFAULT_FREQUENCY,
            MIX_DEFAULT_FORMAT,
            MIX_DEFAULT_CHANNELS,
            1024))
    {
        return 1;
    }

    if (image_init())
    {
        return 1;
    }

    time_cap_fps(FPS_CAP);

    SDL_SetRelativeMouseMode(true);

    // create programs
    struct program *basic_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/basic.fs");

    if (!basic_program)
    {
        return 1;
    }

    struct program *phong_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/phong.fs");

    if (!phong_program)
    {
        return 1;
    }

    struct program *deferred_ambient_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/deferred_ambient.fs");

    if (!deferred_ambient_program)
    {
        return 1;
    }

    struct program *deferred_directional_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/deferred_directional.fs");

    if (!deferred_directional_program)
    {
        return 1;
    }

    struct program *deferred_point_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/deferred_point.fs");

    if (!deferred_point_program)
    {
        return 1;
    }

    struct program *deferred_spot_program = program_create(
        "assets/shaders/basic.vs",
        "assets/shaders/deferred_spot.fs");

    if (!deferred_spot_program)
    {
        return 1;
    }

    // create meshes
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

    struct mesh *quad_mesh = mesh_create(
        quad_vertices,
        sizeof(quad_vertices),
        quad_indices,
        sizeof(quad_indices));

    if (!quad_mesh)
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

    struct mesh *cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        return 1;
    }

    // create textures
    struct texture *box_diffuse_texture = texture_create("assets/images/box_diffuse.png");

    if (!box_diffuse_texture)
    {
        return 1;
    }

    struct texture *box_specular_texture = texture_create("assets/images/box_specular.png");

    if (!box_specular_texture)
    {
        return 1;
    }

    struct texture *cobble_diffuse_texture = texture_create("assets/images/cobble_diffuse.jpg");

    if (!cobble_diffuse_texture)
    {
        return 1;
    }

    struct texture *cobble_specular_texture = texture_create("assets/images/cobble_specular.jpg");

    if (!cobble_specular_texture)
    {
        return 1;
    }

    // create materials
    struct material *box_material = material_create(
        box_diffuse_texture,
        box_specular_texture,
        NULL,
        (vec3){1.0f, 1.0f, 1.0f},
        32.0f,
        1.0f);

    if (!box_material)
    {
        return 1;
    }

    struct material *cobble_material = material_create(
        cobble_diffuse_texture,
        cobble_specular_texture,
        NULL,
        (vec3){1.0f, 1.0f, 1.0f},
        32.0f,
        1.0f);

    if (!cobble_material)
    {
        return 1;
    }

    // create objects
    struct object *objects[] = {
        object_create(
            cube_mesh,
            cobble_material,
            (vec3){0.0f, -2.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){10.0f, 1.0f, 10.0f}),
        object_create(
            cube_mesh,
            box_material,
            (vec3){0.0f, 1.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){1.0f, 1.0f, 1.0f})};
    const unsigned int num_objects = sizeof(objects) / sizeof(struct object *);

    for (int i = 0; i < num_objects; i++)
    {
        if (!objects[i])
        {
            return 1;
        }
    }

    // create lights
    vec3 ambient = {0.1f, 0.1f, 0.1f};

    struct directional_light *directional_lights[] = {
        directional_light_create(
            (vec3){-0.2f, -1.0f, -0.3f},
            (vec3){0.05f, 0.05f, 0.05f},
            (vec3){0.4f, 0.4f, 0.4f},
            (vec3){1.0f, 1.0f, 1.0f})};
    const unsigned int num_directional_lights = sizeof(directional_lights) / sizeof(struct directional_light *);

    for (int i = 0; i < num_directional_lights; i++)
    {
        if (!directional_lights[i])
        {
            return 1;
        }
    }

    struct point_light *point_lights[] = {
        // red
        point_light_create(
            (vec3){2.0f, 0.0f, 2.0f},
            (vec3){0.1f, 0.0f, 0.0f},
            (vec3){0.8f, 0.0f, 0.0f},
            (vec3){1.0f, 0.0f, 0.0f},
            1.0f,
            0.09f,
            0.032f),
        // yellow
        point_light_create(
            (vec3){-2.0f, 0.0f, -2.0f},
            (vec3){0.1f, 0.1f, 0.0f},
            (vec3){0.8f, 0.8f, 0.0f},
            (vec3){1.0f, 1.0f, 0.0f},
            1.0f,
            0.09f,
            0.032f),
        // green
        point_light_create(
            (vec3){2.0f, 0.0f, -2.0f},
            (vec3){0.0f, 0.1f, 0.0f},
            (vec3){0.0f, 0.8f, 0.0f},
            (vec3){0.0f, 1.0f, 0.0f},
            1.0f,
            0.09f,
            0.032f),
        // blue
        point_light_create(
            (vec3){-2.0f, 0.0f, 2.0f},
            (vec3){0.0f, 0.0f, 0.1f},
            (vec3){0.0f, 0.0f, 0.8f},
            (vec3){0.0f, 0.0f, 1.0f},
            1.0f,
            0.09f,
            0.032f)};
    const unsigned int num_point_lights = sizeof(point_lights) / sizeof(struct point_light *);

    for (int i = 0; i < num_point_lights; i++)
    {
        if (!point_lights[i])
        {
            return 1;
        }
    }

    struct spot_light *spot_lights[] = {
        spot_light_create(
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            1.0f,
            0.09f,
            0.032f,
            cosf(glm_rad(12.5f)),
            cosf(glm_rad(15.0f)))};
    const unsigned int num_spot_lights = sizeof(spot_lights) / sizeof(struct spot_light *);

    for (int i = 0; i < num_spot_lights; i++)
    {
        if (!spot_lights[i])
        {
            return 1;
        }
    }

    // create camera
    struct camera *camera = camera_create(
        (vec3){0.0f, 0.0f, 3.0f},
        (vec3){0.0f, 0.0f, -1.0f},
        (vec3){0.0f, 1.0f, 0.0f},
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    if (!camera)
    {
        return 1;
    }

    // load music
    Mix_Music *background_music = Mix_LoadMUS("assets/audio/background.mp3");

    if (!background_music)
    {
        error(Mix_GetError());

        return 1;
    }

    // load sounds
    Mix_Chunk *shoot_sound = Mix_LoadWAV("assets/audio/shoot.wav");

    if (!shoot_sound)
    {
        error(Mix_GetError());

        return 1;
    }

    // setup shader samplers
    program_bind(basic_program);
    program_set_int(program_get_location(basic_program, "material.diffuse"), 0);
    program_unbind();

    program_bind(phong_program);
    program_set_int(program_get_location(phong_program, "material.diffuse"), 0);
    program_set_int(program_get_location(phong_program, "material.specular"), 1);
    program_set_int(program_get_location(phong_program, "material.emission"), 2);
    program_unbind();

    program_bind(deferred_ambient_program);
    program_set_int(program_get_location(deferred_ambient_program, "material.diffuse"), 0);
    program_set_int(program_get_location(deferred_ambient_program, "material.specular"), 1);
    program_set_int(program_get_location(deferred_ambient_program, "material.emission"), 2);
    program_unbind();

    program_bind(deferred_directional_program);
    program_set_int(program_get_location(deferred_directional_program, "material.diffuse"), 0);
    program_set_int(program_get_location(deferred_directional_program, "material.specular"), 1);
    program_set_int(program_get_location(deferred_directional_program, "material.emission"), 2);
    program_unbind();

    program_bind(deferred_point_program);
    program_set_int(program_get_location(deferred_point_program, "material.diffuse"), 0);
    program_set_int(program_get_location(deferred_point_program, "material.specular"), 1);
    program_set_int(program_get_location(deferred_point_program, "material.emission"), 2);
    program_unbind();

    program_bind(deferred_spot_program);
    program_set_int(program_get_location(deferred_spot_program, "material.diffuse"), 0);
    program_set_int(program_get_location(deferred_spot_program, "material.specular"), 1);
    program_set_int(program_get_location(deferred_spot_program, "material.emission"), 2);
    program_unbind();

    // used for switching shaders
    enum lighting lighting = LIGHTING_DEFERRED;

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

        // update window title
        char title[256];
        sprintf(title, "%s - FPS: %d", WINDOW_TITLE, time_fps());
        window_set_title(title);

        // get keyboard input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_1:
                {
                    if (Mix_PlayingMusic())
                    {
                        Mix_HaltMusic();
                    }
                    else
                    {
                        Mix_PlayMusic(background_music, -1);
                    }
                }
                break;
                case SDLK_2:
                {
                    if (Mix_PlayingMusic())
                    {
                        if (Mix_PausedMusic())
                        {
                            Mix_ResumeMusic();
                        }
                        else
                        {
                            Mix_PauseMusic();
                        }
                    }
                }
                break;
                case SDLK_F1:
                {
                    lighting = LIGHTING_BASIC;
                }
                break;
                case SDLK_F2:
                {
                    lighting = LIGHTING_PHONG;
                }
                break;
                case SDLK_F3:
                {
                    lighting = LIGHTING_DEFERRED;
                }
                break;
                case SDLK_F4:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        quit = true;
                    }
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
                    SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                // mouselook
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
                // zoom
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
                quit = true;
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

        // calculate movement speed
        float speed = 5.0f * time_delta();

        // sprinting
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }

        // slow movement speed when moving diagonally
        if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
        {
            // precomputed 1 / sqrt(2)
            speed *= 0.71f;
        }

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            vec3 movement;
            glm_vec_scale(camera->front, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);
            vec3 movement;
            glm_vec_scale(direction, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            vec3 movement;
            glm_vec_scale(camera->front, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);
            vec3 movement;
            glm_vec_scale(direction, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // shooting
        static float shoot_timer = 0.0f;
        shoot_timer += time_delta();
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                Mix_PlayChannel(-1, shoot_sound, 0);
            }
        }

        // update objects
        float angle = time_current() * 0.001f;
        objects[1]->rotation[0] = angle;
        objects[1]->rotation[1] = angle;
        objects[1]->rotation[2] = angle;

        // update lights
        glm_vec_copy(camera->position, spot_lights[0]->position);
        glm_vec_copy(camera->front, spot_lights[0]->direction);

        // calculate projection matrix
        mat4 camera_projection;
        camera_calc_perspective(camera, camera_projection);

        // calculate view matrix
        mat4 camera_view;
        camera_calc_view(camera, camera_view);

        // clear the window
        window_clear();

        // draw objects
        for (int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            switch (lighting)
            {
            case LIGHTING_BASIC:
            {
                program_bind(basic_program);

                program_set_mat4(program_get_location(basic_program, "object.model"), model);

                program_set_mat4(program_get_location(basic_program, "camera.projection"), camera_projection);
                program_set_mat4(program_get_location(basic_program, "camera.view"), camera_view);
                program_set_vec3(program_get_location(basic_program, "camera.position"), camera->position);

                program_set_vec3(program_get_location(basic_program, "material.color"), objects[i]->material->color);
                program_set_float(program_get_location(basic_program, "material.shininess"), objects[i]->material->shininess);
                program_set_float(program_get_location(basic_program, "material.glow"), objects[i]->material->glow);

                material_bind(objects[i]->material);

                mesh_draw(objects[i]->mesh);

                material_unbind();

                program_unbind();
            }
            break;
            case LIGHTING_PHONG:
            {
                program_bind(phong_program);

                program_set_mat4(program_get_location(phong_program, "object.model"), model);

                program_set_mat4(program_get_location(phong_program, "camera.projection"), camera_projection);
                program_set_mat4(program_get_location(phong_program, "camera.view"), camera_view);
                program_set_vec3(program_get_location(phong_program, "camera.position"), camera->position);

                program_set_vec3(program_get_location(phong_program, "material.color"), objects[i]->material->color);
                program_set_float(program_get_location(phong_program, "material.shininess"), objects[i]->material->shininess);
                program_set_float(program_get_location(phong_program, "material.glow"), objects[i]->material->glow);

                program_set_vec3(program_get_location(phong_program, "ambient"), ambient);

                program_set_vec3(program_get_location(phong_program, "directional_light.direction"), directional_lights[0]->direction);
                program_set_vec3(program_get_location(phong_program, "directional_light.ambient"), directional_lights[0]->ambient);
                program_set_vec3(program_get_location(phong_program, "directional_light.diffuse"), directional_lights[0]->diffuse);
                program_set_vec3(program_get_location(phong_program, "directional_light.specular"), directional_lights[0]->specular);

                program_set_vec3(program_get_location(phong_program, "point_lights[0].position"), point_lights[0]->position);
                program_set_vec3(program_get_location(phong_program, "point_lights[0].ambient"), point_lights[0]->ambient);
                program_set_vec3(program_get_location(phong_program, "point_lights[0].diffuse"), point_lights[0]->diffuse);
                program_set_vec3(program_get_location(phong_program, "point_lights[0].specular"), point_lights[0]->specular);
                program_set_float(program_get_location(phong_program, "point_lights[0].constant"), point_lights[0]->constant);
                program_set_float(program_get_location(phong_program, "point_lights[0].linear"), point_lights[0]->linear);
                program_set_float(program_get_location(phong_program, "point_lights[0].quadratic"), point_lights[0]->quadratic);

                program_set_vec3(program_get_location(phong_program, "point_lights[1].position"), point_lights[1]->position);
                program_set_vec3(program_get_location(phong_program, "point_lights[1].ambient"), point_lights[1]->ambient);
                program_set_vec3(program_get_location(phong_program, "point_lights[1].diffuse"), point_lights[1]->diffuse);
                program_set_vec3(program_get_location(phong_program, "point_lights[1].specular"), point_lights[1]->specular);
                program_set_float(program_get_location(phong_program, "point_lights[1].constant"), point_lights[1]->constant);
                program_set_float(program_get_location(phong_program, "point_lights[1].linear"), point_lights[1]->linear);
                program_set_float(program_get_location(phong_program, "point_lights[1].quadratic"), point_lights[1]->quadratic);

                program_set_vec3(program_get_location(phong_program, "point_lights[2].position"), point_lights[2]->position);
                program_set_vec3(program_get_location(phong_program, "point_lights[2].ambient"), point_lights[2]->ambient);
                program_set_vec3(program_get_location(phong_program, "point_lights[2].diffuse"), point_lights[2]->diffuse);
                program_set_vec3(program_get_location(phong_program, "point_lights[2].specular"), point_lights[2]->specular);
                program_set_float(program_get_location(phong_program, "point_lights[2].constant"), point_lights[2]->constant);
                program_set_float(program_get_location(phong_program, "point_lights[2].linear"), point_lights[2]->linear);
                program_set_float(program_get_location(phong_program, "point_lights[2].quadratic"), point_lights[2]->quadratic);

                program_set_vec3(program_get_location(phong_program, "point_lights[3].position"), point_lights[3]->position);
                program_set_vec3(program_get_location(phong_program, "point_lights[3].ambient"), point_lights[3]->ambient);
                program_set_vec3(program_get_location(phong_program, "point_lights[3].diffuse"), point_lights[3]->diffuse);
                program_set_vec3(program_get_location(phong_program, "point_lights[3].specular"), point_lights[3]->specular);
                program_set_float(program_get_location(phong_program, "point_lights[3].constant"), point_lights[3]->constant);
                program_set_float(program_get_location(phong_program, "point_lights[3].linear"), point_lights[3]->linear);
                program_set_float(program_get_location(phong_program, "point_lights[3].quadratic"), point_lights[3]->quadratic);

                program_set_vec3(program_get_location(phong_program, "spot_light.position"), spot_lights[0]->position);
                program_set_vec3(program_get_location(phong_program, "spot_light.direction"), spot_lights[0]->direction);
                program_set_vec3(program_get_location(phong_program, "spot_light.ambient"), spot_lights[0]->ambient);
                program_set_vec3(program_get_location(phong_program, "spot_light.diffuse"), spot_lights[0]->diffuse);
                program_set_vec3(program_get_location(phong_program, "spot_light.specular"), spot_lights[0]->specular);
                program_set_float(program_get_location(phong_program, "spot_light.constant"), spot_lights[0]->constant);
                program_set_float(program_get_location(phong_program, "spot_light.linear"), spot_lights[0]->linear);
                program_set_float(program_get_location(phong_program, "spot_light.quadratic"), spot_lights[0]->quadratic);
                program_set_float(program_get_location(phong_program, "spot_light.cutOff"), spot_lights[0]->cutOff);
                program_set_float(program_get_location(phong_program, "spot_light.outerCutOff"), spot_lights[0]->outerCutOff);

                material_bind(objects[i]->material);

                mesh_draw(objects[i]->mesh);

                material_unbind();

                program_unbind();
            }
            break;
            case LIGHTING_DEFERRED:
            {
                program_bind(deferred_ambient_program);

                program_set_mat4(program_get_location(deferred_ambient_program, "object.model"), model);

                program_set_mat4(program_get_location(deferred_ambient_program, "camera.projection"), camera_projection);
                program_set_mat4(program_get_location(deferred_ambient_program, "camera.view"), camera_view);
                program_set_vec3(program_get_location(deferred_ambient_program, "camera.position"), camera->position);

                program_set_vec3(program_get_location(deferred_ambient_program, "material.color"), objects[i]->material->color);
                program_set_float(program_get_location(deferred_ambient_program, "material.shininess"), objects[i]->material->shininess);
                program_set_float(program_get_location(deferred_ambient_program, "material.glow"), objects[i]->material->glow);

                program_set_vec3(program_get_location(deferred_ambient_program, "ambient"), ambient);

                material_bind(objects[i]->material);

                mesh_draw(objects[i]->mesh);

                material_unbind();

                program_unbind();

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);

                for (int j = 0; j < num_directional_lights; j++)
                {
                    program_bind(deferred_directional_program);

                    program_set_mat4(program_get_location(deferred_directional_program, "object.model"), model);

                    program_set_mat4(program_get_location(deferred_directional_program, "camera.projection"), camera_projection);
                    program_set_mat4(program_get_location(deferred_directional_program, "camera.view"), camera_view);
                    program_set_vec3(program_get_location(deferred_directional_program, "camera.position"), camera->position);

                    program_set_vec3(program_get_location(deferred_directional_program, "material.color"), objects[i]->material->color);
                    program_set_float(program_get_location(deferred_directional_program, "material.shininess"), objects[i]->material->shininess);
                    program_set_float(program_get_location(deferred_directional_program, "material.glow"), objects[i]->material->glow);

                    program_set_vec3(program_get_location(deferred_directional_program, "directional_light.direction"), directional_lights[j]->direction);
                    program_set_vec3(program_get_location(deferred_directional_program, "directional_light.ambient"), directional_lights[j]->ambient);
                    program_set_vec3(program_get_location(deferred_directional_program, "directional_light.diffuse"), directional_lights[j]->diffuse);
                    program_set_vec3(program_get_location(deferred_directional_program, "directional_light.specular"), directional_lights[j]->specular);

                    material_bind(objects[i]->material);

                    mesh_draw(objects[i]->mesh);

                    material_unbind();

                    program_unbind();
                }

                for (int j = 0; j < num_point_lights; j++)
                {
                    program_bind(deferred_point_program);

                    program_set_mat4(program_get_location(deferred_point_program, "object.model"), model);

                    program_set_mat4(program_get_location(deferred_point_program, "camera.projection"), camera_projection);
                    program_set_mat4(program_get_location(deferred_point_program, "camera.view"), camera_view);
                    program_set_vec3(program_get_location(deferred_point_program, "camera.position"), camera->position);

                    program_set_vec3(program_get_location(deferred_point_program, "material.color"), objects[i]->material->color);
                    program_set_float(program_get_location(deferred_point_program, "material.shininess"), objects[i]->material->shininess);
                    program_set_float(program_get_location(deferred_point_program, "material.glow"), objects[i]->material->glow);

                    program_set_vec3(program_get_location(deferred_point_program, "point_light.position"), point_lights[j]->position);
                    program_set_vec3(program_get_location(deferred_point_program, "point_light.ambient"), point_lights[j]->ambient);
                    program_set_vec3(program_get_location(deferred_point_program, "point_light.diffuse"), point_lights[j]->diffuse);
                    program_set_vec3(program_get_location(deferred_point_program, "point_light.specular"), point_lights[j]->specular);
                    program_set_float(program_get_location(deferred_point_program, "point_light.constant"), point_lights[j]->constant);
                    program_set_float(program_get_location(deferred_point_program, "point_light.linear"), point_lights[j]->linear);
                    program_set_float(program_get_location(deferred_point_program, "point_light.quadratic"), point_lights[j]->quadratic);

                    material_bind(objects[i]->material);

                    mesh_draw(objects[i]->mesh);

                    material_unbind();

                    program_unbind();
                }

                for (int j = 0; j < num_spot_lights; j++)
                {
                    program_bind(deferred_spot_program);

                    program_set_mat4(program_get_location(deferred_spot_program, "object.model"), model);

                    program_set_mat4(program_get_location(deferred_spot_program, "camera.projection"), camera_projection);
                    program_set_mat4(program_get_location(deferred_spot_program, "camera.view"), camera_view);
                    program_set_vec3(program_get_location(deferred_spot_program, "camera.position"), camera->position);

                    program_set_vec3(program_get_location(deferred_spot_program, "material.color"), objects[i]->material->color);
                    program_set_float(program_get_location(deferred_spot_program, "material.shininess"), objects[i]->material->shininess);
                    program_set_float(program_get_location(deferred_spot_program, "material.glow"), objects[i]->material->glow);

                    program_set_vec3(program_get_location(deferred_spot_program, "spot_light.direction"), spot_lights[j]->direction);
                    program_set_vec3(program_get_location(deferred_spot_program, "spot_light.position"), spot_lights[j]->position);
                    program_set_vec3(program_get_location(deferred_spot_program, "spot_light.ambient"), spot_lights[j]->ambient);
                    program_set_vec3(program_get_location(deferred_spot_program, "spot_light.diffuse"), spot_lights[j]->diffuse);
                    program_set_vec3(program_get_location(deferred_spot_program, "spot_light.specular"), spot_lights[j]->specular);
                    program_set_float(program_get_location(deferred_spot_program, "spot_light.constant"), spot_lights[j]->constant);
                    program_set_float(program_get_location(deferred_spot_program, "spot_light.linear"), spot_lights[j]->linear);
                    program_set_float(program_get_location(deferred_spot_program, "spot_light.quadratic"), spot_lights[j]->quadratic);
                    program_set_float(program_get_location(deferred_spot_program, "spot_light.cutOff"), spot_lights[j]->cutOff);
                    program_set_float(program_get_location(deferred_spot_program, "spot_light.outerCutOff"), spot_lights[j]->outerCutOff);

                    material_bind(objects[i]->material);

                    mesh_draw(objects[i]->mesh);

                    material_unbind();

                    program_unbind();
                }

                glDepthFunc(GL_LESS);
                glDepthMask(GL_TRUE);
                glDisable(GL_BLEND);
            }
            break;
            }
        }

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    Mix_FreeChunk(shoot_sound);
    Mix_FreeMusic(background_music);
    for (int i = 0; i < num_spot_lights; i++)
    {
        spot_light_destroy(spot_lights[i]);
    }
    for (int i = 0; i < num_point_lights; i++)
    {
        point_light_destroy(point_lights[i]);
    }
    for (int i = 0; i < num_directional_lights; i++)
    {
        directional_light_destroy(directional_lights[i]);
    }
    for (int i = 0; i < num_objects; i++)
    {
        object_destroy(objects[i]);
    }
    material_destroy(box_material);
    texture_destroy(cobble_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);
    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);
    program_destroy(deferred_spot_program);
    program_destroy(deferred_point_program);
    program_destroy(deferred_directional_program);
    program_destroy(deferred_ambient_program);
    program_destroy(phong_program);
    program_destroy(basic_program);

    // close engine
    image_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
