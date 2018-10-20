#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

#define FPS_CAP 60

enum lighting
{
    LIGHTING_BASIC,
    LIGHTING_PHONG
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
        "assets/shaders/phong.vs",
        "assets/shaders/phong.fs");

    if (!phong_program)
    {
        return 1;
    }

    struct program *post_program = program_create(
        "assets/shaders/post.vs",
        "assets/shaders/post.fs");

    if (!post_program)
    {
        return 1;
    }

    struct program *depth_program = program_create(
        "assets/shaders/depth.vs",
        "assets/shaders/depth.fs");

    if (!depth_program)
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

    // setup screen fbo
    GLuint screen_fbo;
    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    GLuint screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint screen_rbo;
    glGenRenderbuffers(1, &screen_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screen_rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete screen framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    GLuint depthmap_fbo;
    glGenFramebuffers(1, &depthmap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

    GLuint depthmap_texture;
    glGenTextures(1, &depthmap_texture);
    glBindTexture(GL_TEXTURE_2D, depthmap_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete depthmap framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // used for switching shaders
    enum lighting lighting = LIGHTING_PHONG;

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
                    lighting = LIGHTING_BASIC;
                }
                break;
                case SDLK_F2:
                {
                    lighting = LIGHTING_PHONG;
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

        // bind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

        // draw scene
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // calculate sun projection matrix
        mat4 sun_projection;
        glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f, sun_projection);

        // calculate sun view matrix
        mat4 sun_view;
        glm_lookat((vec3){-2.0f, 4.0f, -1.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, sun_view);

        // draw objects
        for (int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            program_bind(depth_program);

            program_set_mat4(program_get_location(basic_program, "sun.projection"), sun_projection);
            program_set_mat4(program_get_location(basic_program, "sun.view"), sun_view);

            program_set_mat4(program_get_location(basic_program, "object.model"), model);

            object_draw(objects[i]);

            program_unbind();
        }

        // unbind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

        // draw scene
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // calculate camera projection matrix
        mat4 camera_projection;
        camera_calc_perspective(camera, camera_projection);

        // calculate camera view matrix
        mat4 camera_view;
        camera_calc_view(camera, camera_view);

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

                program_set_mat4(program_get_location(basic_program, "camera.projection"), camera_projection);
                program_set_mat4(program_get_location(basic_program, "camera.view"), camera_view);
                program_set_vec3(program_get_location(basic_program, "camera.position"), camera->position);

                program_set_mat4(program_get_location(basic_program, "object.model"), model);

                program_set_int(program_get_location(basic_program, "material.diffuse"), 0);
                program_set_vec3(program_get_location(basic_program, "material.color"), objects[i]->material->color);

                object_draw(objects[i]);

                program_unbind();
            }
            break;
            case LIGHTING_PHONG:
            {
                program_bind(phong_program);

                program_set_mat4(program_get_location(phong_program, "camera.projection"), camera_projection);
                program_set_mat4(program_get_location(phong_program, "camera.view"), camera_view);
                program_set_vec3(program_get_location(phong_program, "camera.position"), camera->position);

                program_set_mat4(program_get_location(phong_program, "object.model"), model);

                program_set_int(program_get_location(phong_program, "material.diffuse"), 0);
                program_set_int(program_get_location(phong_program, "material.specular"), 1);
                program_set_int(program_get_location(phong_program, "material.emission"), 2);
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

                object_draw(objects[i]);

                program_unbind();
            }
            break;
            }
        }

        // unbind fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // post processing
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        program_bind(post_program);

        program_set_int(program_get_location(post_program, "screen.texture"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screen_texture);

        mesh_draw(quad_mesh);

        glBindTexture(GL_TEXTURE_2D, 0);

        program_unbind();

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    glDeleteRenderbuffers(1, &screen_rbo);
    glDeleteTextures(1, &screen_texture);
    glDeleteFramebuffers(1, &screen_fbo);

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
    program_destroy(depth_program);
    program_destroy(post_program);
    program_destroy(phong_program);
    program_destroy(basic_program);

    // close engine
    image_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
