#include <engine/engine.h>

#define WINDOW_TITLE "Project Kilonova"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RENDER_SCALE 1.0f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define FPS_CAP 300
#define FRAME_DELAY (1000 / FPS_CAP)

int main(int argc, char *argv[])
{
    // init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(0);
    TTF_Init();
    SDLNet_Init();

    // create window
    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // create OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // setup audio
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

    // setup renderer and audio
    renderer_init(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        RENDER_SCALE,
        SHADOW_WIDTH,
        SHADOW_HEIGHT);
    audio_init();

    // create textures
    struct texture *box_diffuse_texture = texture_create("assets/images/box_diffuse.png");
    struct texture *box_specular_texture = texture_create("assets/images/box_specular.png");
    struct texture *cobble_diffuse_texture = texture_create("assets/images/cobble_diffuse.jpg");
    struct texture *cobble_specular_texture = texture_create("assets/images/cobble_specular.jpg");
    struct texture *grass_texture = texture_create("assets/images/grass.png");

    // create cubemaps
    const char *skybox_filenames[] = {
        "assets/images/sky/right.jpg",
        "assets/images/sky/left.jpg",
        "assets/images/sky/top.jpg",
        "assets/images/sky/bottom.jpg",
        "assets/images/sky/front.jpg",
        "assets/images/sky/back.jpg"
    };
    struct cubemap *skybox_cubemap = cubemap_create(skybox_filenames);

    // create sounds
    struct sound *bounce_sound = sound_create("assets/audio/bounce.wav");
    struct sound *shoot_sound = sound_create("assets/audio/shoot.wav");

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

    // create objects
    vec3 floor_object_position = { 0.0f, -4.0f, 0.0f };
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
        assets.cube_mesh,
        cobble_material,
        floor_object_position,
        floor_object_rotation,
        floor_object_scale);
    struct object *box_1_object = object_create(
        assets.cube_mesh,
        assets.default_material,
        box_1_object_position,
        box_1_object_rotation,
        box_1_object_scale);
    struct object *box_2_object = object_create(
        assets.cube_mesh,
        box_material,
        box_2_object_position,
        box_2_object_rotation,
        box_2_object_scale);
    struct object *box_3_object = object_create(
        assets.cube_mesh,
        box_material,
        box_3_object_position,
        box_3_object_rotation,
        box_3_object_scale);
    struct object *box_4_object = object_create(
        assets.cube_mesh,
        box_material,
        box_4_object_position,
        box_4_object_rotation,
        box_4_object_scale);
    struct object *box_5_object = object_create(
        assets.cube_mesh,
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

    // create sun
    vec3 sun_direction = { -0.2f, -1.0f, -0.3f };
    vec3 sun_ambient = { 0.1f, 0.1f, 0.1f };
    vec3 sun_diffuse = { 0.8f, 0.8f, 0.8f };
    vec3 sun_specular = { 1.0f, 1.0f, 1.0f };

    struct sun *sun = sun_create(
        sun_direction,
        sun_ambient,
        sun_diffuse,
        sun_specular);

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
    vec3 torch_spot_light_position = { 0.0f, 0.0f, 0.0f };
    vec3 torch_spot_light_direction = { 0.0f, 0.0f, 0.0f };
    vec3 torch_spot_light_ambient = { 0.5f, 0.5f, 0.5f };
    vec3 torch_spot_light_diffuse = { 1.0f, 1.0f, 1.0f };
    vec3 torch_spot_light_specular = { 1.0f, 1.0f, 1.0f };
    vec3 torch_spot_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct spot_light *torch_spot_light = spot_light_create(
        torch_spot_light_position,
        torch_spot_light_direction,
        torch_spot_light_ambient,
        torch_spot_light_diffuse,
        torch_spot_light_specular,
        torch_spot_light_attenuation,
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    // create water
    vec3 test_water_position = { 0.0f, -2.0f, 0.0f };
    vec2 test_water_scale = { 100.0f, 100.0f };

    struct water *test_water = water_create(
        test_water_position,
        test_water_scale);

    vec3 grass_sprite_color = { 1.0f, 1.0f, 1.0f };
    vec2 grass_sprite_position = { 0.0f, 0.0f };
    float grass_sprite_rotation = 0.0f;
    vec2 grass_sprite_scale = { 1.0f, 1.0f };

    struct sprite *grass_sprite = sprite_create(
        grass_sprite_color,
        grass_texture,
        grass_sprite_position,
        grass_sprite_rotation,
        grass_sprite_scale);

    // create camera
    vec3 main_camera_position = { 0.0f, 0.0f, 3.0f };

    struct camera *main_camera = camera_create(
        main_camera_position,
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    if (!main_camera)
    {
        printf("Failed to create main camera\n");

        return 1;
    }

    // create sources
    struct source *origin_source = source_create();
    struct source *camera_source = source_create();

    // game settings
    unsigned int current_time = 0;
    float fps_update_timer = 0.0f;
    bool torch = true;
    float bounce_timer = 0.0f;
    float shoot_timer = 0.0f;

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);
    renderer_set_mode(RENDER_MODE_DEFERRED);

    // main loop
    bool quit = false;
    while (!quit)
    {
        // timer for fps cap
        unsigned int frame_start = SDL_GetTicks();

        // calculate time passed since last frame
        unsigned int previous_time = current_time;
        current_time = frame_start;

        // calculate delta time and fps
        float delta_time = (current_time - previous_time) / 1000.0f;
        unsigned int fps = (unsigned int)(1 / delta_time);

        // update window title
        fps_update_timer += delta_time;

        if (fps_update_timer > 0.25f)
        {
            fps_update_timer = 0.0f;

            char title[256];
            sprintf_s(title, sizeof(title), "%s - FPS: %d", WINDOW_TITLE, fps);
            SDL_SetWindowTitle(window, title);
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
                    torch = !torch;
                }
                break;
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        unsigned int flags = SDL_GetWindowFlags(window);

                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        {
                            SDL_SetWindowFullscreen(window, 0);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
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
                main_camera->pitch -= event.motion.yrel * 0.1f;
                main_camera->yaw += event.motion.xrel * 0.1f;

                if (main_camera->pitch > 89.0f)
                {
                    main_camera->pitch = 89.0f;
                }
                if (main_camera->pitch < -89.0f)
                {
                    main_camera->pitch = -89.0f;
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                // zoom
                if (main_camera->fov >= 1.0f && main_camera->fov <= 90.0f)
                {
                    main_camera->fov -= event.wheel.y;
                }
                if (main_camera->fov <= 1.0f)
                {
                    main_camera->fov = 1.0f;
                }
                if (main_camera->fov >= 90.0f)
                {
                    main_camera->fov = 90.0f;
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

                    SDL_SetWindowSize(window, width, height);

                    printf("Window resized to %dx%d\n", width, height);
                }
                break;
                }
            }
            break;
            }
        }

        // calculate movement speed
        float speed = 5.0f * delta_time;

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

        // calculate camera vectors
        vec3 main_camera_front;
        camera_calc_front(main_camera, &main_camera_front);

        vec3 main_camera_up;
        camera_calc_up(main_camera, &main_camera_up);

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            vec3 movement;
            glm_vec_scale(main_camera_front, speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            vec3 direction;
            glm_cross(main_camera_front, main_camera_up, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, -speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            vec3 movement;
            glm_vec_scale(main_camera_front, -speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            vec3 direction;
            glm_cross(main_camera_front, main_camera_up, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        if (keys[SDL_SCANCODE_SPACE])
        {
            vec3 movement;
            glm_vec_scale(main_camera_up, speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        if (keys[SDL_SCANCODE_LCTRL])
        {
            vec3 movement;
            glm_vec_scale(main_camera_up, -speed, movement);
            glm_vec_add(main_camera->position, movement, main_camera->position);
        }

        // calculate angle for rotating stuff
        float angle = current_time * 0.001f;
        float angle_sin = sinf(angle);
        float angle_cos = cosf(angle);

        // update objects
        box_1_object->rotation[0] = angle_sin;
        box_1_object->rotation[1] = angle_cos;

        // update lights
        sun->direction[0] = angle_sin;
        sun->direction[2] = angle_cos;

        glm_vec_copy(main_camera->position, torch_spot_light->position);
        glm_vec_copy(main_camera_front, torch_spot_light->direction);

        // update audio
        vec3 camera_velocity = GLM_VEC3_ZERO_INIT;
        glm_vec_scale(main_camera_front, speed, camera_velocity);
        vec3 camera_orientation[2];
        glm_vec_copy(main_camera_front, camera_orientation[0]);
        glm_vec_copy(main_camera_up, camera_orientation[1]);

        audio_set_listener(main_camera->position, camera_velocity, camera_orientation);

        // update sources
        source_set_position(camera_source, main_camera->position);

        // shooting
        shoot_timer += delta_time;

        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                source_play(camera_source, shoot_sound);
            }
        }

        // 3d audio test
        bounce_timer += delta_time;

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (bounce_timer >= 0.25f)
            {
                bounce_timer = 0.0f;

                source_play(origin_source, bounce_sound);
            }
        }

        // setup renderer
        for (unsigned int i = 0; i < num_objects; i++)
        {
            renderer_add_object(objects[i]);
        }

        renderer_set_sun(sun);

        for (unsigned int i = 0; i < num_point_lights; i++)
        {
            renderer_add_point_light(point_lights[i]);
        }

        if (torch)
        {
            renderer_add_spot_light(torch_spot_light);
        }

        renderer_set_skybox(skybox_cubemap);

        renderer_add_water(test_water);

        // renderer_add_sprite(grass_sprite);

        // render everything
        renderer_draw(main_camera, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, SDL_GetTicks(), delta_time);

        // display the window
        SDL_GL_SwapWindow(window);

        // apply fps cap
        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;

        if (FRAME_DELAY > frame_time)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    // free resources
    source_destroy(camera_source);
    source_destroy(origin_source);

    water_destroy(test_water);

    camera_destroy(main_camera);

    spot_light_destroy(torch_spot_light);

    point_light_destroy(blue_point_light);
    point_light_destroy(green_point_light);
    point_light_destroy(yellow_point_light);
    point_light_destroy(red_point_light);

    sun_destroy(sun);

    object_destroy(box_5_object);
    object_destroy(box_4_object);
    object_destroy(box_3_object);
    object_destroy(box_2_object);
    object_destroy(box_1_object);
    object_destroy(floor_object);

    cubemap_destroy(skybox_cubemap);

    material_destroy(cobble_material);
    material_destroy(box_material);

    sound_destroy(shoot_sound);
    sound_destroy(bounce_sound);

    texture_destroy(cobble_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);

    // close engine
    audio_quit();
    renderer_quit();

    // close audio
    Mix_CloseAudio();

    // close window
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    // close SDL
    SDLNet_Quit();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}