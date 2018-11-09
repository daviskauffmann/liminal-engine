#include <engine/engine.h>

#define WINDOW_TITLE "Breakout"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RENDER_SCALE 1.0f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

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
    core_init();

    window_init(
        WINDOW_TITLE,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    renderer_init(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        RENDER_SCALE,
        SHADOW_WIDTH,
        SHADOW_HEIGHT);

    // create textures
    struct texture *awesomeface_texture = texture_create("assets/images/awesomeface.png");

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

        // update sprite
        awesomeface_sprite->rotation += time_delta();

        // setup renderer
        renderer_add_sprite(awesomeface_sprite);

        // draw everything
        renderer_draw(true);

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    sprite_destroy(awesomeface_sprite);

    // close engine
    renderer_quit();
    window_quit();
    core_quit();

    return 0;
}
