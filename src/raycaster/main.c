#include <engine/engine.h>

#define WINDOW_TITLE "Raycaster"
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 200

#define NUM_TEXTURES 8
#define NUM_SPRITES 3
#define NUM_TRACKS 1
#define NUM_SOUNDS 1

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define NUM_BILLBOARDS 19

#define MOVE_SPEED 5.0f
#define SPRINT_MULT 2.0f
#define ROTATE_SPEED 3.0f
#define MOUSE_SENSITIVITY 2.0f

#define FLOOR_TEXTURE_MULT 1
#define CEILING_TEXTURE_MULT 1

#define FOG_STRENGTH 0.5f

#define FPS_CAP 60

#define FONT_SIZE 12

struct billboard
{
    float x;
    float y;
    unsigned char sprite_index;
    float sprite_scale_x;
    float sprite_scale_y;
    float sprite_translate_y;
};

struct player
{
    float pos_x;
    float pos_y;
    float dir_x;
    float dir_y;
    float plane_x;
    float plane_y;
};

void player_move(struct player *player, float dx, float dy);
void player_rotate(struct player *player, float angle);
void comb_sort(int *order, float *dist, int amount);
unsigned int color_darken(unsigned int color);
unsigned int color_fog(unsigned int color, float distance);

unsigned char wall_map[MAP_WIDTH][MAP_HEIGHT] = {
    {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
    {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}};

unsigned char floor_map[MAP_WIDTH][MAP_HEIGHT] = {
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}};

unsigned char ceiling_map[MAP_WIDTH][MAP_HEIGHT] = {
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}};

struct billboard billboards[NUM_BILLBOARDS] = {
    {20.5f, 11.5f, 2, 1.0f, 1.0f, 0.0f},
    {18.5f, 4.50f, 2, 1.0f, 1.0f, 0.0f},
    {10.0f, 4.50f, 2, 1.0f, 1.0f, 0.0f},
    {10.0f, 12.5f, 2, 1.0f, 1.0f, 0.0f},
    {3.50f, 6.50f, 2, 1.0f, 1.0f, 0.0f},
    {3.50f, 20.5f, 2, 1.0f, 1.0f, 0.0f},
    {3.50f, 14.5f, 2, 1.0f, 1.0f, 0.0f},
    {14.5f, 20.5f, 2, 1.0f, 1.0f, 0.0f},
    {18.5f, 10.5f, 1, 1.0f, 1.0f, 0.0f},
    {18.5f, 11.5f, 1, 1.0f, 1.0f, 0.0f},
    {18.5f, 12.5f, 1, 1.0f, 1.0f, 0.0f},
    {21.5f, 1.50f, 0, 1.0f, 1.0f, 0.0f},
    {15.5f, 1.50f, 0, 1.0f, 1.0f, 0.0f},
    {16.0f, 1.80f, 0, 1.0f, 1.0f, 0.0f},
    {16.2f, 1.20f, 0, 1.0f, 1.0f, 0.0f},
    {3.50f, 2.50f, 0, 1.0f, 1.0f, 0.0f},
    {9.50f, 15.5f, 0, 1.0f, 1.0f, 0.0f},
    {10.0f, 15.1f, 0, 1.0f, 1.0f, 0.0f},
    {10.5f, 15.8f, 0, 1.0f, 1.0f, 0.0f},
};

