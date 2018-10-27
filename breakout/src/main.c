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

static enum game_state game_stage;

int main(int argc, char *argv[])
{
    // init engine
    if (core_init())
    {
        return 1;
    }

    if (window_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
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

    // cache uniform locations
    GLint basic_program_camera_projection = program_get_location(basic_program, "camera.projection");
    GLint basic_program_object_model = program_get_location(basic_program, "object.model");
    GLint basic_program_sprite_color = program_get_location(basic_program, "sprite.color");
    GLint basic_program_sprite_image = program_get_location(basic_program, "sprite.image");

    // setup shader samplers
    program_bind(basic_program);
    program_set_int(basic_program_sprite_image, 0);
    program_unbind();

    // create textures
    struct texture *awesomeface_texture = texture_create("assets/images/awesomeface.png");

    if (!awesomeface_texture)
    {
        return 1;
    }

    // sprite stuff
    vec3 sprite_color = { 1.0f, 1.0f, 1.0f };

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

        // calculate camera projection
        mat4 camera_projection;
        glm_ortho_default(window_get_aspect(), camera_projection);

        // calculate object model
        mat4 object_model = GLM_MAT4_IDENTITY_INIT;

        program_bind(basic_program);

        program_set_mat4(basic_program_camera_projection, camera_projection);
        program_set_mat4(basic_program_object_model, object_model);
        program_set_vec3(basic_program_sprite_color, sprite_color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, awesomeface_texture->texture);

        sprite_draw();

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
    core_quit();

    return 0;
}
