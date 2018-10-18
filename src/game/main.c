#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS_CAP 60

int main(int argc, char *argv[])
{
    // setup engine
    if (engine_init())
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

    time_cap_fps(FPS_CAP);

    SDL_SetRelativeMouseMode(true);

    // create meshes
    struct mesh *monkey_mesh = mesh_create_obj("assets/models/monkey.obj");

    if (!monkey_mesh)
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

    struct texture *matrix_texture = texture_create("assets/images/matrix.jpg");

    if (!matrix_texture)
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

    // create objects
    struct object *objects[] = {
        object_create(
            cube_mesh,
            box_material,
            (vec3){0.0f, 0.0f, 0.0f},
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
    struct directional_light *directional_light = directional_light_create(
        (vec3){-0.2f, -1.0f, -0.3f},
        (vec3){0.05f, 0.05f, 0.05f},
        (vec3){0.4f, 0.4f, 0.4f},
        (vec3){1.0f, 0.0f, 0.0f});

    if (!directional_light)
    {
        return 1;
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

    struct spot_light *spot_light = spot_light_create(
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 0.0f, 0.0f},
        1.0f,
        0.09f,
        0.032f,
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    if (!spot_light)
    {
        return 1;
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

    // cache uniform locations
    GLint basic_program_time = program_get_location(basic_program, "time");
    GLint basic_program_material_diffuse = program_get_location(basic_program, "material.diffuse");
    GLint basic_program_material_color = program_get_location(basic_program, "material.color");
    GLint basic_program_object_model = program_get_location(basic_program, "object.model");
    GLint basic_program_camera_projection = program_get_location(basic_program, "camera.projection");
    GLint basic_program_camera_view = program_get_location(basic_program, "camera.view");
    GLint basic_program_camera_position = program_get_location(basic_program, "camera.position");

    GLint phong_program_time = program_get_location(phong_program, "time");
    GLint phong_program_material_diffuse = program_get_location(phong_program, "material.diffuse");
    GLint phong_program_material_specular = program_get_location(phong_program, "material.specular");
    GLint phong_program_material_emission = program_get_location(phong_program, "material.emission");
    GLint phong_program_material_color = program_get_location(phong_program, "material.color");
    GLint phong_program_material_shininess = program_get_location(phong_program, "material.shininess");
    GLint phong_program_material_glow = program_get_location(phong_program, "material.glow");
    GLint phong_program_object_model = program_get_location(phong_program, "object.model");
    GLint phong_program_camera_projection = program_get_location(phong_program, "camera.projection");
    GLint phong_program_camera_view = program_get_location(phong_program, "camera.view");
    GLint phong_program_camera_position = program_get_location(phong_program, "camera.position");
    GLint phong_program_directional_light_direction = program_get_location(phong_program, "directional_light.direction");
    GLint phong_program_directional_light_ambient = program_get_location(phong_program, "directional_light.ambient");
    GLint phong_program_directional_light_diffuse = program_get_location(phong_program, "directional_light.diffuse");
    GLint phong_program_directional_light_specular = program_get_location(phong_program, "directional_light.specular");
    GLint phong_program_point_lights_0_position = program_get_location(phong_program, "point_lights[0].position");
    GLint phong_program_point_lights_0_ambient = program_get_location(phong_program, "point_lights[0].ambient");
    GLint phong_program_point_lights_0_diffuse = program_get_location(phong_program, "point_lights[0].diffuse");
    GLint phong_program_point_lights_0_specular = program_get_location(phong_program, "point_lights[0].specular");
    GLint phong_program_point_lights_0_constant = program_get_location(phong_program, "point_lights[0].constant");
    GLint phong_program_point_lights_0_linear = program_get_location(phong_program, "point_lights[0].linear");
    GLint phong_program_point_lights_0_quadratic = program_get_location(phong_program, "point_lights[0].quadratic");
    GLint phong_program_point_lights_1_position = program_get_location(phong_program, "point_lights[1].position");
    GLint phong_program_point_lights_1_ambient = program_get_location(phong_program, "point_lights[1].ambient");
    GLint phong_program_point_lights_1_diffuse = program_get_location(phong_program, "point_lights[1].diffuse");
    GLint phong_program_point_lights_1_specular = program_get_location(phong_program, "point_lights[1].specular");
    GLint phong_program_point_lights_1_constant = program_get_location(phong_program, "point_lights[1].constant");
    GLint phong_program_point_lights_1_linear = program_get_location(phong_program, "point_lights[1].linear");
    GLint phong_program_point_lights_1_quadratic = program_get_location(phong_program, "point_lights[1].quadratic");
    GLint phong_program_point_lights_2_position = program_get_location(phong_program, "point_lights[2].position");
    GLint phong_program_point_lights_2_ambient = program_get_location(phong_program, "point_lights[2].ambient");
    GLint phong_program_point_lights_2_diffuse = program_get_location(phong_program, "point_lights[2].diffuse");
    GLint phong_program_point_lights_2_specular = program_get_location(phong_program, "point_lights[2].specular");
    GLint phong_program_point_lights_2_constant = program_get_location(phong_program, "point_lights[2].constant");
    GLint phong_program_point_lights_2_linear = program_get_location(phong_program, "point_lights[2].linear");
    GLint phong_program_point_lights_2_quadratic = program_get_location(phong_program, "point_lights[2].quadratic");
    GLint phong_program_point_lights_3_position = program_get_location(phong_program, "point_lights[3].position");
    GLint phong_program_point_lights_3_ambient = program_get_location(phong_program, "point_lights[3].ambient");
    GLint phong_program_point_lights_3_diffuse = program_get_location(phong_program, "point_lights[3].diffuse");
    GLint phong_program_point_lights_3_specular = program_get_location(phong_program, "point_lights[3].specular");
    GLint phong_program_point_lights_3_constant = program_get_location(phong_program, "point_lights[3].constant");
    GLint phong_program_point_lights_3_linear = program_get_location(phong_program, "point_lights[3].linear");
    GLint phong_program_point_lights_3_quadratic = program_get_location(phong_program, "point_lights[3].quadratic");
    GLint phong_program_spot_light_position = program_get_location(phong_program, "spot_light.position");
    GLint phong_program_spot_light_direction = program_get_location(phong_program, "spot_light.direction");
    GLint phong_program_spot_light_ambient = program_get_location(phong_program, "spot_light.ambient");
    GLint phong_program_spot_light_diffuse = program_get_location(phong_program, "spot_light.diffuse");
    GLint phong_program_spot_light_specular = program_get_location(phong_program, "spot_light.specular");
    GLint phong_program_spot_light_constant = program_get_location(phong_program, "spot_light.constant");
    GLint phong_program_spot_light_linear = program_get_location(phong_program, "spot_light.linear");
    GLint phong_program_spot_light_quadratic = program_get_location(phong_program, "spot_light.quadratic");
    GLint phong_program_spot_light_cutOff = program_get_location(phong_program, "spot_light.cutOff");
    GLint phong_program_spot_light_outerCutOff = program_get_location(phong_program, "spot_light.outerCutOff");

    // setup shader samplers
    program_bind(basic_program);
    program_set_int(basic_program_material_diffuse, 0);
    program_unbind();

    program_bind(phong_program);
    program_set_int(phong_program_material_diffuse, 0);
    program_set_int(phong_program_material_specular, 1);
    program_set_int(phong_program_material_emission, 2);
    program_unbind();

    // used for swiching shaders
    struct program *current_program = phong_program;

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
                    current_program = basic_program;
                }
                break;
                case SDLK_F2:
                {
                    current_program = phong_program;
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

        // update lights
        glm_vec_copy(camera->position, spot_light->position);
        glm_vec_copy(camera->front, spot_light->direction);

        // calculate matrices
        mat4 camera_projection;
        camera_calc_projection(camera, camera_projection);

        mat4 camera_view;
        camera_calc_view(camera, camera_view);

        // update shaders
        program_bind(basic_program);
        program_set_int(basic_program_time, time_current());
        program_set_mat4(basic_program_camera_projection, camera_projection);
        program_set_mat4(basic_program_camera_view, camera_view);
        program_set_vec3(basic_program_camera_position, camera->position);
        program_unbind();

        program_bind(phong_program);
        program_set_int(phong_program_time, time_current());
        program_set_mat4(phong_program_camera_projection, camera_projection);
        program_set_mat4(phong_program_camera_view, camera_view);
        program_set_vec3(phong_program_camera_position, camera->position);
        program_set_vec3(phong_program_directional_light_direction, directional_light->direction);
        program_set_vec3(phong_program_directional_light_ambient, directional_light->ambient);
        program_set_vec3(phong_program_directional_light_diffuse, directional_light->diffuse);
        program_set_vec3(phong_program_directional_light_specular, directional_light->specular);
        program_set_vec3(phong_program_point_lights_0_position, point_lights[0]->position);
        program_set_vec3(phong_program_point_lights_0_ambient, point_lights[0]->ambient);
        program_set_vec3(phong_program_point_lights_0_diffuse, point_lights[0]->diffuse);
        program_set_vec3(phong_program_point_lights_0_specular, point_lights[0]->specular);
        program_set_float(phong_program_point_lights_0_constant, point_lights[0]->constant);
        program_set_float(phong_program_point_lights_0_linear, point_lights[0]->linear);
        program_set_float(phong_program_point_lights_0_quadratic, point_lights[0]->quadratic);
        program_set_vec3(phong_program_point_lights_1_position, point_lights[1]->position);
        program_set_vec3(phong_program_point_lights_1_ambient, point_lights[1]->ambient);
        program_set_vec3(phong_program_point_lights_1_diffuse, point_lights[1]->diffuse);
        program_set_vec3(phong_program_point_lights_1_specular, point_lights[1]->specular);
        program_set_float(phong_program_point_lights_1_constant, point_lights[1]->constant);
        program_set_float(phong_program_point_lights_1_linear, point_lights[1]->linear);
        program_set_float(phong_program_point_lights_1_quadratic, point_lights[1]->quadratic);
        program_set_vec3(phong_program_point_lights_2_position, point_lights[2]->position);
        program_set_vec3(phong_program_point_lights_2_ambient, point_lights[2]->ambient);
        program_set_vec3(phong_program_point_lights_2_diffuse, point_lights[2]->diffuse);
        program_set_vec3(phong_program_point_lights_2_specular, point_lights[2]->specular);
        program_set_float(phong_program_point_lights_2_constant, point_lights[2]->constant);
        program_set_float(phong_program_point_lights_2_linear, point_lights[2]->linear);
        program_set_float(phong_program_point_lights_2_quadratic, point_lights[2]->quadratic);
        program_set_vec3(phong_program_point_lights_3_position, point_lights[3]->position);
        program_set_vec3(phong_program_point_lights_3_ambient, point_lights[3]->ambient);
        program_set_vec3(phong_program_point_lights_3_diffuse, point_lights[3]->diffuse);
        program_set_vec3(phong_program_point_lights_3_specular, point_lights[3]->specular);
        program_set_float(phong_program_point_lights_3_constant, point_lights[3]->constant);
        program_set_float(phong_program_point_lights_3_linear, point_lights[3]->linear);
        program_set_float(phong_program_point_lights_3_quadratic, point_lights[3]->quadratic);
        program_set_vec3(phong_program_spot_light_position, spot_light->position);
        program_set_vec3(phong_program_spot_light_direction, spot_light->direction);
        program_set_vec3(phong_program_spot_light_ambient, spot_light->ambient);
        program_set_vec3(phong_program_spot_light_diffuse, spot_light->diffuse);
        program_set_vec3(phong_program_spot_light_specular, spot_light->specular);
        program_set_float(phong_program_spot_light_constant, spot_light->constant);
        program_set_float(phong_program_spot_light_linear, spot_light->linear);
        program_set_float(phong_program_spot_light_quadratic, spot_light->quadratic);
        program_set_float(phong_program_spot_light_cutOff, spot_light->cutOff);
        program_set_float(phong_program_spot_light_outerCutOff, spot_light->outerCutOff);
        program_unbind();

        // update objects
        for (int i = 0; i < num_objects; i++)
        {
            float angle = time_current() * 0.001f;
            objects[i]->rotation[0] = angle;
            objects[i]->rotation[1] = angle;
            objects[i]->rotation[2] = angle;
        }

        // clear the window
        window_clear();

        // draw objects
        for (int i = 0; i < num_objects; i++)
        {
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            program_bind(basic_program);
            program_set_mat4(basic_program_object_model, model);
            program_set_vec3(basic_program_material_color, objects[i]->material->color);
            program_unbind();

            program_bind(phong_program);
            program_set_mat4(phong_program_object_model, model);
            program_set_vec3(phong_program_material_color, objects[i]->material->color);
            program_set_float(phong_program_material_shininess, objects[i]->material->shininess);
            program_set_float(phong_program_material_glow, objects[i]->material->glow);
            program_unbind();

            program_bind(current_program);
            object_draw(objects[i]);
            program_unbind();
        }

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    Mix_FreeChunk(shoot_sound);
    Mix_FreeMusic(background_music);
    directional_light_destroy(directional_light);
    for (int i = 0; i < num_point_lights; i++)
    {
        point_light_destroy(point_lights[i]);
    }
    spot_light_destroy(spot_light);
    for (int i = 0; i < num_objects; i++)
    {
        object_destroy(objects[i]);
    }
    material_destroy(box_material);
    texture_destroy(matrix_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);
    mesh_destroy(monkey_mesh);

    // close engine
    audio_quit();
    window_quit();
    engine_quit();

    return 0;
}
