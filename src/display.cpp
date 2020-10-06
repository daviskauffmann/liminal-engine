#include "display.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string>

const char *severity_to_string(GLenum severity)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "Notification";
    case GL_DEBUG_SEVERITY_HIGH:
        return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
    // fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
    //         (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    //         type, severity_to_string(severity), message);
}

namespace pk
{
    display::display(const char *title, int width, int height)
    {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
        SDL_version sdl_version;
        SDL_GetVersion(&sdl_version);
        std::cout << "SDL " << std::to_string(sdl_version.major) << "." << std::to_string(sdl_version.minor) << "." << std::to_string(sdl_version.patch) << std::endl;

        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        const SDL_version *img_version = IMG_Linked_Version();
        std::cout << "SDL_image " << std::to_string(img_version->major) << "." << std::to_string(img_version->minor) << "." << std::to_string(img_version->patch) << std::endl;

        Mix_Init(0);
        const SDL_version *mix_version = Mix_Linked_Version();
        std::cout << "SDL_mixer " << std::to_string(mix_version->major) << "." << std::to_string(mix_version->minor) << "." << std::to_string(mix_version->patch) << std::endl;

        window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        context = SDL_GL_CreateContext(window);

        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

        GLenum error = glewInit();
        if (error != GLEW_OK)
        {
            std::cout << "Error: Couldn't initialize GLEW\n"
                      << glewGetErrorString(error) << std::endl;
        }
        std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
        std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Vendor " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);

        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    display::~display()
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

    bool display::poll_event(SDL_Event *event)
    {
        bool result = SDL_PollEvent(event);

        if (result)
        {
            ImGui_ImplSDL2_ProcessEvent(event);
        }

        return result;
    }

    void display::set_title(const char *title)
    {
        SDL_SetWindowTitle(window, title);
    }

    void display::set_window_size(int width, int height)
    {
        SDL_SetWindowSize(window, width, height);
    }

    void display::toggle_fullscreen()
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

    void display::make_current()
    {
        SDL_GL_MakeCurrent(window, context);
    }

    void display::start_gui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
    }

    void display::end_gui()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void display::swap()
    {
        SDL_GL_SwapWindow(window);
    }
} // namespace pk
