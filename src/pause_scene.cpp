#include "pause_scene.hpp"

#include <imgui.h>

namespace pk
{
    pause_scene::pause_scene(pk::scene *paused_scene)
        : paused_scene(paused_scene)
    {
    }

    pause_scene::~pause_scene()
    {
    }

    scene *pause_scene::handle_event(SDL_Event event)
    {
        ImGuiIO &io = ImGui::GetIO();

        switch (event.type)
        {
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                if (!io.WantCaptureKeyboard)
                {
                    pk::scene *return_scene = paused_scene;
                    delete this;
                    return return_scene;
                }
            }
            break;
            }
        }
        }

        return this;
    }

    scene *pause_scene::update(pk::audio *audio, float delta_time)
    {
        return this;
    }

    void pause_scene::render(pk::renderer *renderer) const
    {
        renderer->greyscale = true;

        paused_scene->render(renderer);
    }

    void pause_scene::gui() const
    {
    }

    void pause_scene::print_commands()
    {
    }

    void pause_scene::handle_command(const char *command)
    {
    }
} // namespace pk
