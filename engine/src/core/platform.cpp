#include "core/platform.hpp"

#include <AL/al.h>
#include <GL/glew.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <sol/sol.hpp>

liminal::platform::platform(const std::string &window_title, int window_width, int window_height)
{
    // init SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(img_flags) != img_flags)
    {
        std::cerr << "Error: Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return;
    }

    int mix_flags = 0;
    if (Mix_Init(mix_flags) != mix_flags)
    {
        std::cerr << "Error: Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }

    // create window
    window = SDL_CreateWindow(
        window_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Error: Failed to create window: " << SDL_GetError() << std::endl;
        return;
    }

    // create OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    sdl_gl_context = SDL_GL_CreateContext(window);
    if (!sdl_gl_context)
    {
        std::cerr << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_GL_MakeCurrent(window, sdl_gl_context);

    // init GLEW
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cerr << "Error: Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
        return;
    }

    // init OpenAL
    al_device = alcOpenDevice(nullptr);
    if (!al_device)
    {
        std::cerr << "Error: Failed to open device" << std::endl;
        return;
    }

    al_context = alcCreateContext(al_device, nullptr);
    if (!al_context)
    {
        std::cerr << "Error: Failed to create context" << std::endl;
        return;
    }

    if (!alcMakeContextCurrent(al_context))
    {
        std::cerr << "Error: Failed to make context current" << std::endl;
        return;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        std::cerr << "Error: Failed to initialize the mixer API: " << Mix_GetError() << std::endl;
        return;
    }

    // init ImGui
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";
}

liminal::platform::~platform()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(al_context);
    alcCloseDevice(al_device);

    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(sdl_gl_context);

    IMG_Quit();
    Mix_Quit();

    SDL_Quit();
}

void liminal::platform::set_window_size(int width, int height)
{
    SDL_SetWindowSize(window, width, height);
}

void liminal::platform::toggle_fullscreen()
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

void liminal::platform::toggle_vsync()
{
    SDL_GL_SetSwapInterval(!SDL_GL_GetSwapInterval());
}

void liminal::platform::process_event(SDL_Event *event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void liminal::platform::begin_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void liminal::platform::end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}
