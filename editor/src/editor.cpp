#include <imgui.h>
#include <iostream>
#include <liminal.hpp>

class editor : public liminal::app
{
    void update(float delta_time) override
    {
        // ImGui::Begin("Scene Hierarchy");

        // for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
        // {
        //     ImGui::Text("%s", transform.name);
        // }

        // ImGui::End();
    }
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new editor();
}
