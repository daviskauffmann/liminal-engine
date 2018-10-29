#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define RENDER_SCALE 1.0f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define FPS_CAP 120

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

    if (renderer_init(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        RENDER_SCALE,
        SHADOW_WIDTH,
        SHADOW_HEIGHT))
    {
        return 1;
    }

    time_cap_fps(FPS_CAP);
    window_set_fullscreen(SDL_WINDOW_FULLSCREEN);
    renderer_set_mode(RENDER_MODE_FORWARD);
    SDL_SetRelativeMouseMode(true);

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
    vec3 box_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *box_material = material_create(
        box_material_color,
        box_diffuse_texture,
        box_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!box_material)
    {
        return 1;
    }

    vec3 cobble_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *cobble_material = material_create(
        cobble_material_color,
        cobble_diffuse_texture,
        cobble_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!cobble_material)
    {
        return 1;
    }

    // create scene
    // TODO: the scene should hold and manage all the objects and lights
    // this would allow for instanced rendering
    vec3 main_scene_sun_direction = { -0.2f, -1.0f, -0.3f };
    vec3 main_scene_sun_ambient = { 0.1f, 0.1f, 0.1f };
    vec3 main_scene_sun_diffuse = { 0.8f, 0.8f, 0.8f };
    vec3 main_scene_sun_specular = { 1.0f, 1.0f, 1.0f };

    struct scene *main_scene = scene_create(
        main_scene_sun_direction,
        main_scene_sun_ambient,
        main_scene_sun_diffuse,
        main_scene_sun_specular);

    if (!main_scene)
    {
        return 1;
    }

    // create objects
    vec3 floor_object_position = { 0.0f, -2.0f, 0.0f };
    vec3 floor_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 floor_object_scale = { 10.0f, 1.0f, 10.0f };

    struct object *floor_object = object_create(
        cube_mesh,
        cobble_material,
        floor_object_position,
        floor_object_rotation,
        floor_object_scale);

    if (!floor_object)
    {
        return 1;
    }

    vec3 box_1_object_position = { 0.0f, 1.0f, 0.0f };
    vec3 box_1_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_1_object_scale = { 1.0f, 1.0f, 1.0f };

    struct object *box_1_object = object_create(
        cube_mesh,
        box_material,
        box_1_object_position,
        box_1_object_rotation,
        box_1_object_scale);

    if (!box_1_object)
    {
        return 1;
    }

    vec3 box_2_object_position = { 2.0f, -0.5f, 0.0f };
    vec3 box_2_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_2_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_2_object = object_create(
        cube_mesh,
        box_material,
        box_2_object_position,
        box_2_object_rotation,
        box_2_object_scale);

    if (!box_2_object)
    {
        return 1;
    }

    vec3 box_3_object_position = { 0.0f, -0.5f, 2.0f };
    vec3 box_3_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_3_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_3_object = object_create(
        cube_mesh,
        box_material,
        box_3_object_position,
        box_3_object_rotation,
        box_3_object_scale);

    if (!box_3_object)
    {
        return 1;
    }

    vec3 box_4_object_position = { -2.0f, -0.5f, 0.0f };
    vec3 box_4_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_4_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_4_object = object_create(
        cube_mesh,
        box_material,
        box_4_object_position,
        box_4_object_rotation,
        box_4_object_scale);

    if (!box_4_object)
    {
        return 1;
    }

    vec3 box_5_object_position = { 0.0f, -0.5f, -2.0f };
    vec3 box_5_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_5_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_5_object = object_create(
        cube_mesh,
        box_material,
        box_5_object_position,
        box_5_object_rotation,
        box_5_object_scale);

    if (!box_5_object)
    {
        return 1;
    }

    struct object *objects[] = {
        floor_object,
        box_1_object,
        box_2_object,
        box_3_object,
        box_4_object,
        box_5_object
    };
    const unsigned int num_objects = sizeof(objects) / sizeof(struct object *);

    // create lights
    vec3 red_point_light_position = { 2.0f, 0.0f, 2.0f };
    vec3 red_point_light_ambient = { 0.1f, 0.0f, 0.0f };
    vec3 red_point_light_diffuse = { 0.8f, 0.0f, 0.0f };
    vec3 red_point_light_specular = { 1.0f, 0.0f, 0.0f };
    vec3 red_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *red_point_light = point_light_create(
        red_point_light_position,
        red_point_light_ambient,
        red_point_light_diffuse,
        red_point_light_specular,
        red_point_light_attenuation);

    if (!red_point_light)
    {
        return 1;
    }

    vec3 yellow_point_light_position = { -2.0f, 0.0f, -2.0f };
    vec3 yellow_point_light_ambient = { 0.1f, 0.1f, 0.0f };
    vec3 yellow_point_light_diffuse = { 0.8f, 0.8f, 0.0f };
    vec3 yellow_point_light_specular = { 1.0f, 1.0f, 0.0f };
    vec3 yellow_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *yellow_point_light = point_light_create(
        yellow_point_light_position,
        yellow_point_light_ambient,
        yellow_point_light_diffuse,
        yellow_point_light_specular,
        yellow_point_light_attenuation);

    if (!yellow_point_light)
    {
        return 1;
    }

    vec3 green_point_light_position = { 2.0f, 0.0f, -2.0f };
    vec3 green_point_light_ambient = { 0.0f, 0.1f, 0.0f };
    vec3 green_point_light_diffuse = { 0.0f, 0.8f, 0.0f };
    vec3 green_point_light_specular = { 0.0f, 1.0f, 0.0f };
    vec3 green_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *green_point_light = point_light_create(
        green_point_light_position,
        green_point_light_ambient,
        green_point_light_diffuse,
        green_point_light_specular,
        green_point_light_attenuation);

    if (!green_point_light)
    {
        return 1;
    }

    vec3 blue_point_light_position = { -2.0f, 0.0f, 2.0f };
    vec3 blue_point_light_ambient = { 0.0f, 0.0f, 0.1f };
    vec3 blue_point_light_diffuse = { 0.0f, 0.0f, 0.8f };
    vec3 blue_point_light_specular = { 0.0f, 0.0f, 1.0f };
    vec3 blue_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *blue_point_light = point_light_create(
        blue_point_light_position,
        blue_point_light_ambient,
        blue_point_light_diffuse,
        blue_point_light_specular,
        blue_point_light_attenuation);

    if (!blue_point_light)
    {
        return 1;
    }

    struct point_light *point_lights[] = {
        red_point_light,
        yellow_point_light,
        green_point_light,
        blue_point_light };
    const unsigned int num_point_lights = sizeof(point_lights) / sizeof(struct point_light *);

    vec3 flashlight_spot_light_position = { 0.0f, 0.0f, 0.0f };
    vec3 flashlight_spot_light_direction = { 0.0f, 0.0f, 0.0f };
    vec3 flashlight_spot_light_ambient = { 0.5f, 0.5f, 0.5f };
    vec3 flashlight_spot_light_diffuse = { 1.0f, 1.0f, 1.0f };
    vec3 flashlight_spot_light_specular = { 1.0f, 1.0f, 1.0f };
    vec3 flashlight_spot_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct spot_light *flashlight_spot_light = spot_light_create(
        flashlight_spot_light_position,
        flashlight_spot_light_direction,
        flashlight_spot_light_ambient,
        flashlight_spot_light_diffuse,
        flashlight_spot_light_specular,
        flashlight_spot_light_attenuation,
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    if (!flashlight_spot_light)
    {
        return 1;
    }

    // create camera
    vec3 camera_position = { 0.0f, 0.0f, 3.0f };
    vec3 camera_front = { 0.0f, 0.0f, -1.0f };
    vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    struct camera *camera = camera_create(
        camera_position,
        camera_front,
        camera_up,
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

    // game settings
    bool flashlight = true;

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

        // update window title
        char title[256];
        sprintf_s(title, sizeof(title), "%s - FPS: %d", WINDOW_TITLE, time_fps());
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
                    renderer_set_mode(RENDER_MODE_FORWARD);
                }
                break;
                case SDLK_F2:
                {
                    renderer_set_mode(RENDER_MODE_DEFERRED);
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
                case SDLK_f:
                {
                    flashlight = !flashlight;
                }
                break;
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        unsigned int flags = window_get_flags();

                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        {
                            window_set_fullscreen(0);
                        }
                        else
                        {
                            window_set_fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
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
                    sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)) };
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

        main_scene->sun_direction[0] = sinf(angle);
        main_scene->sun_direction[2] = cosf(angle);

        // update lights
        glm_vec_copy(camera->position, flashlight_spot_light->position);
        glm_vec_copy(camera->front, flashlight_spot_light->direction);

        // setup renderer
        renderer_set_camera(camera);
        renderer_set_scene(main_scene);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            renderer_add_object(objects[i]);
        }

        for (unsigned int i = 0; i < num_point_lights; i++)
        {
            renderer_add_point_light(point_lights[i]);
        }

        if (flashlight)
        {
            renderer_add_spot_light(flashlight_spot_light);
        }

        // render everything
        renderer_draw();

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    Mix_FreeChunk(shoot_sound);
    Mix_FreeMusic(background_music);

    spot_light_destroy(flashlight_spot_light);

    point_light_destroy(red_point_light);
    point_light_destroy(green_point_light);
    point_light_destroy(blue_point_light);
    point_light_destroy(yellow_point_light);

    object_destroy(floor_object);
    object_destroy(box_1_object);
    object_destroy(box_2_object);
    object_destroy(box_3_object);
    object_destroy(box_4_object);
    object_destroy(box_5_object);

    scene_destroy(main_scene);

    material_destroy(cobble_material);
    material_destroy(box_material);

    texture_destroy(cobble_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);

    // close engine
    renderer_quit();
    audio_quit();
    window_quit();
    engine_quit();

    return 0;
}
