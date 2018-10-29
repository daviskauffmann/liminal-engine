#include <engine/engine.h>

#define WINDOW_TITLE "Breakout"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

enum game_state
{
    GAME_STATE_ACTIVE,
    GAME_STATE_MENU,
    GAME_STATE_WIN
};

static enum game_state game_state;

int main(int argc, char *argv[])
{
    // init engine
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

    if (sprite_init())
    {
        return 1;
    }

    // create programs
    struct program *basic_program = program_create(
        "assets/shaders/basic.vert",
        "assets/shaders/basic.frag");

    if (!basic_program)
    {
        return 1;
    }

    // setup shader samplers
    program_bind(basic_program);
    program_set_int(basic_program, "sprite.image", 0);
    program_unbind();

    // create textures
    struct texture *awesomeface_texture = texture_create("assets/images/awesomeface.png");

    if (!awesomeface_texture)
    {
        return 1;
    }

    // create sprites
    vec3 awesomeface_sprite_color = { 1.0f, 1.0f, 1.0f };
    vec2 awesomeface_sprite_position = { 0.0f, 0.0f };
    float awesomeface_sprite_rotation = 0.0f;
    vec2 awesomeface_sprite_scale = { 1.0f, 1.0f };

    struct sprite *awesomeface_sprite = sprite_create(
        awesomeface_sprite_color,
        awesomeface_texture,
        awesomeface_sprite_position,
        awesomeface_sprite_rotation,
        awesomeface_sprite_scale);

    if (!awesomeface_sprite)
    {
        return 1;
    }

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

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
        }

        awesomeface_sprite->rotation += time_delta();

        // calculate camera projection
        mat4 camera_projection;
        glm_ortho_default(window_get_aspect(), camera_projection);

        // TEST: draw awesomeface sprite

        // calculate sprite model
        mat4 awesomeface_sprite_model = GLM_MAT4_IDENTITY_INIT;
        sprite_calc_model(awesomeface_sprite, awesomeface_sprite_model);

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        program_bind(basic_program);

        program_set_mat4(basic_program, "camera.projection", camera_projection);
        program_set_mat4(basic_program, "sprite.model", awesomeface_sprite_model);
        program_set_vec3(basic_program, "sprite.color", awesomeface_sprite->color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, awesomeface_sprite->image->texture);

        sprite_draw(awesomeface_sprite);

        program_unbind();

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    program_destroy(basic_program);

    // close engine
    sprite_quit();
    window_quit();
    engine_quit();

    return 0;
}