int main(int argc, char *args[])
{
    engine_init();

    window_sw_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);

    audio_init();

    struct bitmap *textures[NUM_TEXTURES];
    textures[0] = bitmap_create("assets/images/eagle.png");
    textures[1] = bitmap_create("assets/images/redbrick.png");
    textures[2] = bitmap_create("assets/images/purplestone.png");
    textures[3] = bitmap_create("assets/images/greystone.png");
    textures[4] = bitmap_create("assets/images/bluestone.png");
    textures[5] = bitmap_create("assets/images/mossy.png");
    textures[6] = bitmap_create("assets/images/wood.png");
    textures[7] = bitmap_create("assets/images/colorstone.png");

    struct bitmap *sprites[NUM_SPRITES];
    sprites[0] = bitmap_create("assets/images/barrel.png");
    sprites[1] = bitmap_create("assets/images/pillar.png");
    sprites[2] = bitmap_create("assets/images/greenlight.png");

    Mix_Music *tracks[NUM_TRACKS];
    tracks[0] = audio_music_load("assets/audio/background.mp3");

    Mix_Chunk *sounds[NUM_SOUNDS];
    sounds[0] = audio_chunk_load("assets/audio/shoot.wav");

    TTF_Font *font = font_open("assets/fonts/VeraMono.ttf", 24);

    bool fps_cap = true;
    bool textured = true;
    bool draw_walls = true;
    bool draw_floor = true;
    bool draw_billboards = true;
    bool shading = true;
    bool foggy = true;

    struct player *player = malloc(sizeof(struct player));

    player->pos_x = 22.0f;
    player->pos_y = 11.5f;
    player->dir_x = -1.0f;
    player->dir_y = 0.0f;
    player->plane_x = 0.0f;
    player->plane_y = 1.0f;

    printf("FOV: %d\n", (int)(2 * atanf(player->plane_y) / M_PI * 180));

    unsigned int *pixel_buffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(unsigned int));
    float *depth_buffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(float));

    time_cap_fps(FPS_CAP);

    io_set_relative_mouse(true);

    bool quit = false;
    while (!quit)
    {
        time_frame_start();

        int num_keys;
        const unsigned char *keys = io_keyboard(&num_keys);

        int mouse_x, mouse_y;
        unsigned int mouse = io_mouse(&mouse_x, &mouse_y);

        SDL_Event event;
        while (io_event(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEMOTION:
            {
                int mouse_dx = event.motion.xrel;

                float angle = -mouse_dx / 1000.0f * MOUSE_SENSITIVITY;

                player_rotate(player, angle);
            }
            break;
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_F1:
                {
                    fps_cap = !fps_cap;
                }
                break;
                case SDLK_F2:
                {
                    textured = !textured;
                }
                break;
                case SDLK_F3:
                {
                    draw_walls = !draw_walls;
                }
                break;
                case SDLK_F4:
                {
                    draw_floor = !draw_floor;
                }
                break;
                case SDLK_F5:
                {
                    draw_billboards = !draw_billboards;
                }
                break;
                case SDLK_F6:
                {
                    shading = !shading;
                }
                break;
                case SDLK_F7:
                {
                    foggy = !foggy;
                }
                break;
                case SDLK_1:
                {
                    if (audio_music_playing())
                    {
                        audio_music_stop();
                    }
                    else
                    {
                        audio_music_play(tracks[0], -1);
                    }
                }
                break;
                case SDLK_2:
                {
                    if (audio_music_playing())
                    {
                        if (audio_music_paused())
                        {
                            audio_music_resume();
                        }
                        else
                        {
                            audio_music_pause();
                        }
                    }
                }
                break;
                case SDLK_TAB:
                {
                    io_set_relative_mouse(!io_get_relative_mouse());
                }
                break;
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        window_sw_toggle_fullscreen();
                    }
                }
                break;
                }
            }
            break;
            case SDL_QUIT:
            {
                quit = true;
            }
            break;
            }
        }

        float speed = MOVE_SPEED * time_delta();

        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= SPRINT_MULT;
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
            float dx = player->dir_x * speed;
            float dy = player->dir_y * speed;

            player_move(player, dx, dy);
        }

        if (keys[SDL_SCANCODE_A])
        {
            float dx = -player->dir_y * speed;
            float dy = player->dir_x * speed;

            player_move(player, dx, dy);
        }

        if (keys[SDL_SCANCODE_S])
        {
            float dx = -player->dir_x * speed;
            float dy = -player->dir_y * speed;

            player_move(player, dx, dy);
        }

        if (keys[SDL_SCANCODE_D])
        {
            float dx = player->dir_y * speed;
            float dy = -player->dir_x * speed;

            player_move(player, dx, dy);
        }

        float angle = ROTATE_SPEED * time_delta();

        if (keys[SDL_SCANCODE_Q])
        {
            player_rotate(player, angle);
        }

        if (keys[SDL_SCANCODE_E])
        {
            player_rotate(player, -angle);
        }

        static float shoot_timer = 0.0f;
        shoot_timer += time_delta();
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                audio_chunk_play(-1, sounds[0], 0);
            }
        }

        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            for (int y = 0; y < WINDOW_HEIGHT; y++)
            {
                pixel_buffer[x + y * WINDOW_WIDTH] = 0;
                depth_buffer[x + y * WINDOW_WIDTH] = FLT_MAX;
            }

            float camera_x = (2.0f * x / WINDOW_WIDTH) - 1;

            float ray_dir_x = (camera_x * player->plane_x) + player->dir_x;
            float ray_dir_y = (camera_x * player->plane_y) + player->dir_y;

            int map_x = (int)player->pos_x;
            int map_y = (int)player->pos_y;

            float side_dist_x;
            float side_dist_y;

            float delta_dist_x = fabsf(1 / ray_dir_x);
            float delta_dist_y = fabsf(1 / ray_dir_y);

            int step_x;
            int step_y;

            if (ray_dir_x < 0)
            {
                side_dist_x = (player->pos_x - map_x) * delta_dist_x;
                step_x = -1;
            }
            else
            {
                side_dist_x = (map_x + 1 - player->pos_x) * delta_dist_x;
                step_x = 1;
            }
            if (ray_dir_y < 0)
            {
                side_dist_y = (player->pos_y - map_y) * delta_dist_y;
                step_y = -1;
            }
            else
            {
                side_dist_y = (map_y + 1 - player->pos_y) * delta_dist_y;
                step_y = 1;
            }

            int side;

            while (true)
            {
                if (side_dist_x < side_dist_y)
                {
                    map_x += step_x;
                    side_dist_x += delta_dist_x;
                    side = 0;
                }
                else
                {
                    map_y += step_y;
                    side_dist_y += delta_dist_y;
                    side = 1;
                }

                if (wall_map[map_x][map_y] > 0)
                {
                    break;
                }
            }

            float perp_wall_dist =
                side == 0
                    ? (map_x - player->pos_x + (1 - step_x) / 2) / ray_dir_x
                    : (map_y - player->pos_y + (1 - step_y) / 2) / ray_dir_y;

            int line_height = (int)(WINDOW_HEIGHT / perp_wall_dist);

            int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
            if (draw_start < 0)
            {
                draw_start = 0;
            }
            int draw_end = line_height / 2 + WINDOW_HEIGHT / 2;
            if (draw_end >= WINDOW_HEIGHT)
            {
                draw_end = WINDOW_HEIGHT - 1;
            }

            if (textured)
            {
                float wall_x;
                if (side == 0)
                {
                    wall_x = player->pos_y + perp_wall_dist * ray_dir_y;
                }
                else
                {
                    wall_x = player->pos_x + perp_wall_dist * ray_dir_x;
                }
                wall_x -= floorf(wall_x);

                if (draw_walls)
                {
                    int texture_index = wall_map[map_x][map_y] - 1;
                    struct bitmap *texture = textures[texture_index];

                    int texture_x = (int)(wall_x * texture->width);
                    if (side == 0 && ray_dir_x > 0)
                    {
                        texture_x = texture->width - texture_x - 1;
                    }
                    if (side == 1 && ray_dir_y < 0)
                    {
                        texture_x = texture->width - texture_x - 1;
                    }

                    for (int y = draw_start; y <= draw_end; y++)
                    {
                        int texture_y = (((y * 256 - WINDOW_HEIGHT * 128 + line_height * 128) * texture->height) / line_height) / 256;

                        unsigned int color = bitmap_get_pixel(texture, texture_x, texture_y);

                        if (shading && side == 1)
                        {
                            color = color_darken(color);
                        }

                        if (foggy)
                        {
                            color = color_fog(color, perp_wall_dist);
                        }

                        pixel_buffer[x + y * WINDOW_WIDTH] = color;
                        depth_buffer[x + y * WINDOW_WIDTH] = perp_wall_dist;
                    }
                }

                if (draw_floor)
                {
                    float floor_x_wall;
                    float floor_y_wall;

                    if (side == 0 && ray_dir_x > 0)
                    {
                        floor_x_wall = (float)map_x;
                        floor_y_wall = map_y + wall_x;
                    }
                    else if (side == 0 && ray_dir_x < 0)
                    {
                        floor_x_wall = (float)(map_x + 1);
                        floor_y_wall = map_y + wall_x;
                    }
                    else if (side == 1 && ray_dir_y > 0)
                    {
                        floor_x_wall = map_x + wall_x;
                        floor_y_wall = (float)map_y;
                    }
                    else // if (side == 1 && ray_dir_y < 0)
                    {
                        floor_x_wall = map_x + wall_x;
                        floor_y_wall = (float)(map_y + 1);
                    }

                    if (draw_end < 0)
                    {
                        draw_end = WINDOW_HEIGHT;
                    }

                    for (int y = draw_end + 1; y < WINDOW_HEIGHT; y++)
                    {
                        float current_dist = WINDOW_HEIGHT / (2.0f * y - WINDOW_HEIGHT);
                        float weight = current_dist / perp_wall_dist;

                        float current_x = weight * floor_x_wall + (1 - weight) * player->pos_x;
                        float current_y = weight * floor_y_wall + (1 - weight) * player->pos_y;

                        {
                            int texture_index = floor_map[(int)current_x][(int)current_y];
                            struct bitmap *texture = textures[texture_index];

                            int texture_x = (int)(current_x * texture->width / FLOOR_TEXTURE_MULT) % texture->width;
                            int texture_y = (int)(current_y * texture->height / FLOOR_TEXTURE_MULT) % texture->height;

                            unsigned int color = bitmap_get_pixel(texture, texture_x, texture_y);

                            if (foggy)
                            {
                                color = color_fog(color, current_dist);
                            }

                            pixel_buffer[x + y * WINDOW_WIDTH] = color;
                            depth_buffer[x + y * WINDOW_WIDTH] = current_dist;
                        }

                        {
                            int texture_index = ceiling_map[(int)current_x][(int)current_y];
                            struct bitmap *texture = textures[texture_index];

                            int texture_x = (int)(current_x * texture->width / CEILING_TEXTURE_MULT) % texture->width;
                            int texture_y = (int)(current_y * texture->height / CEILING_TEXTURE_MULT) % texture->height;

                            unsigned int color = bitmap_get_pixel(texture, texture_x, texture_y);

                            if (foggy)
                            {
                                color = color_fog(color, current_dist);
                            }

                            if (shading)
                            {
                                color = color_darken(color);
                            }

                            pixel_buffer[x + (WINDOW_HEIGHT - y) * WINDOW_WIDTH] = color;
                            depth_buffer[x + (WINDOW_HEIGHT - y) * WINDOW_WIDTH] = current_dist;
                        }
                    }
                }
            }
            else
            {
                if (draw_walls)
                {
                    unsigned int color;
                    switch (wall_map[map_x][map_y])
                    {
                    case 0:
                        color = 0xff00ffff;
                        break;
                    case 1:
                        color = 0xff0000ff;
                        break;
                    case 2:
                        color = 0xff00ff00;
                        break;
                    case 3:
                        color = 0xffff0000;
                        break;
                    case 4:
                        color = 0xffff00ff;
                        break;
                    case 5:
                        color = 0xffffff00;
                        break;
                    default:
                        color = 0xffffffff;
                        break;
                    }

                    if (shading && side == 1)
                    {
                        color = color_darken(color);
                    }

                    if (foggy)
                    {
                        color = color_fog(color, perp_wall_dist);
                    }

                    for (int y = draw_start; y <= draw_end; y++)
                    {
                        pixel_buffer[x + y * WINDOW_WIDTH] = color;
                        depth_buffer[x + y * WINDOW_WIDTH] = perp_wall_dist;
                    }
                }

                if (draw_floor)
                {
                    unsigned int floor_color = 0xff646464;
                    unsigned int ceiling_color = floor_color;

                    if (shading)
                    {
                        ceiling_color = color_darken(ceiling_color);
                    }

                    for (int y = draw_end + 1; y < WINDOW_HEIGHT; y++)
                    {
                        pixel_buffer[x + y * WINDOW_WIDTH] = floor_color;
                        depth_buffer[x + y * WINDOW_WIDTH] = perp_wall_dist;
                    }

                    for (int y = 0; y < draw_start; y++)
                    {
                        pixel_buffer[x + y * WINDOW_WIDTH] = ceiling_color;
                        depth_buffer[x + y * WINDOW_WIDTH] = perp_wall_dist;
                    }
                }
            }
        }

        if (draw_billboards)
        {
            int billboard_order[NUM_BILLBOARDS];
            float billboard_dist[NUM_BILLBOARDS];

            for (int i = 0; i < NUM_BILLBOARDS; i++)
            {
                billboard_order[i] = i;
                billboard_dist[i] = powf(player->pos_x - billboards[i].x, 2) + powf(player->pos_y - billboards[i].y, 2);
            }
            comb_sort(billboard_order, billboard_dist, NUM_BILLBOARDS);

            for (int i = 0; i < NUM_BILLBOARDS; i++)
            {
                struct billboard billboard = billboards[billboard_order[i]];

                float billboard_x = billboard.x - player->pos_x;
                float billboard_y = billboard.y - player->pos_y;

                float inv_det = 1 / (player->plane_x * player->dir_y - player->dir_x * player->plane_y);

                float transform_x = inv_det * (player->dir_y * billboard_x - player->dir_x * billboard_y);
                float transform_y = inv_det * (-player->plane_y * billboard_x + player->plane_x * billboard_y);

                int object_screen_x = (int)((WINDOW_WIDTH / 2) * (1 + transform_x / transform_y));

                int object_width = abs((int)(WINDOW_HEIGHT / transform_y * billboard.sprite_scale_x));
                int object_height = abs((int)(WINDOW_HEIGHT / transform_y * billboard.sprite_scale_y));

                int draw_start_x = -object_width / 2 + object_screen_x;
                if (draw_start_x < 0)
                {
                    draw_start_x = 0;
                }
                int draw_end_x = object_width / 2 + object_screen_x;
                if (draw_end_x >= WINDOW_WIDTH)
                {
                    draw_end_x = WINDOW_WIDTH - 1;
                }

                int translate_y = (int)(billboard.sprite_translate_y / transform_y);

                int draw_start_y = -object_height / 2 + WINDOW_HEIGHT / 2 + translate_y;
                if (draw_start_y < 0)
                {
                    draw_start_y = 0;
                }
                int draw_end_y = object_height / 2 + WINDOW_HEIGHT / 2 + translate_y;
                if (draw_end_y >= WINDOW_HEIGHT)
                {
                    draw_end_y = WINDOW_HEIGHT - 1;
                }

                // float angle = atan2f(billboard_y, billboard_x);

                struct bitmap *sprite = sprites[billboard.sprite_index];

                for (int x = draw_start_x; x < draw_end_x; x++)
                {
                    int sprite_x = (256 * (x - (-object_width / 2 + object_screen_x)) * sprite->width / object_width) / 256;

                    if (transform_y > 0 && x > 0 && x < WINDOW_WIDTH)
                    {
                        for (int y = draw_start_y; y < draw_end_y; y++)
                        {
                            if (transform_y < depth_buffer[x + y * WINDOW_WIDTH])
                            {
                                int sprite_y = ((((y - translate_y) * 256 - WINDOW_HEIGHT * 128 + object_height * 128) * sprite->height) / object_height) / 256;

                                unsigned int color = bitmap_get_pixel(sprite, sprite_x, sprite_y);

                                if (foggy)
                                {
                                    color = color_fog(color, transform_y);
                                }

                                if ((color & 0x00ffffff) != 0)
                                {
                                    // unsigned int previous_color = pixel_buffer[x + y * w];

                                    pixel_buffer[x + y * WINDOW_WIDTH] = color;
                                    depth_buffer[x + y * WINDOW_WIDTH] = transform_y;
                                }
                            }
                        }
                    }
                }
            }
        }

        window_sw_draw_pixels(NULL, pixel_buffer, WINDOW_WIDTH * sizeof(unsigned int));

        {
            int line = 0;

            window_sw_draw_text(
                font,
                FONT_SIZE,
                0,
                FONT_SIZE * line++,
                (SDL_Color){255, 255, 255, 255},
                "FPS: %d",
                time_fps());

            window_sw_draw_text(
                font,
                FONT_SIZE,
                0,
                FONT_SIZE * line++,
                (SDL_Color){255, 255, 255, 255},
                "Pos: (%f, %f)",
                player->pos_x,
                player->pos_y);

            window_sw_draw_text(
                font,
                FONT_SIZE,
                0,
                FONT_SIZE * line++,
                (SDL_Color){255, 255, 255, 255},
                "Dir: (%f, %f)",
                player->dir_x,
                player->dir_y);

            window_sw_draw_text(
                font,
                FONT_SIZE,
                0,
                FONT_SIZE * line++,
                (SDL_Color){255, 255, 255, 255},
                "Plane: (%f, %f)",
                player->plane_x,
                player->plane_y);
        }

        window_sw_render();

        time_frame_end();
    }

    // free resources
    free(depth_buffer);
    free(pixel_buffer);

    free(player);

    font_close(font);

    for (int i = 0; i < NUM_TRACKS; i++)
    {
        audio_music_free(tracks[i]);
    }
    for (int i = 0; i < NUM_SOUNDS; i++)
    {
        audio_chunk_free(sounds[i]);
    }

    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        bitmap_destroy(textures[i]);
    }
    for (int i = 0; i < NUM_SPRITES; i++)
    {
        bitmap_destroy(sprites[i]);
    }

    window_sw_quit();
    engine_quit();

    return 0;
}

