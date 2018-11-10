// TODO: come up with a name for this library
// TODO: make a pass through all code and clean up names and whatnot (such as using _id for opengl handles)
// TODO: further namespace header guards

#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RENDER_SCALE 1.0f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define FPS_CAP 120u

int main(int argc, char *argv[])
{
    // setup engine
    core_init();
    window_init(
        WINDOW_TITLE,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);
    audio_init(
        MIX_DEFAULT_FREQUENCY,
        MIX_DEFAULT_FORMAT,
        MIX_DEFAULT_CHANNELS,
        1024);
    renderer_init(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        RENDER_SCALE,
        SHADOW_WIDTH,
        SHADOW_HEIGHT);

    time_cap_fps(FPS_CAP);
    // window_set_fullscreen(SDL_WINDOW_FULLSCREEN);
    renderer_set_mode(RENDER_MODE_FORWARD);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // create textures
    struct texture *box_diffuse_texture = texture_create("assets/images/box_diffuse.png");
    struct texture *box_specular_texture = texture_create("assets/images/box_specular.png");
    struct texture *cobble_diffuse_texture = texture_create("assets/images/cobble_diffuse.jpg");
    struct texture *cobble_specular_texture = texture_create("assets/images/cobble_specular.jpg");

    // create materials
    vec3 box_material_color = { 1.0f, 1.0f, 1.0f };
    vec3 cobble_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *box_material = material_create(
        box_material_color,
        box_diffuse_texture,
        box_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);
    struct material *cobble_material = material_create(
        cobble_material_color,
        cobble_diffuse_texture,
        cobble_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    // create skybox
    const char *skybox_cubemap_files[] = {
        "assets/images/sky/right.jpg",
        "assets/images/sky/left.jpg",
        "assets/images/sky/top.jpg",
        "assets/images/sky/bottom.jpg",
        "assets/images/sky/front.jpg",
        "assets/images/sky/back.jpg",
    };

    struct cubemap *skybox_cubemap = cubemap_create(skybox_cubemap_files);

    // create objects
    vec3 floor_object_position = { 0.0f, -2.0f, 0.0f };
    vec3 box_1_object_position = { 0.0f, 1.0f, 0.0f };
    vec3 box_2_object_position = { 2.0f, -0.5f, 0.0f };
    vec3 box_3_object_position = { 0.0f, -0.5f, 2.0f };
    vec3 box_4_object_position = { -2.0f, -0.5f, 0.0f };
    vec3 box_5_object_position = { 0.0f, -0.5f, -2.0f };

    vec3 floor_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_1_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_2_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_3_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_4_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_5_object_rotation = { 0.0f, 0.0f, 0.0f };

    vec3 floor_object_scale = { 10.0f, 1.0f, 10.0f };
    vec3 box_1_object_scale = { 1.0f, 1.0f, 1.0f };
    vec3 box_2_object_scale = { 0.5f, 0.5f, 0.5f };
    vec3 box_3_object_scale = { 0.5f, 0.5f, 0.5f };
    vec3 box_4_object_scale = { 0.5f, 0.5f, 0.5f };
    vec3 box_5_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *floor_object = object_create(
        cube_mesh,
        cobble_material,
        floor_object_position,
        floor_object_rotation,
        floor_object_scale);
    struct object *box_1_object = object_create(
        cube_mesh,
        box_material,
        box_1_object_position,
        box_1_object_rotation,
        box_1_object_scale);
    struct object *box_2_object = object_create(
        cube_mesh,
        box_material,
        box_2_object_position,
        box_2_object_rotation,
        box_2_object_scale);
    struct object *box_3_object = object_create(
        cube_mesh,
        box_material,
        box_3_object_position,
        box_3_object_rotation,
        box_3_object_scale);
    struct object *box_4_object = object_create(
        cube_mesh,
        box_material,
        box_4_object_position,
        box_4_object_rotation,
        box_4_object_scale);
    struct object *box_5_object = object_create(
        cube_mesh,
        box_material,
        box_5_object_position,
        box_5_object_rotation,
        box_5_object_scale);

    struct object *objects[] = {
        floor_object,
        box_1_object,
        box_2_object,
        box_3_object,
        box_4_object,
        box_5_object
    };
    const unsigned int num_objects = sizeof(objects) / sizeof(struct object *);

    // create directional light
    vec3 directional_light_direction = { -0.2f, -1.0f, -0.3f };
    vec3 directional_light_ambient = { 0.1f, 0.1f, 0.1f };
    vec3 directional_light_diffuse = { 0.8f, 0.8f, 0.8f };
    vec3 directional_light_specular = { 1.0f, 1.0f, 1.0f };

    struct directional_light *directional_light = directional_light_create(
        directional_light_direction,
        directional_light_ambient,
        directional_light_diffuse,
        directional_light_specular);

    // create point lights
    vec3 red_point_light_position = { 2.0f, 0.0f, 2.0f };
    vec3 yellow_point_light_position = { -2.0f, 0.0f, -2.0f };
    vec3 green_point_light_position = { 2.0f, 0.0f, -2.0f };
    vec3 blue_point_light_position = { -2.0f, 0.0f, 2.0f };

    vec3 red_point_light_ambient = { 0.1f, 0.0f, 0.0f };
    vec3 yellow_point_light_ambient = { 0.1f, 0.1f, 0.0f };
    vec3 green_point_light_ambient = { 0.0f, 0.1f, 0.0f };
    vec3 blue_point_light_ambient = { 0.0f, 0.0f, 0.1f };

    vec3 red_point_light_diffuse = { 0.8f, 0.0f, 0.0f };
    vec3 yellow_point_light_diffuse = { 0.8f, 0.8f, 0.0f };
    vec3 green_point_light_diffuse = { 0.0f, 0.8f, 0.0f };
    vec3 blue_point_light_diffuse = { 0.0f, 0.0f, 0.8f };

    vec3 red_point_light_specular = { 1.0f, 0.0f, 0.0f };
    vec3 yellow_point_light_specular = { 1.0f, 1.0f, 0.0f };
    vec3 green_point_light_specular = { 0.0f, 1.0f, 0.0f };
    vec3 blue_point_light_specular = { 0.0f, 0.0f, 1.0f };

    vec3 red_point_light_attenuation = { 1.0f, 0.09f, 0.32f };
    vec3 yellow_point_light_attenuation = { 1.0f, 0.09f, 0.32f };
    vec3 green_point_light_attenuation = { 1.0f, 0.09f, 0.32f };
    vec3 blue_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *red_point_light = point_light_create(
        red_point_light_position,
        red_point_light_ambient,
        red_point_light_diffuse,
        red_point_light_specular,
        red_point_light_attenuation);
    struct point_light *yellow_point_light = point_light_create(
        yellow_point_light_position,
        yellow_point_light_ambient,
        yellow_point_light_diffuse,
        yellow_point_light_specular,
        yellow_point_light_attenuation);
    struct point_light *green_point_light = point_light_create(
        green_point_light_position,
        green_point_light_ambient,
        green_point_light_diffuse,
        green_point_light_specular,
        green_point_light_attenuation);
    struct point_light *blue_point_light = point_light_create(
        blue_point_light_position,
        blue_point_light_ambient,
        blue_point_light_diffuse,
        blue_point_light_specular,
        blue_point_light_attenuation);

    struct point_light *point_lights[] = {
        red_point_light,
        yellow_point_light,
        green_point_light,
        blue_point_light };
    const unsigned int num_point_lights = sizeof(point_lights) / sizeof(struct point_light *);

    // create spot lights
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

    // create water
    vec3 water_position = { 0.0f, -2.0f, 0.0f };
    vec2 water_scale = { 20.0f, 20.0f };

    struct water *water = water_create(
        water_position,
        water_scale);

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

    // load music
    struct music *background_music = music_create("assets/audio/background.mp3");

    // load sounds
    struct chunk *shoot_chunk = chunk_create("assets/audio/shoot.wav");

    // game settings
    bool flashlight = true;
    float fps_update_timer = 0.0f;

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

        // update window title
        fps_update_timer += time_delta();

        if (fps_update_timer > 0.25f)
        {
            fps_update_timer = 0.0f;

            char title[256];
            sprintf_s(title, sizeof(title), "%s - FPS: %d", WINDOW_TITLE, time_fps());
            window_set_title(title);
        }

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
                    if (audio_playing_music())
                    {
                        audio_stop_music();
                    }
                    else
                    {
                        music_play(background_music, -1);
                    }
                }
                break;
                case SDLK_2:
                {
                    if (audio_playing_music())
                    {
                        if (audio_paused_music())
                        {
                            audio_resume_music();
                        }
                        else
                        {
                            audio_pause_music();
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

                chunk_play(shoot_chunk, -1, 0);
            }
        }

        // update objects
        float angle = time_current() * 0.001f;
        objects[1]->rotation[0] = angle;
        objects[1]->rotation[1] = angle;
        objects[1]->rotation[2] = angle;

        directional_light->direction[0] = sinf(angle);
        directional_light->direction[2] = cosf(angle);

        // update lights
        glm_vec_copy(camera->position, flashlight_spot_light->position);
        glm_vec_copy(camera->front, flashlight_spot_light->direction);

        // setup renderer
        renderer_set_camera(camera);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            renderer_add_object(objects[i]);
        }

        renderer_set_directional_light(directional_light);

        for (unsigned int i = 0; i < num_point_lights; i++)
        {
            renderer_add_point_light(point_lights[i]);
        }

        if (flashlight)
        {
            renderer_add_spot_light(flashlight_spot_light);
        }

        renderer_set_skybox(skybox_cubemap);

        renderer_add_water(water);

        // render everything
        renderer_draw(false);

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    chunk_destroy(shoot_chunk);

    music_destroy(background_music);

    water_destroy(water);

    spot_light_destroy(flashlight_spot_light);

    point_light_destroy(red_point_light);
    point_light_destroy(yellow_point_light);
    point_light_destroy(green_point_light);
    point_light_destroy(blue_point_light);

    directional_light_destroy(directional_light);

    object_destroy(floor_object);
    object_destroy(box_1_object);
    object_destroy(box_2_object);
    object_destroy(box_3_object);
    object_destroy(box_4_object);
    object_destroy(box_5_object);

    cubemap_destroy(skybox_cubemap);

    material_destroy(box_material);
    material_destroy(cobble_material);

    texture_destroy(box_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(cobble_specular_texture);

    // close engine
    renderer_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
