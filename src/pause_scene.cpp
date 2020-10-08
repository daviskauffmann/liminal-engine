#include "pause_scene.hpp"

#include <imgui.h>

pk::pause_scene::pause_scene(pk::scene *paused_scene)
    : paused_scene(paused_scene)
{
    delete_paused_scene = true;
}

pk::pause_scene::~pause_scene()
{
    if (delete_paused_scene)
    {
        delete paused_scene;
    }
}

pk::scene *pk::pause_scene::handle_event(SDL_Event event)
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

                delete_paused_scene = false;
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

pk::scene *pk::pause_scene::update(pk::audio *audio, float delta_time)
{
    return this;
}

void pk::pause_scene::render(pk::renderer *renderer) const
{
    renderer->greyscale = true;

    paused_scene->render(renderer);
}

void pk::pause_scene::gui() const
{
}
