#include <engine/engine.h>

static SDL_Window *window;
static SDL_GLContext context;
struct program *basic_program;
struct program *phong_program;
struct texture *default_texture;
struct mesh *quad_mesh;
struct mesh *cube_mesh;
struct material *default_material;

int window_init(const char *title, int width, int height)
{
    // create window
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        error(SDL_GetError());

        return 1;
    }

    // create OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    context = SDL_GL_CreateContext(window);

    if (!context)
    {
        error(SDL_GetError());

        return 1;
    }

    // init GLEW
    {
        GLenum glewError = glewInit();

        if (glewError != GLEW_OK)
        {
            error(glewGetErrorString(glewError));

            return 1;
        }
    }

    info("GLEW %s", glewGetString(GLEW_VERSION));
    info("OpenGL %s", glGetString(GL_VERSION));

    // configure OpenGL
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_TEXTURE_2D);

    // create shader programs
    basic_program = program_create(
        "../engine/assets/shaders/basic.vs",
        "../engine/assets/shaders/basic.fs");

    if (!basic_program)
    {
        return 1;
    }

    phong_program = program_create(
        "../engine/assets/shaders/phong.vs",
        "../engine/assets/shaders/phong.fs");

    if (!phong_program)
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

    quad_mesh = mesh_create(
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

    cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        return 1;
    }

    // create textures
    default_texture = texture_create("../engine/assets/images/default.png");

    if (!default_texture)
    {
        return 1;
    }

    // create materials
    default_material = material_create(
        default_texture,
        NULL,
        NULL,
        (vec3){1.0f, 1.0f, 1.0f},
        32.0f,
        1.0f);

    if (!default_material)
    {
        return 1;
    }

    return 0;
}

const char *window_get_title(void)
{
    return SDL_GetWindowTitle(window);
}

void window_set_title(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

void window_get_size(int *width, int *height)
{
    SDL_GetWindowSize(window, width, height);
}

float window_get_aspect(void)
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    return (float)width / (float)height;
}

unsigned int window_get_flags(void)
{
    return SDL_GetWindowFlags(window);
}

void window_toggle_fullscreen(void)
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

void window_resize(int width, int height)
{
    SDL_SetWindowSize(window, width, height);
    glViewport(0, 0, width, height);

    info("Window resized to %dx%d", width, height);
}

void window_clear(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_swap(void)
{
    SDL_GL_SwapWindow(window);
}

void window_quit(void)
{
    material_destroy(default_material);
    texture_destroy(default_texture);
    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);
    program_destroy(phong_program);
    program_destroy(basic_program);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
}
