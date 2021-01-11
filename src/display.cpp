#include "display.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>
#include <string>

pk::display::display(const char *title, int width, int height)
{
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
        return;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        spdlog::error("Failed to initialize GLEW: {}", glewGetErrorString(error));
        return;
    }

    // TODO: error handling?
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
}

pk::display::~display()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);
}

bool pk::display::poll_event(SDL_Event *event)
{
    bool result = SDL_PollEvent(event);

    if (result)
    {
        ImGui_ImplSDL2_ProcessEvent(event);
    }

    return result;
}

void pk::display::set_title(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

void pk::display::set_window_size(int width, int height)
{
    SDL_SetWindowSize(window, width, height);
}

void pk::display::toggle_fullscreen()
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

void pk::display::make_current()
{
    SDL_GL_MakeCurrent(window, context);
}

void pk::display::start_gui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void pk::display::end_gui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void pk::display::swap()
{
    SDL_GL_SwapWindow(window);
}
