#include <liminal/core/platform.hpp>

#include <AL/al.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <iostream>
#include <sol/sol.hpp>

liminal::platform *liminal::platform::instance = nullptr;

liminal::platform::platform(const char *const window_title, const int window_width, const int window_height)
{
    instance = this;

    // init SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    const int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(img_flags) != img_flags)
    {
        std::cerr << "Error: Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return;
    }

    const int mix_flags = 0;
    if (Mix_Init(mix_flags) != mix_flags)
    {
        std::cerr << "Error: Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }

    // create window
    window = SDL_CreateWindow(
        window_title,
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
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_GL_MakeCurrent(window, context);

    // init GLEW
    const auto error = glewInit();
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

    auto &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = "assets/imgui.ini";

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
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
    SDL_GL_DeleteContext(context);

    IMG_Quit();
    Mix_Quit();

    SDL_Quit();
}

void liminal::platform::set_window_title(const char *const title) const
{
    SDL_SetWindowTitle(window, title);
}

void liminal::platform::set_window_size(const int width, const int height) const
{
    SDL_SetWindowSize(window, width, height);
}

void liminal::platform::toggle_fullscreen() const
{
    const auto flags = SDL_GetWindowFlags(window);
    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_SetWindowFullscreen(window, 0);
    }
    else
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

void liminal::platform::toggle_vsync() const
{
    SDL_GL_SetSwapInterval(!SDL_GL_GetSwapInterval());
}

bool liminal::platform::get_relative_mouse_mode() const
{
    return SDL_GetRelativeMouseMode();
}

void liminal::platform::set_relative_mouse_mode(const bool enabled) const
{
    SDL_SetRelativeMouseMode((SDL_bool)enabled);
}

void liminal::platform::process_event(const SDL_Event *const event) const
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void liminal::platform::begin_frame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void liminal::platform::end_frame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        SDL_GL_MakeCurrent(window, context);
    }

    SDL_GL_SwapWindow(window);
}
