#include "platform.hpp"

#include <GL/glew.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <sol/sol.hpp>

liminal::platform::platform(const char *title, int width, int height)
{
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

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Error: Failed to create window: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cerr << "Error: Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
        return;
    }

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        std::cerr << "Error: Failed to initialize the mixer API: " << Mix_GetError() << std::endl;
        return;
    }
}

liminal::platform::~platform()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);

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

void liminal::platform::process_event(SDL_Event *event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void liminal::platform::begin_render()
{
    SDL_GL_MakeCurrent(window, context);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void liminal::platform::end_render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}