void player_move(struct player *player, float dx, float dy)
{
    if (wall_map[(int)(player->pos_x + dx)][(int)(player->pos_y)] == 0)
    {
        player->pos_x += dx;
    }
    if (wall_map[(int)(player->pos_x)][(int)(player->pos_y + dy)] == 0)
    {
        player->pos_y += dy;
    }
}

void player_rotate(struct player *player, float angle)
{
    float rot_x = cosf(angle);
    float rot_y = sinf(angle);

    float old_dir_x = player->dir_x;
    player->dir_x = player->dir_x * rot_x - player->dir_y * rot_y;
    player->dir_y = old_dir_x * rot_y + player->dir_y * rot_x;

    float old_plane_x = player->plane_x;
    player->plane_x = player->plane_x * rot_x - player->plane_y * rot_y;
    player->plane_y = old_plane_x * rot_y + player->plane_y * rot_x;
}

void comb_sort(int *order, float *dist, int amount)
{
    int gap = amount;
    bool swapped = false;
    while (gap > 1 || swapped)
    {
        gap = (gap * 10) / 13;
        if (gap == 9 || gap == 10)
        {
            gap = 11;
        }
        if (gap < 1)
        {
            gap = 1;
        }

        swapped = false;
        for (int i = 0; i < amount - gap; i++)
        {
            int j = i + gap;
            if (dist[i] < dist[j])
            {
                float temp_dist = dist[i];
                dist[i] = dist[j];
                dist[j] = temp_dist;

                int temp_order = order[i];
                order[i] = order[j];
                order[j] = temp_order;

                swapped = true;
            }
        }
    }
}

unsigned int color_darken(unsigned int color)
{
    return (color >> 1) & 0x7f7f7f;
}

unsigned int color_fog(unsigned int color, float distance)
{
    int red = (color >> 16) & 0x0ff;
    int green = (color >> 8) & 0x0ff;
    int blue = color & 0x0ff;

    float fog_intensity = distance * FOG_STRENGTH;

    if (fog_intensity > 1)
    {
        float redf = (float)red;
        float greenf = (float)green;
        float bluef = (float)blue;

        redf /= fog_intensity;
        greenf /= fog_intensity;
        bluef /= fog_intensity;

        red = (int)redf;
        green = (int)greenf;
        blue = (int)bluef;
    }

    return ((red & 0x0ff) << 16) | ((green & 0x0ff) << 8) | (blue & 0x0ff);
}
