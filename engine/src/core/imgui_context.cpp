#include <liminal/core/imgui_context.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <stdexcept>

liminal::imgui_context::imgui_context(SDL_Window *const sdl_window, void *const sdl_gl_context)
    : sdl_window(sdl_window),
      sdl_gl_context(sdl_gl_context)
{
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = "data/imgui.ini";

    ImGui_ImplSDL2_InitForOpenGL(sdl_window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");
}

liminal::imgui_context::~imgui_context()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void liminal::imgui_context::handle_event(const SDL_Event &event) const
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void liminal::imgui_context::begin_frame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
    ImGui::NewFrame();
}

void liminal::imgui_context::end_frame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);
    }
}
